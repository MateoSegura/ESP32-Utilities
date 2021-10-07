#include "settings.h"
#include "../terminal/terminal.h"

struct ESP32_FILE
{
    const char *name;
    String content;
};

ESP_ERROR initializeSPIFFS();
ESP_ERROR readSettingsFile();
ESP_ERROR SaveSettingsOnFirstBoot();

void setupSPIFFS(void *parameters)
{
    long initial_time = micros();
    TerminalMessage spiffs_debug_message;
    const uint8_t debug_message_queue_ticks = 1;

    // * 1. Initialize Memory
    ESP_ERROR initialize_spiffs = initializeSPIFFS();

    if (initialize_spiffs.on_error)
    {
        spiffs_debug_message = TerminalMessage("SPIFFS was not initialized", // Message
                                               "FFS", ERROR, micros(),       // System, message type, timestamp
                                               micros() - initial_time);     // Process Time

        addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
        vTaskDelete(NULL);
    }

    // * 2. Read settings file
    ESP_ERROR read_settings = readSettingsFile();
    if (read_settings.on_error)
    {
        spiffs_debug_message = TerminalMessage(read_settings.debug_message, // Message
                                               "FFS", ERROR, micros(),      // System, message type, timestamp
                                               micros() - initial_time);    // Process Time
        addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
        vTaskDelete(NULL);
    }

    // * 3. Figure out if it's the first boot
    if (BottleBirdSettings.first_boot)
    {
        ESP_ERROR write_file_on_first_boot = SaveSettingsOnFirstBoot();

        if (write_file_on_first_boot.on_error)
        {
            spiffs_debug_message = TerminalMessage(write_file_on_first_boot.debug_message, // Message
                                                   "FFS", ERROR, micros(),                 // System, message type, timestamp
                                                   micros() - initial_time);               // Process Time
        }
        else
        {
            spiffs_debug_message = TerminalMessage("First boot. Device was setup & settings file was re-written", // Message
                                                   "FFS", ERROR, micros(),                                        // System, message type, timestamp
                                                   micros() - initial_time);                                      // Process Time
        }

        addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        xSemaphoreGive(app.start_main);
        vTaskDelete(NULL);
    }

    // * 4. Figure out if device has been paired yet
    if (BottleBirdSettings.device_is_setup)
    {
        spiffs_debug_message = TerminalMessage("Device is setup. Starting main", // Message
                                               "FFS", INFO, micros(),            // System, message type, timestamp
                                               micros() - initial_time);         // Process Time

        addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        xSemaphoreGive(app.start_main); // * Start Main App if paired
        vTaskDelete(NULL);
    }
    else
    {
        spiffs_debug_message = TerminalMessage("Device is not setup. Starting initial pairing: ", // Message
                                               "FFS", INFO, micros(),                             // System, message type, timestamp
                                               micros() - initial_time);                          // Process Time

        addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
        vTaskDelay(10 / portTICK_PERIOD_MS);
        xSemaphoreGive(app.start_pairing); // * Start Pairing
        vTaskDelete(NULL);
    }

    // In theory this should never happen, and you should know why!. =)

    vTaskDelete(NULL);
}

ESP_ERROR initializeSPIFFS()
{
    ESP_ERROR err;
    err.on_error = false;

    // * Begin SPIFFS memory
    ESP_ERROR initialize_spiffs = spiffsMemory.begin(); // Initialize SPIFFS

    if (initialize_spiffs.on_error) // Catch error
    {
        err.on_error = true;
        err.debug_message = initialize_spiffs.on_error;
    }

    return err;
}

ESP_ERROR readSettingsFile()
{
    ESP_ERROR err;
    err.on_error = false;

    // * Parse JSON
    DynamicJsonDocument settings_json_file(SETTINGS_FILE_SIZE_BYTES);
    ESP_ERROR read_settings = spiffsMemory.readJSON("/settings.json", settings_json_file);

    if (read_settings.on_error)
    {
        err.on_error = true;
        err.debug_message = read_settings.debug_message;
        return err;
    }

    // * Read settings from settings file
    ESP_ERROR settings_correct = BottleBirdSettings.fromJSON(settings_json_file);

    if (settings_correct.on_error)
    {
        err.on_error = true;
        err.debug_message = settings_correct.debug_message;
        return err;
    }

    return err;
}

ESP_ERROR SaveSettingsOnFirstBoot()
{
    ESP_ERROR err;
    err.on_error = false;

    BottleBirdSettings.first_boot = false;

    StaticJsonDocument<SETTINGS_FILE_SIZE_BYTES> new_settings_json;

    ESP_ERROR get_settings_string = BottleBirdSettings.toJSON(new_settings_json);

    if (get_settings_string.on_error)
    {
        err.on_error = true;
        err.debug_message = get_settings_string.debug_message;
        return err;
    }

    ESP_ERROR write_settings = spiffsMemory.writeJSON(SETTINGS_FILE, new_settings_json);

    if (write_settings.on_error)
    {
        err.on_error = true;
        err.debug_message = get_settings_string.debug_message;
        return err;
    }

    return err;
}