#pragma once

/* 
* Company: ANZE Suspension
* File Name: Terminal.h
* Project: ESP32 Utilities Terminal
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//* To do list:
//TODO: Implement examples

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer the following features to the programmer without adding too much processing overhead
// *

//*****************************************************        LIBRARIES        *****************************************************/

// * Terminal
#include "libraries/terminal/terminal.h"

// * Real Time Clock
#include "libraries/real_time_clock/rtc.h"

//TODO: Implement weekday functionality in update time function
//TODO: Implement string return functionality

// * System on Chip
#include "libraries/soc/soc.h"

// * Analog to Digital Converters
#include "libraries/adc/chips/ads1015/ads1015.h"
#include "libraries/adc/chips/ard1867/Ard1863.h"
#include "libraries/adc/chips/ltc1867/ltc1867.h"

//TODO: Clean up & convert library to new format

// * Bluetooth Low Energy
#include "libraries/bluetooth/bluetooth_low_energy/ble.h"

//TODO: Implement JSON & SPIFFS config setting
//TODO: Implement Bluetooth Server Mode (multiconnect?)
//TODO: Implement Bluetooth Client Mode (multiconnect?)
//TODO: Pass client object to send data function
//TODO: Add library documentation: https://learn.adafruit.com/the-well-automated-arduino-library/doxygen-tips

// End.
