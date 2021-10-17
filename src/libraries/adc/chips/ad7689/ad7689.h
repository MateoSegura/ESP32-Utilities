

#pragma once

/*
* Company: ANZE Suspension
* File Name: ad7689.h
* Project: ESP32 Utilities
* Version: 1.0
* Compartible Hardware:
* Date Created: September 8, 2021
* Last Modified: September 9, 2021
*
* Copyright 2021, Mateo Segura, All rights reserved.
*/

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to offer easy & fast interfacing with the AD7689 Analog to Digital Converter from analog devices
// *

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
// * None.

//*****************************************************        LIBRARIES        *****************************************************/
#include <utils.h>
#include <Arduino.h>
#include <SPI.h>

// reference voltage note: decoupling caps required on REF/REFIN when using INT_REF!
#define INT_REF_25 0b000
#define INT_REF_4096 0b001
#define EXT_REF_TEMP_ON 0b010
#define EXT_REF_TEMP_BUF 0b011
#define EXT_REF_TEMP_OFF 0b110
#define EXT_REF_TEMP_OFF_BUF 0b111

// sequencer configuration default: no sequencer
#define SEQ_OFF 0b00
#define SEQ_UPDATE 0b01
#define SEQ_SCAN_INPUT_TEMP 0b10
#define SEQ_SCAN_INPUT 0b11

#define MAX_FREQ 38000000 // 26 ns period @ VDD 5V and VIO 3.3 - 5V

#define UNIPOLAR_MODE 0
#define BIPOLAR_MODE 1
#define DIFFERENTIAL_MODE 2

#define REF_INTERNAL 0
#define REF_EXTERNAL 1
#define REF_GND 2
#define REF_COM 3

// bit shifts needed for config register values, from datasheet p. 27 table 11:
#define CFG 13
#define INCC 10
#define INx 7
#define BW 6
#define REF 3
#define SEQ 1
#define RB 0

#define TEMP_REF 4.096 // reference voltage to be used for temperature measurement, either 2.5V or 4.096V
#define BASE_TEMP 25
#define TEMP_BASE_VOLTAGE 0.283
#define TEMP_RICO 0.001
#define INTERNAL_25 2.5
#define INTERNAL_4096 4.096
#define TOTAL_CHANNELS 8
#define TOTAL_STEPS 65536
#define TCONV 4
#define TACQ 2
#define STARTUP_DELAY 100

//*****************************************************       DATA TYPES        *****************************************************/
struct AD7689_Configuration
{
    bool update_configuration;
    uint8_t input_channel_configuration;
    uint8_t selected_channel;
    uint8_t bandwith_configuration;
    uint8_t reference_voltage_configuration;
    uint8_t sequencer_configuration;
    float reference_voltage;
    bool readback_enabled;
};

//****************************************************      TERMINAL CLASS        ***************************************************/

class AD7689
{
public:
    ESP_ERROR begin(uint8_t cs_pin, SPIClass esp_spi_bus, uint32_t esp_spi_bus_sck_frequency);

    uint16_t getConfigurationCommand();

    // -- Set configuration
    void setConfigurationUpdate(bool set);
    void setChannelConfiguration(); // polar/ unipolar/ ref, etc.
    void setSelectedChannel(uint8_t channel);
    void setBandwidthFilter(bool set);
    void setReference();
    void setChannelSequencer(bool set, bool temp_enabled);
    void setReadBackRegister(bool set);

    // -- Get configuration state
    void getConfigurationUpdateState();
    void getChannelConfigurationState();
    void getSelectedChannelState();
    void getBandwidthFilterState();
    void getReferenceState();
    void getChannelSequencerState();
    void getReadBackRegisterState();

private:
    AD7689_Configuration adc_configuration;
    SPIClass spi_bus;
    SPISettings spi_bus_settings;
    uint8_t adc_cs_pin;

    uint8_t request_channel;

    uint16_t executeTransaction();

    ESP_ERROR preloadFactorySettings();
};
// End.