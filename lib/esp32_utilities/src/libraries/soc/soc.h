#pragma once
/* 
* Company: ANZE Suspension
* File Name: ESP32UtilitiesBLEserver.h
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer some abstraction for ESP32 system on chip.

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
// * None.

//*****************************************************        LIBRARIES        *****************************************************/
#include <utils.h>
#include <Arduino.h>
#include "Wire.h" // I2C
#include <SPI.h>  // SPI
#include "timer/esp_timer.h"

//*****************************************************       DATA TYPES        *****************************************************/
enum ESP_CPU
{
    CPU_0,
    CPU_1,
};

//************************************************      SYSTEM ON CHIP CLASS        *************************************************/
class SystemOnChip
{
public:
    // * SoC Manager
    EspClass utilities;

    // * SoC CPUs
    ESP_CPU cpu0 = CPU_0;
    ESP_CPU cpu1 = CPU_1;

    // * Timers
    ESPtimer timer0 = ESPtimer(0);
    ESPtimer timer1 = ESPtimer(1);
    ESPtimer timer2 = ESPtimer(2);
    ESPtimer timer3 = ESPtimer(3);

    // *  UART
    HardwareSerial uart0 = Serial;

    // * I2C
    TwoWire i2c0 = Wire;
    TwoWire i2c1 = Wire1;

    // * SPI
    SPIClass vspi = VSPI;
    SPIClass hspi = HSPI;

private:
};

//  End.