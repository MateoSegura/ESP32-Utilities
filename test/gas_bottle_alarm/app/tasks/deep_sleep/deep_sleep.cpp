#include "deep_sleep.h"
#include "../terminal/terminal.h"

void setupDeepSleepManager(void *parameters)
{
    long initial_time = micros();
    TerminalMessage sleep_debug_message;
    const uint8_t debug_message_queue_ticks = 0;

    // * Get wake up source
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
                                          "SLE", ERROR, micros(),   // System, message type, timestamp
                                          micros() - initial_time); // Process Time

    addDebugMessageToQueue(&sleep_debug_message, debug_message_queue_ticks);

    vTaskDelete(NULL);
}