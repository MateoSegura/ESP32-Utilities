/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: ESP32 System on Module
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: October 9, 2021
*/

#include <Arduino.h>
#include <esp32_utilities.h>

//* CAN FD Message
#define CAN_SEND_MESSAGE_ID 0x770

//* MCP2518 CS & INT pins
#define CAN0_CONTROLLER_CS_PIN 27
#define CAN0_CONTROLLER_INT_PIN 35

//* Connected through VSPI bus at 80 Mhz max
#define VSPI_MOSI_PIN 23
#define VSPI_MISO_PIN 19
#define VSPI_CLCK_PIN 18

//* Utilities Objects
SystemOnChip esp;

//* Terminal
Terminal terminal;
static QueueHandle_t debug_message_queue = NULL;
static SemaphoreHandle_t debug_message_queue_mutex = NULL;
static uint16_t debug_message_queue_length = 10;

//* CAN controller
MCP2518FD canController(CAN0_CONTROLLER_CS_PIN, esp.vspi, CAN0_CONTROLLER_INT_PIN);

//* CAN Send & receive tasks
void CAN_BusSendMessageTask(void *parameters);
void CAN_BusReceiveMessageTask(void *parameters);

//* Terminal task
void TerminalTask(void *parameters);
void addDebugMessageToQueue(TerminalMessage *message, uint16_t port_ticks);

//* Setup
void setup()
{
    // Initialize UART port & Terminal Object
    esp.uart0.begin(115200);
    terminal.begin(&esp.uart0);

    // Print App title
    esp.uart0.println("\n\n");
    esp.uart0.println("*******************************************************************************************");
    esp.uart0.println("*                                    CAN FD Test Bench                                    *"); // Replace with your title
    esp.uart0.println("*******************************************************************************************");
    esp.uart0.println("\n\n");

    // Initialize VSPI port at 40 Mhz
    esp.vspi.begin(VSPI_CLCK_PIN, VSPI_MISO_PIN, VSPI_MOSI_PIN); // No CS pin. Used in MCP2518FD

    // Create CAN settings object
    long initial_time = micros();
    MCP2518FDSettings settings(MCP2518FDSettings::OSC_40MHz, //Oscillator specification
                               1000 * 1000,
                               DataBitRateFactor::x8);

    uint32_t can_error_code = canController.begin(settings, // Settings
                                                  []
                                                  { canController.isr(); }); //TODO: What's this

    if (can_error_code != 0)
    {
        terminal.printMessage(TerminalMessage("CAN initialization error: " + String(can_error_code, HEX),
                                              "CAN",
                                              ERROR,
                                              micros(),
                                              micros() - initial_time));
        vTaskDelete(NULL); // Stop boot
    }

    terminal.printMessage(TerminalMessage("CAN controller initializated ",
                                          "CAN",
                                          INFO,
                                          micros(),
                                          micros() - initial_time));

    //* Initialize RTOS variables
    debug_message_queue = xQueueCreate(debug_message_queue_length, sizeof(TerminalMessage)); // Queue
    debug_message_queue_mutex = xSemaphoreCreateMutex();                                     // Mutex

    //* Create Send Task:
    //  It will output an error message if the buffer in the controller is full
    //  This means you're not connected to any other node
    //*

    xTaskCreatePinnedToCore(CAN_BusSendMessageTask,
                            "Send Task",
                            10000,
                            NULL,
                            2,
                            NULL,
                            1);

    //* Create Receive Task:
    //  It will output an error message if the buffer in the controller is full
    //  This means you're not connected to any other node
    //*

    // xTaskCreatePinnedToCore(CAN_BusReceiveMessageTask,
    //                         "Receive Task",
    //                         10000,
    //                         NULL,
    //                         1,
    //                         NULL,
    //                         0);

    xTaskCreatePinnedToCore(TerminalTask,
                            "TerminalTask",
                            1000,
                            NULL,
                            1,
                            NULL,
                            0);

    vTaskDelete(NULL);
}

//* Setup & Loop tasks deleted
void loop() {}

//* Send message with CAN ID defined above
void CAN_BusSendMessageTask(void *parameters)
{
    while (1)
    {
        long initial_time = micros();
        TerminalMessage send_debug_message;

        // 1. Create CAN message
        CANFDMessage can_message;
        can_message.id = CAN_SEND_MESSAGE_ID;
        can_message.len = 32;

        // 2. Populate with data
        for (uint8_t i = 0; i < can_message.len; i++)
        {
            can_message.data[i] = i * 2;
        }

        // 3. Try to send
        bool can_send_success = canController.tryToSend(can_message);

        if (!can_send_success)
        {
            send_debug_message = TerminalMessage("CAN data send buffer is full",
                                                 "CAN",
                                                 ERROR,
                                                 micros(),
                                                 micros() - initial_time);
        }
        else
        {
            send_debug_message = TerminalMessage("Message sent. ID -> 0x" + String(can_message.id, HEX) + "\tData Length -> " + String(can_message.len) + " bytes",
                                                 "CAN",
                                                 ERROR,
                                                 micros(),
                                                 micros() - initial_time);
        }

        terminal.printMessage(send_debug_message);

        CANFDMessage can_receive_message;

        TerminalMessage receive_debug_message;

        while (canController.receive(can_receive_message))
        {
            receive_debug_message.body = "[ID:0x";
            receive_debug_message.body += can_receive_message.id;

            if (can_receive_message.ext)
            {
                receive_debug_message.body += "[EXT:";
                receive_debug_message.body += "YES] ";
            }
            else
            {
                receive_debug_message.body += "[EXT:";
                receive_debug_message.body += "NO] ";
            }

            receive_debug_message.body += "[TYPE:";
            switch (can_receive_message.type)
            {
            case 0:
                receive_debug_message.body += "CAN REMOTE";
                break;
            case 1:
                receive_debug_message.body += "CAN DATA";
                break;
            case 2:
                receive_debug_message.body += "CANFD NO BRS";
                break;
            case 3:
                receive_debug_message.body += "CANFD BRS";
                break;
            default:
                break;
            }
            receive_debug_message.body += "] ";

            receive_debug_message.body += " DATA =  ";
            for (uint8_t i = 0; i < can_receive_message.len; i++)
            {
                receive_debug_message.body += can_receive_message.data[i];
                receive_debug_message.body += " ";
            }

            receive_debug_message.system = "CAN";
            receive_debug_message.core = xPortGetCoreID();
            receive_debug_message.time = micros();
            receive_debug_message.process_time = micros() - initial_time;

            terminal.printMessage(receive_debug_message);
        }
        //vTaskDelay(5 / portTICK_PERIOD_MS);

        //vTaskDelay(100 / portTICK_PERIOD_MS); //Send appx. every 100 ms
    }
}

//* Terminal
void TerminalTask(void *parameters)
{
    //* Terminal output
    TerminalMessage debug_message;

    while (1)
    {
        if (xQueueReceive(debug_message_queue, (void *)&debug_message, portMAX_DELAY) == pdTRUE)
        {
            terminal.printMessage(debug_message);
        }
    }
}

void addDebugMessageToQueue(TerminalMessage *message, uint16_t port_ticks)
{
    xSemaphoreTake(debug_message_queue_mutex, port_ticks);
    xQueueSend(debug_message_queue, (void *)message, 0);
    xSemaphoreGive(debug_message_queue_mutex);
}

// End.