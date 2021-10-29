#pragma once

/*
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
// TODO: Implement examples

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer the following features to the programmer without adding too much processing overhead
// *

//*****************************************************        LIBRARIES        *****************************************************/

#include "utils.h"

// * Terminal
#include "libraries/terminal/terminal.h"

// * Real Time Clock
#include "libraries/real_time_clock/rtc.h"

// TODO: Implement weekday functionality in update time function
// TODO: Implement string return functionality

// * System on Chip
#include "libraries/soc/soc.h"

// * Analog to Digital Converters
#include "libraries/adc/ad7689/ad7689.h"

// TODO: Clean up & convert library to new format

// * CAN bus
#include "libraries/can/chips/MCP2518FD/MCP2518FD.h"

// * Bluetooth Low Energy
#include "libraries/bluetooth/bluetooth_low_energy/ble.h"

// TODO: Implement JSON & SPIFFS config setting
// TODO: Implement Bluetooth Server Mode (multiconnect?)
// TODO: Implement Bluetooth Client Mode (multiconnect?)
// TODO: Pass client object to send data function
// TODO: Add library documentation: https://learn.adafruit.com/the-well-automated-arduino-library/doxygen-tips

// * Memory
#include "libraries/memory/spiffs/spiffs_memory.h"
// TODO: Creat SPIFFS if not initialized

#include "libraries/memory/emmc/emmc_memory.h"
// TODO: Implement SD Card library example
// TODO: Add library documentation: https://learn.adafruit.com/the-well-automated-arduino-library/doxygen-tips
// TODO: Implement list directory functionality

// * I/O Expansion
#include "libraries/io_expansion/SX1509.h"

// TODO: Creat SPIFFS if not initialized
//  End.
