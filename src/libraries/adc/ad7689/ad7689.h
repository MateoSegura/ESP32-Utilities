#ifndef AD7689_H
#define AD7689_H

#include <Arduino.h>
#include <utils.h>
#include <SPI.h>

// input configuration: bipolar/unipolar, single ended or differential
#define INCC_BIPOLAR_DIFF (0b000) // 00X
#define INCC_BIPOLAR_COM (0b010)
#define INCC_TEMP (0b011)
#define INCC_UNIPOLAR_DIFF (0b100) // 10X
#define INCC_UNIPOLAR_REF_COM (0b110)
#define INCC_UNIPOLAR_REF_GND (0b111)

// reference voltage (note: decoupling caps required on REF/REFIN when using INT_REF!)
#define INT_REF_25 (0b000)
#define INT_REF_4096 (0b001)
#define EXT_REF_TEMP_ON (0b010)
#define EXT_REF_TEMP_BUF (0b011)
#define EXT_REF_TEMP_OFF (0b110)
#define EXT_REF_TEMP_OFF_BUF (0b111)

// sequencer configuration (default: no sequencer)
#define SEQ_OFF (0b00)
#define SEQ_UPDATE (0b01)
#define SEQ_SCAN_INPUT_TEMP (0b10)
#define SEQ_SCAN_INPUT (0b11)

#define MAX_FREQ (38000000) // 26 ns period @ VDD 5V and VIO 3.3 - 5V

#define UNIPOLAR_MODE (0)
#define BIPOLAR_MODE (1)
#define DIFFERENTIAL_MODE (2)

#define REF_INTERNAL (0)
#define REF_EXTERNAL (1)
#define REF_GND (2)
#define REF_COM (3)

// bit shifts needed for config register values, from datasheet p. 27 table 11:
#define CFG (13)
#define INCC (10)
#define INx (7)
#define BW (6)
#define REF (3)
#define SEQ (1)
#define RB (0)

#define TEMP_REF (4.096) // reference voltage to be used for temperature measurement, either 2.5V or 4.096V
#define BASE_TEMP (25)
#define TEMP_BASE_VOLTAGE (0.283)
#define TEMP_RICO (0.001)
#define INTERNAL_25 (2.5)
#define INTERNAL_4096 (4.096)
#define TOTAL_CHANNELS (8)
#define TOTAL_STEPS (65536)
#define TCONV (4)
#define TACQ (2)
#define STARTUP_DELAY (100)

/** Configuration settings of the ADC.
 *  This should *not* be modified directly by the user.
 *  Incorrect configuration combinations may lead to unpredictable results and potentially damage the ADC!
 */
struct AD7689_conf
{
    bool CFG_conf;     /*!< True if configuration command should be updated. */
    uint8_t INCC_conf; /*!< Input channel configuration */
    uint8_t INx_conf;  /*!< Number of activated input channels */
    uint8_t BW_conf;   /*!< Bandwidth filtering */
    uint8_t REF_conf;  /*!< Reference voltage settings */
    uint8_t SEQ_conf;  /*!< Sequencer settings */
    float REF_voltage; /*!< Reference voltages */
    bool RB_conf;      /*!< True if readback should be enabled */
};

/**
 * Represents the Analog Devices AD7689, an ADC with 8 channels and 16 bit resolution.
 */
class AD7689
{
protected:
    AD7689_conf conf; /*!< Configuration settings for the ADC. */

    float posref;        /*!< Positive voltage reference for unipolar or bipolar mode. */
    float negref;        /*!< Negative voltage reference, either COM or ground. */
    uint8_t refsrc;      /*!< Positive voltage eference source. */
    uint8_t inputConfig; /*!< Input channel configuration. */
    uint8_t refConfig;   /*!< Voltage reference configuration. */

    SPIClass *adc_spi_bus;
    SPISettings adc_spi_settings;
    uint8_t adc_cs_pin;

    uint32_t timeStamps[TOTAL_CHANNELS]; /*!< Last set of time stamps for each channel. */
    uint16_t samples[TOTAL_CHANNELS];    /*!< Last set of samples for each channel. */
    uint16_t framePeriod;                /*!< Length of a single frame, in microseconds. */
    uint16_t curTemp;                    /*!< Last temperature measurement. */
    uint16_t tempTime;                   /*!< Time stamp for last temperature measurement. */
    uint32_t lastSeqEndTime;             /*!< Time stamp of the end of the last data acquisition sequence. */
    uint8_t inputCount;                  /*!< Number of input channels. Even for differential mode. */

    bool sequencerActive; /*!< True when the sequencer is initialized, false at start-up or during self tests */
    bool filterConfig;    /*!< Input filter configuration. */

    uint16_t shiftTransaction(uint16_t command, bool readback, uint16_t *rb_cmd_ptr);
    uint16_t toCommand(AD7689_conf cfg);
    AD7689_conf getADCConfig(bool default_config = false);
    float readTemperature(void);
    void configureSequencer();
    void readChannels(uint8_t channels, uint8_t mode, uint16_t *data, uint16_t *temp);
    float calculateVoltage(uint16_t sample);
    float calculateTemp(uint16_t temp);
    uint32_t initSampleTiming(void);
    void cycleTimingBenchmark(void);

    // initialisation funcitons
    uint8_t getInputConfig(uint8_t polarity, bool differential);
    float getNegRef(float posR, uint8_t polarity);
    uint8_t getRefSrc(uint8_t refS, float posR);
    float getPosRef(uint8_t refS, float posR);

public:
    ESP_ERROR begin(uint8_t cs_pin, SPIClass *spi_bus, uint64_t spi_bus_clk_frequency);
    void enableFiltering(bool onOff);
    float acquireChannel(uint8_t channel, uint32_t *timeStamp);
    float acquireChannel(uint8_t channel);
    float acquireTemperature();
    bool selftest(void);
};
#endif
