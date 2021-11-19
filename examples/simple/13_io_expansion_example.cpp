/*
 * File Name: soc_example.cpp
 * Hardware needed: No extra hardware needed, just a good ol' ESP32 dev. board
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

#include <Arduino.h>
#include <esp32_utilities.h>

//******************** READ ME

//******************** SETTINGS

//* UART0
#define UART0_BAUD_RATE 115200        // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED true // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false     // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

//* I/O Expansion
#define IO_EXPANSION_INPUT_PIN 0 // Connect pin 0 of the expansion to GPIO 26 of the ESP
#define IO_EXPANSION_INT_PIN 13  // Input only. Need's external pull up resistor
#define SX1509_I2C_ADDR 0x3E     // ADDR0 & ADDR1 are set to 0

//* I2C
#define I2C0_SDA_PIN 21
#define I2C0_SCL_PIN 22
#define I2C0_FREQUENCY 3400000 // KHz

//******************** CUSTOM OBJECTS
struct GPIO_Write
{
    uint8_t pin;
    bool state;
};

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;
SX1509 io_expansion;

//******************** RTOS OBJECTS
QueueHandle_t io_expansion_interrupt_queue = NULL;
uint16_t io_expansion_interrupt_queue_length = 10;

//******************** INTERRUPTS
int interrupt_time = 0;
bool request = false;
bool state = false;

static void IRAM_ATTR externalIO_Interrupt()
{
    interrupt_time = micros();
    state = !state;
    digitalWrite(2, state);
    xQueueSend(io_expansion_interrupt_queue, (void *)&request, 0);
}

//******************** RTOS TASKS
void handleExtInterrupt(void *parameters)
{
    GPIO_Write gpio_action;
    long initial_time;

    while (1)
    {
        // Read if available first
        if (xQueueReceive(io_expansion_interrupt_queue, (void *)&gpio_action, portMAX_DELAY) == pdTRUE)
        {
            initial_time = micros();
            uint8_t interrupt_pin = (io_expansion.interruptSource() - 1); // Function returns pin from 1 to 16

            terminal.printMessage(TerminalMessage("Interrupt delay: " + String(micros() - interrupt_time) + " uS" + "\tPin: " + String(interrupt_pin),
                                                  "I/O", INFO, micros(), micros() - initial_time));

            attachInterrupt(IO_EXPANSION_INT_PIN, externalIO_Interrupt, FALLING);
        }
    }
}

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                       System on Chip Example                                             *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);

    // 3. Init I2C bus
    esp.i2c0.begin(I2C0_SDA_PIN, I2C0_SCL_PIN, I2C0_FREQUENCY); // Refer to soc_example.cpp for information on this function

    // 4. Init I/O expansion
    ESP_ERROR initialize_io_expansion = io_expansion.begin(&esp.i2c0, SX1509_I2C_ADDR);

    if (initialize_io_expansion.on_error) // Catch error
    {
        while (1)
        {
            terminal.printMessage(TerminalMessage(initialize_io_expansion.debug_message, "I/O", ERROR, micros()));
            delay(500);
        }
    }

    terminal.printMessage(TerminalMessage("SX1509 External I/O initialized correctly", "I/O", ERROR, micros()));

    // 5. Create RTOS objects for GPIO handle task
    io_expansion_interrupt_queue = xQueueCreate(io_expansion_interrupt_queue_length, sizeof(bool));

    // 6. Create a Hardware Interrupt on INT pin of EXT I/O
    pinMode(IO_EXPANSION_INT_PIN, INPUT);
    attachInterrupt(IO_EXPANSION_INT_PIN, externalIO_Interrupt, FALLING);

    // 7. Setup External Input 0 as input pull up
    io_expansion.pinMode(IO_EXPANSION_INPUT_PIN, INPUT_PULLUP);

    // 8. Attach interrupt
    io_expansion.enableInterrupt(IO_EXPANSION_INPUT_PIN, FALLING);

    // 9
    xTaskCreatePinnedToCore(handleExtInterrupt,
                            "Ext. IO",
                            10000,
                            NULL,
                            25,
                            NULL,
                            1);

    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH);

    while (1)
    {
        delay(500);
        digitalWrite(27, LOW);
        digitalWrite(27, HIGH);
    }

    vTaskDelete(NULL);
}

//********************  LOOP
void loop()
{
}

//* End