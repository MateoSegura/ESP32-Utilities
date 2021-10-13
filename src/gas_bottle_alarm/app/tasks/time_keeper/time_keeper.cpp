#include "time_keeper.h"
#include "../terminal/terminal.h"

void setupTimeKeeper(void *parameters)
{
    long initial_time = micros();
    TerminalMessage rtc_init_debug_message;

    //* 1. Init I2C bus & rtc chip
    esp.i2c0.begin(I2C0_SDA_PIN, I2C0_SCL_PIN, I2C0_FREQUENCY);
    ESP_ERROR initialize_rtc = rtc.begin(RealTimeClock::RV8803_IC, &esp.i2c0);

    if (initialize_rtc.on_error)
    {
        rtc_init_debug_message = TerminalMessage(initialize_rtc.debug_message,
                                                 "RTC", ERROR, micros(), micros() - initial_time);

        while (1)
        {
            addDebugMessageToQueue(&rtc_init_debug_message);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    else
    {
        rtc_init_debug_message = TerminalMessage("RTC initialized correctly",
                                                 "RTC", INFO, micros(), micros() - initial_time);
    }

    addDebugMessageToQueue(&rtc_init_debug_message);

    vTaskDelete(NULL);
}