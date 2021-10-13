/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: ESP32 System on Module
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: September 23, 2021
*/

//*****************************************************        LIBRARIES        *****************************************************/
#include "gas_bottle_alarm/app/app.h"
#include "tasks/rtos/rtos.h"
#include "tasks/terminal/terminal.h"
#include "tasks/wake_up_manager/deep_sleep.h"
#include "tasks/settings/settings.h"
#include "tasks/pairing/device_pairing.h"
#include "tasks/bluetooth_server/ble_server.h"
#include "tasks/led_strip/led_strip.h"
#include "tasks/main_app/main_app.h"
#include "tasks/time_keeper/time_keeper.h"

//*****************************************************       DATA TYPES        *****************************************************/

//*****************************************************         OBJECTS         *****************************************************/
DeviceSettings device_settings;

Terminal terminal;
SystemOnChip esp;
BluetoothLowEnergyServer bleServer;
SPIFFS_Memory spiffsMemory;
RealTimeClock rtc;

Adafruit_NeoPixel led_strip(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

//*********************************************************       APP       *********************************************************/
void BottleBirdApp::begin()
{
    //* 1. Begin UART port for debug output
    esp.uart0.begin(UART0_BAUD_RATE);

    //* Print App title
    esp.uart0.println("\n\n");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                          ESP32 System on Module                                          *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    //* 2. Setup RTOS first. Always.
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

    //* 3. Create tasks if RTOS was setup correctly
    xTaskCreatePinnedToCore(setupTerminal,
                            "Terminal Setup",
                            10000,
                            NULL,
                            25,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(setupLED_Strip,
                            "LED Setup",
                            1000,
                            NULL,
                            25,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(setupDeepSleepManager,
                            "Deep Sleep Setup",
                            10000,
                            NULL,
                            24,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(setupTimeKeeper,
                            "Time Keeper",
                            10000,
                            NULL,
                            22,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(setupSPIFFS,
                            "Deep Sleep Setup",
                            30000,
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

    xTaskCreatePinnedToCore(startMainApp,
                            "Main app",
                            10000,
                            NULL,
                            21,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(bleServerTask,
                            "BLE server Task",
                            10000,
                            NULL,
                            21,
                            NULL,
                            0);

    vTaskDelete(NULL); // Delete setup & loop RTOS tasks.
}

// End.