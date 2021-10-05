#include "settings.h"
#include "../terminal/terminal.h"

struct ESP32_FILE
{
    const char *name;
    String content;
};

bool initializeSPIFFS()
{
    long initial_time = micros();
    TerminalMessage spiffs_debug_message;
    const uint8_t debug_message_queue_ticks = 0;

    // * Begin SPIFFS memory
    ESP_ERROR initialize_spiffs = spiffsMemory.begin(); // Initialize SPIFFS

    if (initialize_spiffs.on_error) // Catch error
    {
        spiffs_debug_message = TerminalMessage(initialize_spiffs.debug_message, // Message
                                               "FFS", ERROR, micros(),          // System, message type, timestamp
                                               micros() - initial_time);        // Process Time

        addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
        return false;
    }

    // *  Correct initialization
    spiffs_debug_message = TerminalMessage("SPIFFS initialized",     // Message
                                           "FFS", INFO, micros(),    // System, message type, timestamp
                                           micros() - initial_time); // Process Time

    addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);

    return true;
}

void printSettingsFile()
{
    TerminalMessage spiffs_debug_message;
    const uint8_t debug_message_queue_ticks = 1;

    ESP32_FILE settings_file;     //Declare local file
    settings_file.name = "hello"; // Give name

    ESP_ERROR read_settings_file = spiffsMemory.readFile(settings_file.name, settings_file.content);

    if (read_settings_file.on_error) // Catch error
    {
        spiffs_debug_message = TerminalMessage(read_settings_file.debug_message, // Message
                                               "FFS", ERROR, micros());          // System, message type, timestamp

        addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
    }

    //addFileToPrintQueue(settings_file.content); // Add File to terminal
}

bool readSettingsFile()
{
    long initial_time = micros();
    TerminalMessage spiffs_debug_message;
    const uint8_t debug_message_queue_ticks = 1;

    // * Parse JSON
    // StaticJsonDocument<SETTINGS_FILE_SIZE> settings_json;
    // ESP_ERROR read_json = Memory.spiffs.readJSON(SETTINGS_FILE_NAME, settings_json);

    // if (read_json.on_error)
    // {
    //     spiffs_debug_message = TerminalMessage(read_json.debug_message,  // Message
    //                                            "FFS", ERR, micros(),     // System, message type, timestamp
    //                                            micros() - initial_time); // Process Time

    //     addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
    //     return false;
    // }

    // // * Read settings from settings file
    // BottleBirdSettings.first_boot = settings_json["first_boot"];
    // BottleBirdSettings.device_setup = settings_json["device_setup"];
    // BottleBirdSettings.wifi_ssid = settings_json["wifi_ssid"];
    // BottleBirdSettings.wifi_password = settings_json["wifi_password"];

    return true;
}

void setupSPIFFS(void *parameters)
{
    long initial_time = micros();
    TerminalMessage spiffs_debug_message;
    const uint8_t debug_message_queue_ticks = 1;

    // -- Initialize Memory
    if (initializeSPIFFS() == true)
    {
        // -- Read Settings File
        if (readSettingsFile() == true)
        {
            printSettingsFile();

            if (BottleBirdSettings.first_boot)
            {
                spiffs_debug_message = TerminalMessage("First boot detected",    // Message
                                                       "FFS", ERROR, micros(),   // System, message type, timestamp
                                                       micros() - initial_time); // Process Time

                addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
            }
            else
            {
                spiffs_debug_message = TerminalMessage("Device is not setup",   // Message
                                                       "FFS", ERROR, micros()); // System, message type, timestamp

                addDebugMessageToQueue(&spiffs_debug_message, debug_message_queue_ticks);
            }
        }
    }

    //vTaskDelay(2000 / portTICK_PERIOD_MS);
    //xSemaphoreGive(start_deep_sleep_semaphore); // Start deep sleep

    vTaskDelete(NULL);
}
