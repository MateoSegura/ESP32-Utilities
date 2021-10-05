#pragma once

/* 
* Company: ANZE Suspension
* File Name: ESP32UtilitiesSPIFFS.h
* Project: ESP32 Utilities SPIFFS
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library was to offer a high level of abstraction for interacting with the SPIFFS in the ESP32
// *
// * Great sources to learn more about SPIFFS & how it interacts with the ESP:
// *
// * https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS
// *
// *

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
//None.

//*****************************************************        LIBRARIES        *****************************************************/
#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"
#include <utils.h>
#include <ArduinoJson.h>

//*****************************************************       DATA TYPES        *****************************************************/

//*****************************************************       EMMC CLASS        *****************************************************/
class SPIFFS_Memory
{
public:
    // -- Initialize SPIFFS
    ESP_ERROR begin();

    //File Operations
    ESP_ERROR readFile(const char *path, String &file_content);

    ESP_ERROR readJSON(const char *path, JsonDocument &json_document)
    {
        ESP_ERROR err;
        err.on_error = false;

        String temp_message;

        if (spiffs_initialized)
        {
            File file = file_system->open(path);

            // Error opening file
            if (!file)
            {
                err.on_error = true;
                temp_message += "Failed to open file \"";
                temp_message += path;
                temp_message += "\" for writting";
            }

            // If filed opened correctly
            else
            {
                DeserializationError error = deserializeJson(json_document, file);

                if (error)
                {
                    err.on_error = true;
                    temp_message += "Failed to read file, using default configuration";
                }
            }
        }
        else
        {
            err.on_error = true;
            temp_message += "SPIFFS is not inititalized";
        }

        err.debug_message = temp_message;
        return err;
    }
    ESP_ERROR writeFile(const char *path, const char *message);
    ESP_ERROR appendFile(const char *path, const char *message);
    ESP_ERROR renameFile(const char *path1, const char *path2);
    ESP_ERROR deleteFile(const char *path);

    // -- Card operations
    uint64_t getSPIFFSsize();
    uint64_t getTotalMemorySpace();
    uint64_t getUsedMemorySpace();

    // -- State Operations
    bool isInitialized()
    {
        return spiffs_initialized;
    }

private:
    // -- Memory Info
    long spiffs_memory_size;
    long spiffs_total_memory_space;
    long spiffs_used_memory_space;

    // -- Debugging
    bool spiffs_initialized;

    // -- File system
    fs::FS *file_system;
};

// End.