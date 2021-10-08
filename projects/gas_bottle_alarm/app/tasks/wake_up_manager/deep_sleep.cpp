#include "deep_sleep.h"
#include "../terminal/terminal.h"

ESP_ERROR setupHardwareForDeepSleep()
{
    ESP_ERROR err;

    return err;
}
void setupDeepSleepManager(void *parameters)
{
    long initial_time = micros();
    TerminalMessage sleep_debug_message;
    const uint8_t debug_message_queue_ticks = 0;

    //* 1.Get wake up source
    String wakeup_reason_string;
    esp_sleep_wakeup_cause_t wakeup_reason;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason)
    {
    case ESP_SLEEP_WAKEUP_EXT0:
        wakeup_reason_string = "Wakeup caused by external signal using RTC_IO";
        break;
    case ESP_SLEEP_WAKEUP_EXT1:
        wakeup_reason_string = "Wakeup caused by external signal using RTC_CNTL";
        break;
    case ESP_SLEEP_WAKEUP_TIMER:
        wakeup_reason_string = "Wakeup caused by timer";
        break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
        wakeup_reason_string = "Wakeup caused by touchpad";
        break;
    case ESP_SLEEP_WAKEUP_ULP:
        wakeup_reason_string = "Wakeup caused by ULP program";
        break;
    default:
        wakeup_reason_string = "Wakeup was not caused by deep sleep";
        break;
    }

    sleep_debug_message = TerminalMessage(wakeup_reason_string,     // Message
                                          "SLE", INFO, micros(),    // System, message type, timestamp
                                          micros() - initial_time); // Process Time

    //* 2. Measure if battery voltage is above threshold
    uint16_t raw_battery_adc_value = analogRead(VBAT_3V3_PIN);
    double battery_voltage = 2 * ((raw_battery_adc_value * 3.3) / 4095); //1/2 voltage divier

    TerminalMessage battery_voltage_message = TerminalMessage(
        "Battery voltage -> " + String(battery_voltage) + " V. Threshold is -> " + String((double)MINIMUM_BAT_VOLTAGE_FOR_BOOT_mV / 1000),
        "SLE", INFO, micros());

    //* 3. Go back to sleep if voltage too low. Device will only wake up again from user button
    if ((battery_voltage * 1000) <= MINIMUM_BAT_VOLTAGE_FOR_BOOT_mV)
    {
        terminal.printMessage(battery_voltage_message);

        battery_voltage_message = TerminalMessage("Battery voltage is too low, starting deep sleep.",
                                                  "SLE", INFO, micros());

        terminal.printMessage(battery_voltage_message);
        terminal.end();

        esp.uart0.println("\n\n");
        esp.uart0.print("Power off");

        esp_deep_sleep_start();
    }

    //* If battery voltage above threshold
    addDebugMessageToQueue(&battery_voltage_message);
    addDebugMessageToQueue(&sleep_debug_message, debug_message_queue_ticks);

    vTaskDelete(NULL);
}