#include "device_pairing.h"
#include "../terminal/terminal.h"
#include "../bluetooth_server/ble_server.h"
#include "../settings/settings.h"

#define PAIRING_TIMEOUT_S 30

void pairingTask(void *parameters)
{
    TerminalMessage pairing_debug_message;
    uint8_t debug_message_queue_ticks = 0;

    //* 1. Await binary semaphore
    xSemaphoreTake(app.rtos.start_pairing, portMAX_DELAY);

    pairing_debug_message = TerminalMessage("Starting pair up process", "PAI", INFO, micros());
    addDebugMessageToQueue(&pairing_debug_message, debug_message_queue_ticks);

    //* 2. Start the bluetooth server task
    xSemaphoreGive(app.rtos.start_ble_server);

    //* 3. Create a task that awaits an instruction. If it never get's it, the sleep timer will
    //*    expire & the device will go back to sleep

    TerminalMessage debug_message;
    String incoming_instruction;

    while (1)
    {
        //* 1. Await initial pairing request
        if (xQueueReceive(app.rtos.ble_rx_message_queue, (void *)&incoming_instruction, portMAX_DELAY) == pdTRUE)
        {
            String temp = incoming_instruction;

            if (temp == "PAIRING")
            {
                debug_message = TerminalMessage("Pairing started",
                                                "PAI", INFO, micros());

                bool initial_pairing_time = millis();

                app.device_settings.device_is_setup = true;

                ESP_ERROR save_settings_on_setup = saveSettings();

                TerminalMessage setup_debug_message;

                if (save_settings_on_setup.on_error)
                {
                    setup_debug_message = TerminalMessage(save_settings_on_setup.debug_message,
                                                          "PAI", ERROR, micros(), micros() - initial_pairing_time);
                }
                else
                {
                    setup_debug_message = TerminalMessage("Device was setup correctly",
                                                          "PAI", INFO, micros(), micros() - initial_pairing_time);
                }

                addDebugMessageToQueue(&setup_debug_message);

                //* Start main
                xSemaphoreGive(app.rtos.start_main); // * Start Main App if paired
                vTaskDelete(NULL);
            }
            else
            {
                debug_message = TerminalMessage("Not pairing commmand",
                                                "PAI", WARNING, micros());
            }

            addDebugMessageToQueue(&debug_message);
        }
    }

    vTaskDelete(NULL);
}
