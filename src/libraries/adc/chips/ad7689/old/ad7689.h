// #pragma once

// /*
// * Company: ANZE Suspension
// * File Name: ad7689.h
// * Project: ESP32 Utilities
// * Version: 1.0
// * Compartible Hardware:
// * Date Created: September 8, 2021
// * Last Modified: September 9, 2021
// *
// * Copyright 2021, Mateo Segura, All rights reserved.
// */

// //*********************************************************     READ ME    **********************************************************/

// // * The purpose for this library is to offer easy & fast interfacing with the AD7689 Analog to Digital Converter from analog devices
// // *

// //*****************************************************     LIBRARY SETTINGS    *****************************************************/
// // * None.

// //*****************************************************        LIBRARIES        *****************************************************/

// #include <Arduino.h>
// #include <utils.h>
// #include <SPI.h>

// #include "cfg_register.h"

// //******************************************************        SETTINGS        *****************************************************/

// #define TEMP_REF (4.096) // reference voltage to be used for temperature measurement, either 2.5V or 4.096V
// #define BASE_TEMP (25)
// #define TEMP_BASE_VOLTAGE (0.283)
// #define TEMP_RICO (0.001)
// #define INTERNAL_25 (2.5)
// #define INTERNAL_4096 (4.096)
// #define TOTAL_CHANNELS (8)
// #define TOTAL_STEPS (65536)
// #define TCONV (4)
// #define TACQ (2)
// #define STARTUP_DELAY (100)

// //*****************************************************       DATA TYPES        *****************************************************/

// /** Configuration settings of the ADC.
//  *  This should *not* be modified directly by the user.
//  *  Incorrect configuration combinations may lead to unpredictable results and potentially damage the ADC!
//  */
// struct AD7689_conf
// {
//     bool CFG_conf;     /*!< True if configuration command should be updated. */
//     uint8_t INCC_conf; /*!< Input channel configuration */
//     uint8_t INx_conf;  /*!< Number of activated input channels */
//     uint8_t BW_conf;   /*!< Bandwidth filtering */
//     uint8_t REF_conf;  /*!< Reference voltage settings */
//     uint8_t SEQ_conf;  /*!< Sequencer settings */
//     float REF_voltage; /*!< Reference voltages */
//     bool RB_conf;      /*!< True if readback should be enabled */
// };

// //*****************************************************       AD7689 CLASS        ****************************************************/

// /**
//  * Represents the Analog Devices AD7689, an ADC with 8 channels and 16 bit resolution.
//  */
// class AD7689
// {
// protected:
//     AD7689_conf conf; /*!< Configuration settings for the ADC. */

//     float posref;        /*!< Positive voltage reference for unipolar or bipolar mode. */
//     float negref;        /*!< Negative voltage reference, either COM or ground. */
//     uint8_t refsrc;      /*!< Positive voltage eference source. */
//     uint8_t inputConfig; /*!< Input channel configuration. */
//     uint8_t refConfig;   /*!< Voltage reference configuration. */

//     uint32_t timeStamps[TOTAL_CHANNELS]; /*!< Last set of time stamps for each channel. */
//     uint16_t samples[TOTAL_CHANNELS];    /*!< Last set of samples for each channel. */
//     uint16_t framePeriod;                /*!< Length of a single frame, in microseconds. */
//     uint16_t curTemp;                    /*!< Last temperature measurement. */
//     uint16_t tempTime;                   /*!< Time stamp for last temperature measurement. */
//     uint32_t lastSeqEndTime;             /*!< Time stamp of the end of the last data acquisition sequence. */
//     uint8_t inputCount;                  /*!< Number of input channels. Even for differential mode. */

//     uint8_t adc_cs_pin;
//     SPIClass *adc_spi_bus;
//     SPISettings adc_spi_bus_settings;

//     bool sequencerActive; /*!< True when the sequencer is initialized, false at start-up or during self tests */
//     bool filterConfig;    /*!< Input filter configuration. */

//     uint16_t shiftTransaction(uint16_t command, bool readback, uint16_t *rb_cmd_ptr) const;
//     uint16_t toCommand(AD7689_conf cfg) const;
//     AD7689_conf getADCConfig(bool default_config = false);
//     float readTemperature(void);
//     void configureSequencer();
//     void readChannels(uint8_t channels, uint8_t mode, uint16_t *data, uint16_t *temp);
//     float calculateVoltage(uint16_t sample) const;
//     float calculateTemp(uint16_t temp) const;
//     uint32_t initSampleTiming(void);
//     void cycleTimingBenchmark(void);

//     // initialisation funcitons
//     uint8_t getInputConfig(uint8_t polarity, bool differential) const;
//     float getNegRef(float posR, uint8_t polarity) const;
//     uint8_t getRefSrc(uint8_t refS, float posR) const;
//     float getPosRef(uint8_t refS, float posR) const;

// public:
//     AD7689(uint8_t chip_select_pin, SPIClass *spi_bus, uint8_t numberChannels = TOTAL_CHANNELS);
//     void enableFiltering(bool onOff);
//     float acquireChannel(uint8_t channel, uint32_t *timeStamp);
//     float acquireChannel(uint8_t channel);
//     float acquireTemperature();
//     bool selftest(void);
// };
