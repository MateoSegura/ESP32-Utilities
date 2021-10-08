#pragma once
/*
* File Name: rtos.h
* Project: Gas Bottle Alarm
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: September 23, 2021
*/

//*********************************************************     READ ME    **********************************************************/
/*
*   How does RTOS task work?
*   
*   Before the application begins, all RTOS variables are initialized. This is necessary for your application to even work in the
*   first place. 
* 
*   - Mateo 
*/

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>
#include <gas_bottle_alarm/app/app.h>

//*****************************************************        FUNCTIONS        *****************************************************/

ESP_ERROR deviceSettingsRTOS();
ESP_ERROR terminalRTOS();
ESP_ERROR bluetoothRTOS();

//********************************************************        MAIN        ********************************************************/

ESP_ERROR BottleBirdApp::setupRTOS()
{
    ESP_ERROR err;

    ESP_ERROR device_settings_rtos = deviceSettingsRTOS();
    if (device_settings_rtos.on_error)
        return device_settings_rtos;

    ESP_ERROR terminal_rtos = terminalRTOS();
    if (terminal_rtos.on_error)
        return terminal_rtos;

    ESP_ERROR bluetooth_rtos = bluetoothRTOS();
    if (bluetooth_rtos.on_error)
        return bluetooth_rtos;

    return err;
}

//******************************************************      DECLARATION        *******************************************************/
ESP_ERROR deviceSettingsRTOS()

{
    ESP_ERROR err;

    app.rtos.start_main = xSemaphoreCreateMutex();
    app.rtos.start_pairing = xSemaphoreCreateMutex();

    if (app.rtos.start_main == NULL || app.rtos.start_pairing == NULL)
    {
        err.on_error = true;
        err.debug_message = "Could not create boot semaphores.";
    }

    xSemaphoreTake(app.rtos.start_main, 0);
    xSemaphoreTake(app.rtos.start_pairing, 0);
    return err;
}
ESP_ERROR terminalRTOS()
{
    ESP_ERROR err;

    app.rtos.debug_message_queue = xQueueCreate(app.rtos.debug_message_queue_length, sizeof(TerminalMessage)); // Queue
    app.rtos.debug_message_queue_mutex = xSemaphoreCreateMutex();                                              // Mutex

    app.rtos.file_print_queue = xQueueCreate(app.rtos.file_print_queue_length, sizeof(String)); // Queue
    app.rtos.file_print_queue_mutex = xSemaphoreCreateMutex();                                  // Mutex

    if (app.rtos.debug_message_queue == NULL || app.rtos.debug_message_queue_mutex == NULL ||
        app.rtos.file_print_queue == NULL || app.rtos.file_print_queue_mutex == NULL)
    {
        err.on_error = true;
        err.debug_message = "Could not create Terminal messages queue objects.";
    }

    return err;
}

ESP_ERROR bluetoothRTOS()
{
    ESP_ERROR err;

    app.rtos.start_ble_server = xSemaphoreCreateMutex();
    app.rtos.handle_ble_instruction = xSemaphoreCreateMutex();

    if (app.rtos.start_ble_server == NULL || app.rtos.handle_ble_instruction == NULL)
    {
        err.on_error = true;
        err.debug_message = "Could not create ble semaphore.";
    }

    xSemaphoreTake(app.rtos.start_ble_server, 0);
    xSemaphoreTake(app.rtos.handle_ble_instruction, 0);

    //* Message queues
    app.rtos.ble_tx_message_queue = xQueueCreate(app.rtos.ble_tx_message_queue_length, sizeof(String)); // TX Queue
    app.rtos.ble_rx_message_queue = xQueueCreate(app.rtos.ble_rx_message_queue_length, sizeof(String)); // RX Queue
    app.rtos.ble_tx_message_queue_mutex = xSemaphoreCreateMutex();                                      // TX Mutex
    app.rtos.ble_rx_message_queue_mutex = xSemaphoreCreateMutex();                                      // RX Mutex

    if ((app.rtos.ble_tx_message_queue == NULL || app.rtos.ble_tx_message_queue_mutex == NULL) ||
        (app.rtos.ble_rx_message_queue == NULL || app.rtos.ble_rx_message_queue_mutex == NULL))
    {
        err.on_error = true;
        err.debug_message = "Could not create Bluetooth Low Energy messages queue objects.";
    }

    return err;
}
