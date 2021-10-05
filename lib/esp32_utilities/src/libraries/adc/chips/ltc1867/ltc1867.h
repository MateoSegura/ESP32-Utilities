#pragma once
/* 
* Company: ANZE Suspension
* File Name: rtc.h
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer some abstraction for RTC ICs.

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
// * None.

//*****************************************************        LIBRARIES        *****************************************************/
#include <utils.h>
#include <Arduino.h>
#include <SPI.h> // SPI

//**************************************************       DEVICE REGISTERS       ************************ **************************/

// - Analog Input multiplexer

//***************************************************       LTC1867 CLASS         ***************************************************/
class LTC1867
{
public:
    ESP_ERROR begin(uint8_t cs_pin, SPIClass esp_spi_bus, uint32_t esp_spi_bus_sck_frequency)
    {
        ESP_ERROR err;
        err.on_error = false;

        // -- This library assumes that SPI port is set externally
        spi_bus = esp_spi_bus;
        spi_settings = SPISettings(esp_spi_bus_sck_frequency, MSBFIRST, SPI_MODE0);
        adc_cs_pin = cs_pin;

        // -- Pulling CS high begins initial conversion
        pinMode(cs_pin, OUTPUT);
        digitalWrite(cs_pin, HIGH);

        vTaskDelay(1 / portTICK_PERIOD_MS);

        // -- Read dummy variable to clear up possible errors
        digitalWrite(adc_cs_pin, LOW);

        spi_bus.beginTransaction(spi_settings);
        spi_bus.transfer16(getChannelByteWord(0) << 8 | 0x00); // Start channel 8 conversion
        spi_bus.endTransaction();

        digitalWrite(adc_cs_pin, HIGH);

        current_channel_selected = 0;

        return err;
    }

    ESP_ERROR readAndSwitchChannel(uint8_t read_channel, uint8_t new_channel, double &result_reading)
    {
        ESP_ERROR err;
        err.on_error = false;

        // -- Check that the input channel is correct
        if (read_channel > 7 || new_channel > 7)
        {
            err.on_error = true;
            err.debug_message = "Invalid channel number";

            return err;
        }

        // -- Change channel if not the one selected
        if (read_channel != current_channel_selected)
        {
            switchChannel(read_channel);
        }

        // -- Read Value
        delayMicroseconds(5);
        digitalWrite(adc_cs_pin, LOW);

        spi_bus.beginTransaction(spi_settings);
        uint16_t adc_reading = spi_bus.transfer16(getChannelByteWord(new_channel) << 8 | 0x00);
        spi_bus.endTransaction();

        digitalWrite(adc_cs_pin, HIGH);

        read_channel = new_channel;

        //result_reading = getMilliVoltsFromReading(adc_reading);

        result_reading = adc_reading;

        return err;
    }

    double getMilliVoltsFromReading(uint16_t analog_reading)
    {
        return analog_reading * 0.0001875;
    }

    ESP_ERROR switchChannel(uint8_t analog_channel)
    {
        ESP_ERROR err;
        err.on_error = false;

        // -- Check that the input channel is correct
        if (analog_channel > 7)
        {
            err.on_error = true;
            err.debug_message = "Invalid channel number";

            return err;
        }

        // -- Read dummy variable to clear up possible errors
        digitalWrite(adc_cs_pin, LOW);

        spi_bus.beginTransaction(spi_settings);
        spi_bus.transfer16(getChannelByteWord(analog_channel) << 8 | 0x00);
        spi_bus.endTransaction();

        digitalWrite(adc_cs_pin, HIGH);

        current_channel_selected = analog_channel;

        return err;
    }

    byte getChannelByteWord(uint8_t analog_channel)
    {
        switch (analog_channel)
        {
        case 0:
            return select_channel_0;
            break;
        case 1:
            return select_channel_1;
            break;
        case 2:
            return select_channel_2;
            break;
        case 3:
            return select_channel_3;
            break;
        case 4:
            return select_channel_4;
            break;
        case 5:
            return select_channel_5;
            break;
        case 6:
            return select_channel_6;
            break;
        case 7:
            return select_channel_7;
            break;

        default:
            break;
        }
    }

private:
    SPIClass spi_bus;
    SPISettings spi_settings;
    uint8_t current_channel_selected;
    uint8_t adc_cs_pin;

    byte select_channel_0 = 0x84;
    byte select_channel_1 = 0xC4;
    byte select_channel_2 = 0x94;
    byte select_channel_3 = 0xD4;
    byte select_channel_4 = 0xA4;
    byte select_channel_5 = 0xE4;
    byte select_channel_6 = 0xB4;
    byte select_channel_7 = 0xF4;
};

// End.
// 0x40 0x88