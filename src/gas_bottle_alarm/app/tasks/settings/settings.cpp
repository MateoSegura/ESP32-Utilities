#include "settings.h"
#include "../terminal/terminal.h"
#include <driver/dac.h>

float adc_calibrated_results[4097];

struct ESP32_FILE
{
    const char *name;
    String content;
};

ESP_ERROR initializeSPIFFS();
ESP_ERROR readSettingsFile();
ESP_ERROR saveSettings();
ESP_ERROR calibrateADC();

void setupSPIFFS(void *parameters)
{
    long initial_time = micros();
    const uint8_t debug_message_queue_ticks = 1;

    // * 1. Initialize Memory
    ESP_ERROR initialize_spiffs = initializeSPIFFS();

    TerminalMessage init_spiffs_debug_message;

    if (initialize_spiffs.on_error)
    {
        init_spiffs_debug_message = TerminalMessage("SPIFFS was not initialized", // Message
                                                    "FFS", ERROR, micros(),       // System, message type, timestamp
                                                    micros() - initial_time);     // Process Time

        addDebugMessageToQueue(&init_spiffs_debug_message, debug_message_queue_ticks);
        vTaskDelete(NULL);
    }
    else
    {
        init_spiffs_debug_message = TerminalMessage("SPIFFS initialized",     // Message
                                                    "FFS", ERROR, micros(),   // System, message type, timestamp
                                                    micros() - initial_time); // Process Time

        addDebugMessageToQueue(&init_spiffs_debug_message, debug_message_queue_ticks);
    }

    // * 2. Read settings file
    ESP_ERROR read_settings = readSettingsFile();

    TerminalMessage read_file_debug_message;

    if (read_settings.on_error)
    {
        read_file_debug_message = TerminalMessage(read_settings.debug_message, // Message
                                                  "FFS", ERROR, micros(),      // System, message type, timestamp
                                                  micros() - initial_time);    // Process Time
        addDebugMessageToQueue(&read_file_debug_message, debug_message_queue_ticks);
        vTaskDelete(NULL);
    }
    else
    {
        read_file_debug_message = TerminalMessage("Settings file read correctly", // Message
                                                  "FFS", ERROR, micros(),         // System, message type, timestamp
                                                  micros() - initial_time);       // Process Time
        addDebugMessageToQueue(&read_file_debug_message, debug_message_queue_ticks);
    }

    // * 3. Figure out if it's the first boot
    TerminalMessage on_first_boot_debug_message;

    if (app.device_settings.first_boot)
    {
        app.device_settings.first_boot = false;

        ESP_ERROR write_file_on_first_boot = saveSettings();

        if (write_file_on_first_boot.on_error)
        {
            on_first_boot_debug_message = TerminalMessage(write_file_on_first_boot.debug_message, // Message
                                                          "FFS", ERROR, micros(),                 // System, message type, timestamp
                                                          micros() - initial_time);               // Process Time
        }
        else
        {
            on_first_boot_debug_message = TerminalMessage("First boot. Device was setup & settings file was re-written", // Message
                                                          "FFS", ERROR, micros(),                                        // System, message type, timestamp
                                                          micros() - initial_time);                                      // Process Time
        }

        addDebugMessageToQueue(&on_first_boot_debug_message, debug_message_queue_ticks);

        //TODO: 3.2. Calibrate ADC & store in RTC memory
    }

    // * 4. Figure out if device has been paired yet
    if (app.device_settings.device_is_setup)
    {
        xSemaphoreGive(app.rtos.start_main); // * Start Main App if paired
    }
    else
    {
        xSemaphoreGive(app.rtos.start_pairing); // * Start Pairing
    }

    vTaskDelete(NULL);
}

ESP_ERROR initializeSPIFFS()
{
    ESP_ERROR err;

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
    ESP_ERROR settings_correct = app.device_settings.fromJSON(settings_json_file);

    if (settings_correct.on_error)
    {
        err.on_error = true;
        err.debug_message = settings_correct.debug_message;
        return err;
    }

    return err;
}

ESP_ERROR saveSettings()
{
    ESP_ERROR err;

    StaticJsonDocument<SETTINGS_FILE_SIZE_BYTES> new_settings_json;

    ESP_ERROR get_settings_string = app.device_settings.toJSON(new_settings_json);

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

ESP_ERROR calibrateADC()
{
    ESP_ERROR err;

    float Res2[4096 * 5];

    dac_output_enable(DAC_CHANNEL_1); // gpio 25
    dac_output_voltage(DAC_CHANNEL_1, 0);
    analogReadResolution(12);

    // * Test Linearity
    for (int j = 0; j < 500; j++)
    {
        if (j % 100 == 0)
            for (int i = 0; i < 256; i++)
            {
                dac_output_voltage(DAC_CHANNEL_1, (i & 0xff));
                delayMicroseconds(10);
                adc_calibrated_results[i * 16] = 0.9 * adc_calibrated_results[i * 16] + 0.1 * analogRead(VBAT_3V3_PIN);
            }
    }

    // * Calculate Interpolated values
    adc_calibrated_results[4096] = 4095.0;

    for (int i = 0; i < 256; i++)
    {
        for (int j = 1; j < 16; j++)
        {
            adc_calibrated_results[i * 16 + j] = adc_calibrated_results[i * 16] + (adc_calibrated_results[(i + 1) * 16] - adc_calibrated_results[(i)*16]) * (float)j / (float)16.0;
        }
    }

    // * Generate LUT
    for (int i = 0; i < 4096; i++)
    {
        adc_calibrated_results[i] = 0.5 + adc_calibrated_results[i];
    }

    adc_calibrated_results[4096] = 4095.5000;

    for (int i = 0; i < 4096; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            Res2[i * 5 + j] = adc_calibrated_results[i] + (adc_calibrated_results[(i + 1)] - adc_calibrated_results[i]) * (float)j / (float)10.0;
        }
    }
    for (int i = 1; i < 4096; i++)
    {
        int index;
        float minDiff = 99999.0;
        for (int j = 0; j < (5 * 4096); j++)
        {
            float diff = fabs((float)(i)-Res2[j]);
            if (diff < minDiff)
            {
                minDiff = diff;
                index = j;
            }
        }
        adc_calibrated_results[i] = (float)index;
    }

    for (int i = 0; i < (4096); i++)
    {
        adc_calibrated_results[i] /= 5;
    }

    return err;
}