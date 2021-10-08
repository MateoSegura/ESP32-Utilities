#pragma once
/*
* Company: ANZE Suspension
* File Name: rtos.h
* Project: ESP32 System on Module
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
#include <system_on_module/app/app.h>

//*****************************************************        FUNCTIONS        *****************************************************/

ESP_ERROR terminalRTOS();

//********************************************************        MAIN        ********************************************************/
ESP_ERROR Application::setupRTOS()
{
    ESP_ERROR err;

    //* Terminal
    ESP_ERROR terminal_rtos = terminalRTOS();
    if (terminal_rtos.on_error)
        return err;

    return err;
}

//******************************************************      DECLARATION        *******************************************************/
ESP_ERROR terminalRTOS()
{
    ESP_ERROR err;

    app.rtos.terminal_message_queue = xQueueCreate(app.rtos.terminal_message_queue_length, sizeof(TerminalMessage)); // Queue
    app.rtos.terminal_message_queue_mutex = xSemaphoreCreateMutex();                                                 // Mutex

    app.rtos.terminal_file_queue = xQueueCreate(app.rtos.terminal_file_queue_length, sizeof(String)); // Queue
    app.rtos.terminal_file_queue = xSemaphoreCreateMutex();                                           // Mutex

    if (app.rtos.terminal_message_queue == NULL || app.rtos.terminal_message_queue == NULL ||
        app.rtos.terminal_file_queue == NULL || app.rtos.terminal_file_queue_mutex == NULL)
    {
        err.on_error = true;
        err.debug_message = "Could not create Terminal messages queue objects.";
    }
    return err;
}