#include "read_can.h"
#include "../terminal/terminal.h"

void setupCANbusTask(void *parameters)
{

    long initial_time = micros();
    TerminalMessage can_debug_message;
    uint8_t debug_queue_port_ticks;

    esp.vspi.begin(VSPI_CLCK_PIN, VSPI_MISO_PIN, VSPI_MOSI_PIN); // Begin SPI bus
    //esp.vspi.setFrequency(VSPI_CLCK_FREQUENCY_MHz * 1000);       // Set SPI frequency

    MCP2518FDSettings settings(MCP2518FDSettings::OSC_40MHz, 1000 * 1000, DataBitRateFactor::x8); // CAN controller settings

    settings.mDriverReceiveFIFOSize = 32; // Setup CAN controller receiving queue size4

    // Refer to library manual for error codes
    const uint32_t err = CAN.begin(settings, []
                                   { CAN.isr(); }); // TODO: Read exception about ESP32 interrupt

    if (err != 0)
    {
        can_debug_message = TerminalMessage("Could not initialize CAN controller. Error: " + String(err), // message
                                            "CAN", ERROR, micros(),                                       // System, message type, timestamp
                                            micros() - initial_time);                                     // Process time

        addDebugMessageToQueue(&can_debug_message);

        vTaskDelete(NULL); // Delete task
    }

    // If initialized correctly
    can_debug_message = TerminalMessage("CAN controller initialized", // message
                                        "CAN", ERROR, micros(),       // System, message type, timestamp
                                        micros() - initial_time);     // Process time

    addDebugMessageToQueue(&can_debug_message);

    CANFDMessage frame;
    uint32_t received_frame_count = 0;

    while (1)
    {
        if (CAN.available())
        {
            CAN.receive(frame);
            received_frame_count++;
            Serial.print("Received: ");
            Serial.println(received_frame_count);

            can_debug_message = TerminalMessage("ID: " + String(frame.id, HEX) + ". Data -> " + frame.data[0], // message
                                                "CAN", ERROR, micros(),                                        // System, message type, timestamp
                                                micros() - initial_time);                                      // Process time

            addDebugMessageToQueue(&can_debug_message);
        }
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }

    //TODO: setup can receive & send functions
    vTaskDelete(NULL);
}