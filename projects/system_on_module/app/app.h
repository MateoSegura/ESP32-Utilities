#pragma once
/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: ESP32 System on Module
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: September 23, 2021
*/

//*********************************************************     READ ME    **********************************************************/

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>

#include "../device/device_pinout.h"
#include "../device/soc_settings.h"

#include <esp32_utilities.h>

//*****************************************************         OBJECTS         *****************************************************/
extern SystemOnChip esp;
extern Terminal terminal;
extern BluetoothLowEnergyServer bleServer;
extern SPIFFS_Memory spiffsMemory;
extern EMMC_Memory emmcMemory;
extern MCP2518FD CAN;

//*****************************************************       DATA TYPES        *****************************************************/

class ApplicationSettings
{
public:
    //TODO: Implement settings
    //TODO: Implement settings from & to JSON
};

//**************************************************        RTOS VARIABLES        ***************************************************/
class ApplicationRTOS_Objects
{
public:
    // * Terminal
    QueueHandle_t terminal_message_queue = NULL;
    SemaphoreHandle_t terminal_message_queue_mutex = NULL;
    uint16_t terminal_message_queue_length = 50;

    QueueHandle_t terminal_file_queue = NULL;
    SemaphoreHandle_t terminal_file_queue_mutex = NULL;
    uint16_t terminal_file_queue_length = 50;
};

//*************************************************           INTERRUPTS           **************************************************/

//*************************************************       TASKS DECLARATION        **************************************************/

//*********************************************************       APP       *********************************************************/
class Application
{
public:
    ApplicationSettings settings;

    ApplicationRTOS_Objects rtos;
    ESP_ERROR setupRTOS(); // Defined in "tasks/rtos.h"

    //* App begin
    void begin();
};

extern Application app; // This is your main app. Initiate in "main.cpp"

// End.