#include "led_strip.h"

//TODO: How to start custom blinking sessions after
void breath();
void refreshLED(void *parameters);

void setupLED_Strip(void *parameters)
{
    //* 1. Enabled LED's by driving LED GND pin
    pinMode(LED_GND_ENABLE, OUTPUT);
    digitalWrite(LED_GND_ENABLE, HIGH);
    vTaskDelay(10 / portTICK_PERIOD_MS); // Allow some time to turn on

    //* 2. Begin & clear pixels
    led_strip.begin();
    led_strip.clear();
    led_strip.show();

    xTaskCreatePinnedToCore(refreshLED, "Refresh LED", 1000, NULL, 25, NULL, xPortGetCoreID());

    vTaskDelete(NULL);
}

uint8_t brightness;
uint8_t minimum_brightness = 40;
uint8_t maximum_brightness = 120;

void refreshLED(void *parameters)
{

    bool breath_in = true;

    while (1)
    {
        for (int i = 0; i < NUMPIXELS; i++)
        {
            led_strip.setPixelColor(i, led_strip.Color(brightness, brightness, brightness));
            led_strip.show();
        }

        if (breath_in)
        {
            brightness = brightness + 1;

            if (brightness == maximum_brightness)
                breath_in = false;
        }

        if (!breath_in)
        {
            brightness = brightness - 1;

            if (brightness == minimum_brightness)
                breath_in = true;
        }

        vTaskDelay(LED_BLINKING_PERIOD_mS / portTICK_PERIOD_MS);
    }
}
