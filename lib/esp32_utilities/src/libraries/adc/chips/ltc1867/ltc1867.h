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
    uint16_t begin(uint8_t cs_pin, SPIClass esp_spi_bus, uint32_t esp_spi_bus_sck_frequency)
    {
        // -- This library assumes that SPI port is set externally
        spi_bus = esp_spi_bus;
        spi_settings = SPISettings(esp_spi_bus_sck_frequency, MSBFIRST, SPI_MODE0);

        // -- Pulling CS high begins initial conversion
        pinMode(cs_pin, OUTPUT);
        digitalWrite(cs_pin, HIGH);

        // -- Read dummy variable to clear up possible errors
        delayMicroseconds(5);
        digitalWrite(cs_pin, LOW);
        spi_bus.beginTransaction(spi_settings);
        uint16_t adc_reading = spi_bus.transfer16(switch_channel_0_p << 8 | 0x00); // 2 - bytes into 16 bit
        spi_bus.endTransaction();
        digitalWrite(cs_pin, HIGH);

        delayMicroseconds(5);
        digitalWrite(cs_pin, LOW);
        spi_bus.beginTransaction(spi_settings);
        uint16_t adc_reading2 = spi_bus.transfer16(switch_channel_0_p << 8 | 0x00);
        spi_bus.endTransaction();
        digitalWrite(cs_pin, HIGH);

        spi_bus.endTransaction();
        return adc_reading2;
    }

private:
    SPIClass spi_bus;
    SPISettings spi_settings;

    byte switch_channel_0_p = 0x84;
};

// End.
