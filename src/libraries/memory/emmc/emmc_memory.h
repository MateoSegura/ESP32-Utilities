#pragma once

/*
 * Company: ANZE Suspension
 * File Name: ESP32UtilitiesEMMC.h
 * Project: ESP32 Utilities EMMC
 * Version: 1.0
 * Compartible Hardware:
 * Date Created: September 8, 2021
 * Last Modified: September 9, 2021
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library was to offer a high level of abstraction for interacting with an eMMC type memory
// *
// * Great sources to learn more about eMMC & how it interacts with the ESP:
// *
// * https://www.reddit.com/r/esp32/comments/d71es9/a_breakdown_of_my_experience_trying_to_talk_to_an/
// * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/sdmmc_host.html
// * https://github.com/espressif/arduino-esp32/tree/master/libraries/SD_MMC/examples
// *

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
#define SD_POWER_UP_DELAY_mS 10 // eMMC IC power up time. This is entirely dependent on the chip you use.

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>
#include "FS.h"
#include "SD.h"
#include "SD_MMC.h"
#include "SPI.h"
#include <iostream>
#include <string>
#include <vector>
#include <utils.h>

//*****************************************************       DATA TYPES        *****************************************************/
//  -- Bus type
enum eMMC_CONNECTION_MODE
{
    eMMC_MODE,
    SPI_MODE
};

// -- eMMC bus width
enum eMMC_BUS_WIDTH
{
    MODE_1_BIT,
    MODE_4_BIT,
};

//*****************************************************       EMMC CLASS        *****************************************************/
class EMMC_Memory
{
public:
    EMMC_Memory()
    {
        emmc_initialized = false;
        emmc_detected = false;
    }

    // -- Initialize SD Card
    ESP_ERROR begin(uint8_t enable_pin, // Gate of P-Channel MOSFET (power switch) for eMMC memory or uSD Card
                    uint8_t detect_pin, // Card detect pin. Available in most uSD Card connectors
                    eMMC_CONNECTION_MODE mode = eMMC_MODE,
                    eMMC_BUS_WIDTH bus_width = MODE_1_BIT);

    // -- Directory Operations
    ESP_ERROR listDirectory(const char *dirname, uint8_t levels); // Not implemented yet
    ESP_ERROR makeDirectory(const char *path);
    ESP_ERROR removeDirectory(const char *path);

    // File Operations
    ESP_ERROR readFile(const char *path);
    ESP_ERROR writeFile(const char *path, char *data, uint16_t length);
    ESP_ERROR appendFile(const char *path, const char *data);
    ESP_ERROR renameFile(const char *path1, const char *path2);
    ESP_ERROR deleteFile(const char *path);

    ESP_ERROR onDetectPinChange();

    // -- Card operations
    uint64_t getEMMCsize()
    {
        return emmc_memory_size;
    }

    uint64_t getTotalMemorySpace()
    {
        return emmc_total_memory_space;
    }

    uint64_t getUsedMemorySpace()
    {
        return emmc_used_memory_space;
    }

    // -- State Operations
    bool isInitialized()
    {
        return emmc_initialized;
    }

    bool setDetected(bool state)
    {
        emmc_detected = state;
        emmc_initialized = state;
    }

private:
    // -- Initialization Variables
    eMMC_CONNECTION_MODE connection_mode;
    eMMC_BUS_WIDTH emmc_bus_width;
    SPIClass emmc_spi;
    uint8_t emmc_enable_pin;
    uint8_t emmc_detect_pin;

    uint8_t flush_count;

    // -- Card Info
    long emmc_memory_size;
    long emmc_total_memory_space;
    long emmc_used_memory_space;

    // -- Debugging
    bool emmc_initialized;
    bool emmc_detected;

    // -- File system
    fs::FS *file_system;
    File myFile;

    // -- THESE PINS CANNOT BE CHANGED
    static const byte esp_emmc_data0 = 2;
    static const byte esp_emmc_dat1 = 4;
    static const byte esp_emmc_dat2 = 12;
    static const byte esp_emmc_dat3 = 13;
    static const byte esp32_emmc_clk = 14;
    static const byte esp32_emmc_cmd = 15;
};

// End.
