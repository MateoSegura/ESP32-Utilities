/* 
* Company: ANZE Suspension
* File Name: ESP32UtilitiesSPIFFS.cpp
* Project: ESP32 Utilities SPIFFS
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*****************************************************       HEADER FILE       *****************************************************/
#include "spiffs_memory.h"

//*********************************************      SPIFFS FUNCTIONS DEFINTIONS       **********************************************/
// -- Initialize SPIFFS
ESP_ERROR SPIFFS_Memory::begin()
{
    ESP_ERROR err;
    err.on_error = false;

    if (SPIFFS.begin() != true)
    {
        err.on_error = true;
        err.debug_message = "Error initializing SPIFFS";
    }
    else
    {
        spiffs_initialized = true;
        file_system = &SPIFFS;
    }

    return err;
}

// -- File Operations
ESP_ERROR SPIFFS_Memory::readFile(const char *path, String &file_content)
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
            temp_message += "\" for reading";
        }

        // If filed opened correctly
        else
        {
            //TODO: Implement serial burst read
            // err.on_error = true;
            // err.debug_message = "Serial file download has not been implemented yet";
            // Serial.begin(115200);
            while (file.available())
            {
                file_content += (char)file.read();
            }

            file.close();
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

ESP_ERROR SPIFFS_Memory::writeFile(const char *path, const char *message)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (spiffs_initialized)
    {
        File file = file_system->open(path, FILE_WRITE);

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
            if (file.println(message))
            {
                temp_message += "Appending to file \"";
                temp_message += path;
                temp_message += "\" was succesful";
                file.close();
            }
            else
            {
                err.on_error = true;
                temp_message += "File write operation failed";
                file.close();
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

ESP_ERROR SPIFFS_Memory::appendFile(const char *path, const char *message)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (spiffs_initialized)
    {
        File file = this->file_system->open(path, FILE_APPEND);

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
            if (file.println(message))
            {
                temp_message += "Appending to file \"";
                temp_message += path;
                temp_message += "\" was succesful";
                file.close();
            }
            else
            {
                err.on_error = true;
                temp_message += "File write operation failed";
                file.close();
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

ESP_ERROR SPIFFS_Memory::renameFile(const char *path1, const char *path2)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;
    if (spiffs_initialized)
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
        temp_message += "SPIFFS is not inititalized";
    }
    err.debug_message = temp_message;
    return err;
}

ESP_ERROR SPIFFS_Memory::deleteFile(const char *path)
{
    ESP_ERROR err;
    err.on_error = false;

    String temp_message;

    if (spiffs_initialized)
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
        temp_message += "SPIFFS is not inititalized";
    }

    err.debug_message = temp_message;
    return err;
}

ESP_ERROR SPIFFS_Memory::readJSON(const char *path, JsonDocument &json_document)
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
            temp_message += "\" for reading";
        }

        // If filed opened correctly
        else
        {
            DeserializationError error = deserializeJson(json_document, file);

            if (error)
            {
                err.on_error = true;
                temp_message += "DeserializationError error code: " + String(error.code());
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

ESP_ERROR SPIFFS_Memory::writeJSON(const char *path, JsonDocument &json_document)
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

        // If filed opened correctly then delete it, and rewrite the contents
        else
        {
            String json_string;
            serializeJson(json_document, json_string);

            ESP_ERROR delete_json = deleteFile(path);

            if (delete_json.on_error)
            {
                err.on_error = true;
                err.debug_message = delete_json.debug_message;
            }
            else
            {
                ESP_ERROR write_json = writeFile(path, json_string.c_str());

                if (write_json.on_error)
                {
                    err.on_error = true;
                    err.debug_message = write_json.debug_message;
                }
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
// End.