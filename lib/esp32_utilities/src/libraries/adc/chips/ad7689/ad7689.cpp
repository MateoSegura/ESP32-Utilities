/* 
* Company: ANZE Suspension
* File Name: Terminal.cpp
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware: 
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*****************************************************       HEADER FILE       *****************************************************/
#include "ad7689.h"

//*******************************************       AD7869 CLASS FUNCTIONS DEFINTION       ******************************************/
ESP_ERROR AD7689::begin(uint8_t cs_pin, SPIClass esp_spi_bus, uint32_t esp_spi_bus_sck_frequency)
{
    ESP_ERROR err;
    err.on_error = false;

    // -- This library assumes that SPI port is set externally
    spi_bus = esp_spi_bus;
    spi_bus_settings = SPISettings(esp_spi_bus_sck_frequency, MSBFIRST, SPI_MODE0);
    adc_cs_pin = cs_pin;

    // -- Pulling CS high begins initial conversion
    pinMode(adc_cs_pin, OUTPUT);
    digitalWrite(adc_cs_pin, HIGH);

    setConfigurationUpdate(true);
    setChannelConfiguration();
    setSelectedChannel(0);
    setBandwidthFilter(false);
    setReference();
    setChannelSequencer(false, false);
    setReadBackRegister(true);

    spi_bus.beginTransaction(spi_bus_settings);

    uint16_t init_command = getConfigurationCommand();

    setSelectedChannel(0);
    digitalWrite(adc_cs_pin, LOW);
    spi_bus.transfer16(getConfigurationCommand());
    digitalWrite(adc_cs_pin, HIGH);

    setReadBackRegister(false);

    // -- Initial read with readback enabled
    setSelectedChannel(1);
    digitalWrite(adc_cs_pin, LOW);
    spi_bus.transfer16(getConfigurationCommand());
    digitalWrite(adc_cs_pin, HIGH);

    // -- Second read to read config register
    setSelectedChannel(2);
    digitalWrite(adc_cs_pin, LOW);
    uint16_t config_register = spi_bus.transfer16(getConfigurationCommand());
    digitalWrite(adc_cs_pin, HIGH);

    setSelectedChannel(3);
    digitalWrite(adc_cs_pin, LOW);
    uint16_t channel1 = spi_bus.transfer16(getConfigurationCommand());
    digitalWrite(adc_cs_pin, HIGH);

    setSelectedChannel(4);
    digitalWrite(adc_cs_pin, LOW);
    uint16_t channel2 = spi_bus.transfer16(getConfigurationCommand());
    digitalWrite(adc_cs_pin, HIGH);

    spi_bus.endTransaction();

    if (config_register != init_command)
    {
        err.on_error = true;
        err.debug_message = channel2 * 0.0000625;
    }

    digitalWrite(adc_cs_pin, HIGH);

    return err;
}

uint16_t AD7689::getConfigurationCommand()
{
    // build 14 bit configuration word
    uint16_t command = 0;
    command |= (adc_configuration.update_configuration) << CFG;            // update config on chip
    command |= (adc_configuration.input_channel_configuration) << INCC;    // mode - single ended, differential, ref, etc
    command |= (adc_configuration.selected_channel) << INx;                // channel
    command |= (adc_configuration.bandwith_configuration) << BW;           // 1 adds more filtering
    command |= (adc_configuration.reference_voltage_configuration) << REF; // internal 4.096V reference
    command |= (adc_configuration.sequencer_configuration) << SEQ;         // don't auto sequence
    command |= !(adc_configuration.readback_enabled) << RB;                // read back config value

    // convert 14 bits to 16 bits, 2 LSB are don't cares
    command = command << 2;

    return command;
}

// -- Set configuration
void AD7689::setConfigurationUpdate(bool set)
{
    adc_configuration.update_configuration = set;
}

// input configuration: bipolar/unipolar, single ended or differential
#define INCC_BIPOLAR_DIFF 0x000 // 00X
#define INCC_BIPOLAR_COM 0x010
#define INCC_TEMP 0x011
#define INCC_UNIPOLAR_DIFF 0b100 // 10X
#define INCC_UNIPOLAR_REF_COM 0b110
#define INCC_UNIPOLAR_REF_GND 0b111

void AD7689::setChannelConfiguration()
{
    adc_configuration.input_channel_configuration = INCC_UNIPOLAR_REF_GND;
}

// Channel #
void AD7689::setSelectedChannel(uint8_t channel)
{
    switch (channel)
    {
    case 0:
        adc_configuration.selected_channel = 0b000;
        break;
    case 1:
        adc_configuration.selected_channel = 0b001;
        break;
    case 2:
        adc_configuration.selected_channel = 0b010;
        break;
    case 3:
        adc_configuration.selected_channel = 0b011;
        break;
    case 4:
        adc_configuration.selected_channel = 0b100;
        break;
    case 5:
        adc_configuration.selected_channel = 0b101;
        break;
    case 6:
        adc_configuration.selected_channel = 0b110;
        break;
    case 7:
        adc_configuration.selected_channel = 0b111;
        break;
    default:
        break;
    }
}

void AD7689::setBandwidthFilter(bool set)
{
    adc_configuration.bandwith_configuration = set;
}

// reference voltage note: decoupling caps required on REF/REFIN when using INT_REF!
#define INT_REF_25 0b000
#define INT_REF_4096 0b001
#define EXT_REF_TEMP_ON 0b010
#define EXT_REF_TEMP_BUF 0b011
#define EXT_REF_TEMP_OFF 0b110
#define EXT_REF_TEMP_OFF_BUF 0b111

void AD7689::setReference()
{
    adc_configuration.reference_voltage_configuration = INT_REF_4096;
}

// sequencer configuration default: no sequencer
#define SEQ_OFF 0b00
#define SEQ_UPDATE 0b01
#define SEQ_SCAN_INPUT_TEMP 0b10
#define SEQ_SCAN_INPUT 0b11

void AD7689::setChannelSequencer(bool set, bool temp_enabled)
{
    if (set)
    {
        if (temp_enabled)
        {
            adc_configuration.sequencer_configuration = SEQ_SCAN_INPUT_TEMP;
        }
        else
        {
            adc_configuration.sequencer_configuration = SEQ_SCAN_INPUT;
        }
    }
    else
    {
        adc_configuration.sequencer_configuration = SEQ_OFF;
    }
}

void AD7689::setReadBackRegister(bool set)
{
    adc_configuration.readback_enabled = set;
}

// End.