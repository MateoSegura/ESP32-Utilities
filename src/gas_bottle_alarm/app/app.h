#pragma once
/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: Bottle Bird
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: September 23, 2021
*/

//TODO: Fix settings.cpp
//*********************************************************     READ ME    **********************************************************/

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>
#include "gas_bottle_alarm/device/soc_settings.h"
#include <esp32_utilities.h>

//*****************************************************       DATA TYPES        *****************************************************/
struct DeviceSettings
{
    bool first_boot;
    bool device_setup;
    const char *wifi_ssid;
    const char *wifi_password;
};

extern RTC_DATA_ATTR DeviceSettings BottleBirdSettings;
//*****************************************************         OBJECTS         *****************************************************/
extern SystemOnChip esp;
extern Terminal terminal;
extern BluetoothLowEnergyServer bleServer;
extern SPIFFS_Memory spiffsMemory;

//**************************************************        RTOS VARIABLES        ***************************************************/

//*************************************************           INTERRUPTS           **************************************************/

//*************************************************       TASKS DECLARATION        **************************************************/

//*********************************************************       APP       *********************************************************/
class BottleBirdApp
{

public:
    // * RTOS
    QueueHandle_t debug_message_queue = NULL;           // Terminal
    SemaphoreHandle_t debug_message_queue_mutex = NULL; // Terminal
    uint16_t debug_message_queue_length = 100;          // Terminal

    QueueHandle_t file_print_queue = NULL;           // Terminal
    SemaphoreHandle_t file_print_queue_mutex = NULL; // Terminal
    uint16_t file_print_queue_length = 100;          // Terminal

    void begin();

private:
    ESP_ERROR setupRTOS();
};

extern BottleBirdApp app;