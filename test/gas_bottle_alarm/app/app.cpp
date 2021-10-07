//* Terminal
#include "gas_bottle_alarm/app/app.h"
#include "gas_bottle_alarm/app/tasks/terminal/terminal.h"
#include "gas_bottle_alarm/app/tasks/deep_sleep/deep_sleep.h"
#include "gas_bottle_alarm/app/tasks/settings/settings.h"
#include "gas_bottle_alarm/app/tasks/pairing/device_pairing.h"
#include "gas_bottle_alarm/app/tasks/bluetooth_server/ble_server.h"

Terminal terminal;
SystemOnChip esp;
BluetoothLowEnergyServer bleServer;
SPIFFS_Memory spiffsMemory;

DeviceSettings BottleBirdSettings;

void BottleBirdApp::begin()
{
    //* Begin UART port for debug output
    esp.uart0.begin(UART0_BAUD_RATE);

    //* Print App title
    esp.uart0.println("\n\n");
    esp.uart0.println("************************************************************************");
    esp.uart0.println("*                      ANZE - Gas Bottle Alarm                         *"); // Replace with your title
    esp.uart0.println("************************************************************************");
    esp.uart0.println("\n\n");

    //* Setup RTOS first. Always.
    ESP_ERROR rtos_initialized = setupRTOS();

    if (rtos_initialized.on_error)
    {
        esp.uart0.println(rtos_initialized.debug_message);
        esp.uart0.println("\n\n");
        esp.uart0.println("Stopping boot.");
        while (1)
        {
        }
    }

    //* Create tasks if RTOS was setup correctly
    xTaskCreatePinnedToCore(setupTerminal,
                            "Terminal Setup",
                            10000,
                            NULL,
                            25,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(setupDeepSleepManager,
                            "Deep Sleep Setup",
                            10000,
                            NULL,
                            24,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(setupSPIFFS,
                            "Deep Sleep Setup",
                            10000,
                            NULL,
                            23,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(pairingTask,
                            "Pairing Task",
                            10000,
                            NULL,
                            22,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(bleServerTask,
                            "BLE server Task",
                            10000,
                            NULL,
                            22,
                            NULL,
                            0);

    vTaskDelete(NULL); // Delete setup & loop RTOS tasks.
}

ESP_ERROR BottleBirdApp::setupRTOS()
{
    ESP_ERROR err;
    err.on_error = false;

    //* Device boot & setup
    start_main = xSemaphoreCreateMutex();
    start_pairing = xSemaphoreCreateMutex();

    if (start_main == NULL || start_pairing == NULL)
    {
        err.on_error = true;
        err.debug_message = "Could not create boot semaphores.";
        while (1)
        {
        }
    }

    xSemaphoreTake(start_main, 0);
    xSemaphoreTake(start_pairing, 0);

    //* Terminal
    debug_message_queue = xQueueCreate(debug_message_queue_length, sizeof(TerminalMessage)); // Queue
    debug_message_queue_mutex = xSemaphoreCreateMutex();                                     // Mutex

    file_print_queue = xQueueCreate(file_print_queue_length, sizeof(String)); // Queue
    file_print_queue_mutex = xSemaphoreCreateMutex();                         // Mutex

    if (debug_message_queue == NULL || debug_message_queue_mutex == NULL ||
        file_print_queue == NULL || file_print_queue_mutex == NULL)
    {
        err.on_error = true;
        err.debug_message = "Could not create Terminal messages queue objects.";
        return err;
    }

    //* Bluetooth
    start_ble_server = xSemaphoreCreateMutex();
    handle_ble_instruction = xSemaphoreCreateMutex();

    if (start_ble_server == NULL || handle_ble_instruction == NULL)
    {
        err.on_error = true;
        err.debug_message = "Could not create ble semaphore.";
        while (1)
        {
        }
    }
    xSemaphoreTake(start_ble_server, 0);
    xSemaphoreTake(handle_ble_instruction, 0);

    ble_tx_message_queue = xQueueCreate(ble_tx_message_queue_length, sizeof(String)); // TX Queue
    ble_rx_message_queue = xQueueCreate(ble_rx_message_queue_length, sizeof(String)); // RX Queue
    ble_tx_message_queue_mutex = xSemaphoreCreateMutex();                             // TX Mutex
    ble_rx_message_queue_mutex = xSemaphoreCreateMutex();                             // RX Mutex

    if ((ble_tx_message_queue == NULL || ble_tx_message_queue_mutex == NULL) ||
        (ble_rx_message_queue == NULL || ble_rx_message_queue_mutex == NULL))
    {
        err.on_error = true;
        err.debug_message = "Could not create Bluetooth Low Energy messages queue objects.";
        while (1)
        {
        }
    }

    return err;
}