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
#include <esp32_utilities.h>

#include "../device/soc_settings.h"
#include "../device/soc_pinout.h"

#include <Adafruit_NeoPixel.h>

//*****************************************************         OBJECTS         *****************************************************/
extern SystemOnChip esp;
extern Terminal terminal;
extern BluetoothLowEnergyServer bleServer;
extern SPIFFS_Memory spiffsMemory;
extern Adafruit_NeoPixel led_strip;

//*****************************************************       DATA TYPES        *****************************************************/

#define SETTINGS_FILE_SIZE_BYTES 3000
#define SETTINGS_FILE "/settings.json"

#define MINIMUM_BAT_VOLTAGE_FOR_BOOT_mV 3700

#define NUMPIXELS 12
#define LED_BLINKING_PERIOD_mS 30

class DeviceSettings
{
public:
    bool first_boot;
    bool device_is_setup;
    const char *wifi_ssid;
    const char *wifi_password;
    const char *ble_service_uuid;
    const char *ble_characteristic_uuid;

    ESP_ERROR fromJSON(JsonDocument &settings_json)
    {
        ESP_ERROR err;
        err.on_error = false;

        //https://arduinojson.org/v6/assistant/

        first_boot = settings_json["first_boot"];
        device_is_setup = settings_json["device_setup"];
        wifi_ssid = settings_json["wifi_ssid"];
        wifi_password = settings_json["wifi_password"];
        ble_service_uuid = settings_json["ble_service_uuid"];
        ble_characteristic_uuid = settings_json["ble_characteristic_uuid"];

        //TODO: Check errors
        return err;
    }

    ESP_ERROR toJSON(JsonDocument &settings_json)
    {
        ESP_ERROR err;
        err.on_error = false;

        settings_json["first_boot"] = first_boot;
        settings_json["device_setup"] = device_is_setup;
        settings_json["wifi_ssid"] = wifi_ssid;
        settings_json["wifi_password"] = wifi_password;
        settings_json["ble_service_uuid"] = ble_service_uuid;
        settings_json["ble_characteristic_uuid"] = ble_characteristic_uuid;

        return err;
    }

private:
};

//**************************************************        RTOS VARIABLES        ***************************************************/
class ApplicationRTOS_Objects
{
public:
    // * Boot & setup
    SemaphoreHandle_t start_main = NULL;
    SemaphoreHandle_t start_pairing = NULL;

    // * Terminal
    QueueHandle_t debug_message_queue = NULL;
    SemaphoreHandle_t debug_message_queue_mutex = NULL;
    uint16_t debug_message_queue_length = 50;

    QueueHandle_t file_print_queue = NULL;
    SemaphoreHandle_t file_print_queue_mutex = NULL;
    uint16_t file_print_queue_length = 5;

    // * Bluetooth
    SemaphoreHandle_t start_ble_server = NULL;
    SemaphoreHandle_t handle_ble_instruction = NULL;

    QueueHandle_t ble_tx_message_queue = NULL;           // Bluetooth Low Energy Server TX
    SemaphoreHandle_t ble_tx_message_queue_mutex = NULL; // Bluetooth Low Energy Server TX
    uint16_t ble_tx_message_queue_length = 100;          // Bluetooth Low Energy Server TX

    QueueHandle_t ble_rx_message_queue = NULL;           // Bluetooth Low Energy Server RXs
    SemaphoreHandle_t ble_rx_message_queue_mutex = NULL; // Bluetooth Low Energy Server RX
    uint16_t ble_rx_message_queue_length = 100;          // Bluetooth Low Energy Server RX
};
//*************************************************           INTERRUPTS           **************************************************/

//*************************************************       TASKS DECLARATION        **************************************************/

//*********************************************************       APP       *********************************************************/
class BottleBirdApp
{

public:
    DeviceSettings device_settings;

    ApplicationRTOS_Objects rtos;
    ESP_ERROR setupRTOS(); // Defined in "tasks/rtos.h"

    //* App begin
    void begin();
};

extern BottleBirdApp app; // This is the main app. Initiated in "main.cpp"

// End.