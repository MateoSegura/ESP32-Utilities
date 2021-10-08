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
#include "app.h"

#include "tasks/rtos/rtos.h"
#include "tasks/terminal/terminal.h"
#include "tasks/save_data/save_data.h"
#include "tasks/read_can/read_can.h"

//*****************************************************       DATA TYPES        *****************************************************/

//*****************************************************         OBJECTS         *****************************************************/
SystemOnChip esp;
Terminal terminal;
BluetoothLowEnergyServer bleServer;
SPIFFS_Memory spiffsMemory;
EMMC_Memory emmcMemory;

MCP2518FD CAN(CAN0_CONTROLLER_CS_PIN, esp.vspi, CAN0_CONTROLLER_INT_PIN);

//*********************************************************       APP       *********************************************************/
void Application::begin()
{
    //* 1. Begin UART port for debug output
    esp.uart0.begin(115200);

    //* 2. Print App title
    esp.uart0.println("\n\n");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                          ESP32 System on Module                                          *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    //* 3. Setup RTOS first. Always.
    ESP_ERROR rtos_initialized = setupRTOS();

    if (rtos_initialized.on_error)
    {
        esp.uart0.println(rtos_initialized.debug_message);
        esp.uart0.println("\n\n");
        esp.uart0.println("Stopping boot.");
        while (1)
            ;
    }

    //* 4. Setup terminal
    xTaskCreatePinnedToCore(setupTerminal,
                            "Terminal Setup",
                            10000,
                            NULL,
                            25,
                            NULL,
                            0);

    xTaskCreatePinnedToCore(setupSaveDataTask,
                            "Save Data Setup",
                            10000,
                            NULL,
                            25,
                            NULL,
                            1);

    xTaskCreatePinnedToCore(setupCANbusTask,
                            "Can bus read",
                            10000,
                            NULL,
                            25,
                            NULL,
                            0);

    vTaskDelete(NULL);
}
