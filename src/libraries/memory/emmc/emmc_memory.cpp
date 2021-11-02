/*
 * Company: ANZE Suspension
 * File Name: ESP32UtilitiesEMMC.cpp
 * Project: ESP32 Utilities EMMC
 * Version: 1.0
 * Compartible Hardware:
 * Date Created: September 8, 2021
 * Last Modified: September 9, 2021
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

//*****************************************************       HEADER FILE       *****************************************************/
#include "emmc_memory.h"

//*********************************************       EMMC FUNCTIONS DEFINTIONS       ***********************************************/
// -- Initialize SD Card
ESP_ERROR EMMC_Memory::begin(uint8_t enable_pin,
                             uint8_t detect_pin,
                             eMMC_CONNECTION_MODE mode,
                             eMMC_BUS_WIDTH bus_width)
{
    ESP_ERROR err;
    err.on_error = false;

    // Only initialize if card is not initialized
    if (!emmc_initialized)
    {
        emmc_detect_pin = detect_pin;
        emmc_enable_pin = enable_pin;

        // Detect if card is inserted
        if (emmc_detect_pin != -1)
        {
            pinMode(emmc_detect_pin, INPUT_PULLUP);
        }

        vTaskDelay(25 / portTICK_PERIOD_MS); // This is required of pin will read HIGH. Don't know why, not too important

        if (digitalRead(emmc_detect_pin) == LOW || emmc_detect_pin == -1)
        {
            emmc_detected = true;
            emmc_bus_width = bus_width;
            connection_mode = mode;

            // Turn SD card on. Give it sometime to power up
            if (emmc_enable_pin != -1)
            {
                pinMode(emmc_enable_pin, OUTPUT);
                digitalWrite(emmc_enable_pin, LOW);
                vTaskDelay(SD_POWER_UP_DELAY_mS / portTICK_PERIOD_MS);
            }

            // Set bus width if in eMMC mode
            switch (connection_mode)
            {
            case eMMC_MODE:
            { // Set up pull up resistors
                pinMode(esp_emmc_data0, INPUT_PULLUP);
                pinMode(esp32_emmc_clk, INPUT_PULLUP);
                pinMode(esp32_emmc_cmd, INPUT_PULLUP);

                bool mode_1_bit = true;

                if (emmc_bus_width == MODE_4_BIT)
                {
                    mode_1_bit = false;
                    pinMode(esp_emmc_dat1, INPUT_PULLUP);
                    pinMode(esp_emmc_dat2, INPUT_PULLUP);
                    pinMode(esp_emmc_dat3, INPUT_PULLUP);
                }

                // Attempt to initialize
                if (!SD_MMC.begin("/sdcard", mode_1_bit))
                {
                    err.on_error = true;
                    err.debug_message = "Card mount failed. Check connections";
                }
                else
                {
                    emmc_initialized = true;
                    file_system = &SD_MMC;

                    emmc_memory_size = SD_MMC.cardSize() / (1024 * 1024);
                    emmc_total_memory_space = SD_MMC.totalBytes() / (1024 * 1024);
                    emmc_used_memory_space = SD_MMC.usedBytes() / (1024 * 1024);
                }
            }

            break;
            case SPI_MODE:
            { // TODO: Implement SPI eMMC mode'
                err.on_error = true;
                err.debug_message = "SPI Mode hasn't been implemented yet";
            }

            break;

            default:
                break;
            }
        }

        // If there's no card inserted
        else
        {
            emmc_detected = false;
            err.on_error = true;
            err.debug_message = "No uSD Card was detected";
        }
    }
    else
    {
        err.on_error = true;
        err.debug_message = "eMMC has already been inititalized";
    }

    return err;
}

// -- Directory Operations
ESP_ERROR EMMC_Memory::listDirectory(const char *dirname, uint8_t levels)
{
    // TODO: Implement directory list functionality
    ESP_ERROR err;
    err.on_error = true;
    err.debug_message = "List directory functionality has not been implemented yet";
    return err;
    // if (emmc_initialized)
    // {
    //     String temp_message;
    //     temp_message += "Listing directory \"";
    //     temp_message += dirname;
    //     temp_message += "\"";
    //     printDebugMessage(temp_message, READ_WRITE);
    //     temp_message = "";

    //     File root = this->_file_system->open(dirname);
    //     if (!root)
    //     {

    //         printDebugMessage("Failed to open directory", ERR);
    //         return;
    //     }
    //     if (!root.isDirectory())
    //     {
    //         printDebugMessage("Not a Directory", ERR);
    //         return;
    //     }

    //     File file = root.openNextFile();
    //     while (file)
    //     {
    //         if (file.isDirectory())
    //         {
    //             temp_message += "\tDirectory: ";
    //             temp_message += file.name();
    //             printDebugMessage(temp_message, READ_WRITE);
    //             temp_message = "";

    //             if (levels)
    //             {
    //                 listDirectory(file.name(), levels - 1);
    //             }
    //         }
    //         else
    //         {
    //             temp_message += "\tFile: ";
    //             temp_message += file.name();
    //             temp_message += " - Size: ";
    //             temp_message += file.size();
    //             temp_message += " bytes";
    //             printDebugMessage(temp_message, READ_WRITE);
    //             temp_message = "";
    //         }
    //         file = root.openNextFile();
    //     }
    // }
    // else
    // {
    //     printDebugMessage("Card has not been initialized", ERR);
    // }
}

ESP_ERROR EMMC_Memory::makeDirectory(const char *path)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (emmc_initialized)
    {
        if (file_system->mkdir(path))
        {
            temp_message += "Directory \"";
            temp_message += path;
            temp_message += "\"";
            temp_message += " created succesfully";
        }
        else
        {
            err.on_error = true;
            temp_message += "Error creating directory";
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }

    err.debug_message = temp_message;
    return err;
}

ESP_ERROR EMMC_Memory::removeDirectory(const char *path)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (emmc_initialized)
    {
        if (file_system->rmdir(path))
        {
            temp_message += "Directory \"";
            temp_message += path;
            temp_message += "\"";
            temp_message += " deleted succesfully";
        }
        else
        {
            err.on_error = true;
            temp_message += "Error deleting directory";
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }
    err.debug_message = temp_message;
    return err;
}

// -- File Operations
ESP_ERROR EMMC_Memory::readFile(const char *path)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (emmc_initialized)
    {
        File file = file_system->open(path);

        // Error opening file
        if (!file)
        {
            err.on_error = true;
            temp_message += "Failed to open file \"";
            temp_message += path;
            temp_message += "\" for reading";
        }

        // If filed opened correctly
        else
        {
            // TODO: Implement serial burst read
            err.on_error = true;
            err.debug_message = "Serial file download has not been implemented yet";
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }

    err.debug_message = temp_message;
    return err;
}

ESP_ERROR EMMC_Memory::writeFile(const char *path, const uint8_t *message)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (emmc_initialized)
    {
        myFile = file_system->open(path, FILE_WRITE);

        // Error opening file
        if (!myFile)
        {
            err.on_error = true;
            temp_message += "Failed to open file \"";
            temp_message += path;
            temp_message += "\" for writting";
        }

        // If filed opened correctly
        else
        {
            if (myFile.write(message, 512))
            {
                temp_message += "Appending to file \"";
                temp_message += path;
                temp_message += "\" was succesful";
                // file.close();
            }
            else
            {
                err.on_error = true;
                temp_message += "File write operation failed";
                myFile.close();
            }
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }
    err.debug_message = temp_message;
    return err;
}

ESP_ERROR EMMC_Memory::appendFile(const char *path, const uint8_t *message)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (emmc_initialized)
    {
        // File file = this->file_system->open(path, FILE_APPEND);

        // Error opening file
        if (!myFile)
        {
            err.on_error = true;
            temp_message += "Failed to open file \"";
            temp_message += path;
            temp_message += "\" for writting";
        }

        // If filed opened correctly
        else
        {
            if (myFile.write(message, 512))
            {
                temp_message += "Appending to file \"";
                temp_message += path;
                temp_message += "\" was succesful";
                myFile.flush();
            }
            else
            {
                err.on_error = true;
                temp_message += "File write operation failed";
                myFile.close();
            }
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }

    err.debug_message = temp_message;
    return err;
}

ESP_ERROR EMMC_Memory::renameFile(const char *path1, const char *path2)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;
    if (emmc_initialized)
    {

        if (file_system->rename(path1, path2))
        {
            temp_message += "Renaming File \"";
            temp_message += path1;
            temp_message += "\" to \"";
            temp_message += path2;
            temp_message += "\" was sucessful";
        }
        else
        {
            err.on_error = true;
            temp_message += "File rename failed";
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }
    err.debug_message = temp_message;
    return err;
}

ESP_ERROR EMMC_Memory::deleteFile(const char *path)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (emmc_initialized)
    {

        if (file_system->remove(path))
        {
            temp_message += "File \"";
            temp_message += path;
            temp_message += "\" has been deleted";
        }
        else
        {
            err.on_error = true;
            temp_message += "Could not delete file \"";
            temp_message += path;
            temp_message += "\"";
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }

    err.debug_message = temp_message;
    return err;
}

ESP_ERROR EMMC_Memory::onDetectPinChange()
{
    ESP_ERROR err;
    err.on_error = false;

    if (digitalRead(emmc_detect_pin) == LOW)
    {
        if (emmc_initialized == false)
        {
            begin(emmc_enable_pin, emmc_detect_pin, connection_mode, emmc_bus_width);
        }
        else
        {
            err.on_error = true;
            err.debug_message = "Invalid pin change init";
        }
    }

    else if (digitalRead(emmc_detect_pin == HIGH))
    {
        if (emmc_initialized == true)
        {
            digitalWrite(emmc_enable_pin, LOW);
            emmc_initialized = false;
            emmc_detected = false;
        }
        else
        {
            err.on_error = true;
            err.debug_message = "Invalid pin change end";
        }
    }

    return err;
}
// End.