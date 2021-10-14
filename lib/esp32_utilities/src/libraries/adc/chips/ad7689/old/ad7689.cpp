// // Driver code for the Analog Devices AD7689 16 bit, 8 channel, SPI ADC.
// // Will use default SPI port of the MCU as specified by the SPI.h header
// // All functionality of the chip is currently supported, some of it is untested however
// // Report bugs by opening a ticket on Github:
// // This driver MAY also work for the similar chip AD7682, but this hasn't been tested.
// // Code released under MIT license blah blah blah, use it however you wish, if it has been useful then let me know about your project at yannick [dot] verbelen [at] inventati [dot] org

// #include "ad7689.h"

// /* sends a 16 bit word to the ADC, and simultaneously captures the response
//    ADC responses lag 2 frames behind on commands
//    if readback is activated, 32 bits will be captured instead of 16
// */
// /**
//  * [AD7689::shiftTransaction Sends a 16 bit word to the ADC, and simultaneously captures the response.
//  * ADC responses lag 2 frames behind on commands.
//  * If readback is active, 32 bits will be captured instead of 16.]
//  * @param  command    The 16 bit command to send to the ADC.
//  * @param  readback   True if readback is desired, otherwise False.
//  * @param  rb_cmd_ptr A pointer to a variable to store the readback response.
//  * @return            A 16 bit word received from the ADC, as response to the command from 2 frames ago.
//  */
// uint16_t AD7689::shiftTransaction(uint16_t command, bool readback, uint16_t *rb_cmd_ptr) const
// {

//     // allow time to sample
//     delayMicroseconds(TCONV);

//     adc_spi_bus->beginTransaction(adc_spi_bus_settings);

//     uint16_t data = (adc_spi_bus->transfer(command >> 8) << 8) | adc_spi_bus->transfer(command & 0xFF);

//     // if a readback is requested, the 16 bit frame is extended with another 16 bits to retrieve the value
//     if (readback)
//     {
//         // duplicate previous command
//         uint16_t res = (adc_spi_bus->transfer(command >> 8) << 8) | adc_spi_bus->transfer(command & 0xFF);
//         if (rb_cmd_ptr)
//         {
//             *rb_cmd_ptr = res;
//         }
//     }
//     adc_spi_bus->endTransaction();

//     // delay to allow data acquisition for the next cycle
//     delayMicroseconds(TACQ); // minumum 1.2µs

//     return data;
// }

// // converts a command structure to a 16 bit word that can be transmitted over SPI
// /**
//  * [AD7689::toCommand Converts a command structure to a 16 bit word that can be transmitted over SPI]
//  * @param  cfg A configuration set of type AD7689_conf holding the configuration settings.
//  * @return     A 16 bit configuration command that can be sent to the ADC with shiftTransaction.
//  */
// uint16_t AD7689::toCommand(AD7689_conf cfg) const
// {

//     // build 14 bit configuration word
//     uint16_t command = 0;
//     command |= cfg.CFG_conf << CFG;             // update config on chip
//     command |= (cfg.INCC_conf & 0b111) << INCC; // mode - single ended, differential, ref, etc
//     command |= (cfg.INx_conf & 0b111) << INx;   // channel
//     command |= cfg.BW_conf << BW;               // 1 adds more filtering
//     command |= (cfg.REF_conf & 0b111) << REF;   // internal 4.096V reference
//     command |= (cfg.SEQ_conf & 0b11) << SEQ;    // don't auto sequence
//     command |= !(cfg.RB_conf) << RB;            // read back config value

//     // convert 14 bits to 16 bits, 2 LSB are don't cares
//     command = command << 2;

//     return command;
// }

// // assemble user settings into a configuration for the ADC, or return a default configuration
// /**
//  * [AD7689::getADCConfig Assemble user settings into a configuration for the ADC, or return a default configuration.]
//  * @param  default_config True if the default configuration should be returned, False if user settings are to be used.
//  * @return                Configuration set for the ADC.
//  */
// AD7689_conf AD7689::getADCConfig(bool default_config)
// {
//     AD7689_conf def;

//     def.CFG_conf = true;                   // overwrite existing configuration
//     def.INCC_conf = INCC_UNIPOLAR_REF_GND; // default unipolar inputs, with reference to ground
//     def.INx_conf = TOTAL_CHANNELS;         // read all channels
//     def.BW_conf = true;                    // full bandwidth
//     def.REF_conf = INT_REF_4096;           // use interal 4.096V reference voltage
//     def.SEQ_conf = SEQ_OFF;                // disable sequencer
//     def.RB_conf = false;                   // disable readback

//     if (!default_config)
//     { // default settings preloaded
//         def.INCC_conf = inputConfig;
//         def.INx_conf = (inputCount - 1);
//         def.BW_conf = !filterConfig;
//         def.REF_conf = refConfig;
//     }

//     // sequencer disabled, remember to restart it when taking measurements
//     sequencerActive = false;

//     return def;
// }

// /**
//  * [AD7689::readTemperature Reads the temperature of the ADC.
//  * This function is meant to be called if the ADC is *only* used as a temperature sensor.
//  * Whenever actual ADC values are read, temperature is read along with it, and returned directly.
//  * This function disables the sequencer.]
//  * @return Internal temperature of the ADC in °C.
//  */
// float AD7689::readTemperature()
// {

//     AD7689_conf temp_conf = getADCConfig(false);

//     // set to use internal reference voltage
//     // this automatically turns on the temperature sensor
//     temp_conf.REF_conf = TEMP_REF == INTERNAL_25 ? INT_REF_25 : INT_REF_4096;

//     // configure MUX for temperature sensor
//     temp_conf.INCC_conf = INCC_TEMP;

//     digitalWrite(adc_cs_pin, LOW);
//     digitalWrite(adc_cs_pin, HIGH);

//     delayMicroseconds(TCONV);

//     // send the command
//     shiftTransaction(toCommand(temp_conf), false, NULL);
//     sequencerActive = false;

//     // skip second frame
//     shiftTransaction(toCommand(getADCConfig(false)), false, NULL);

//     // retrieve temperature reading
//     uint16_t t = shiftTransaction(toCommand(getADCConfig(false)), false, NULL);

//     // calculate temperature from ADC value:
//     // output is 283 mV @ 25°C, and sensitivity of 1 mV/°C
//     return calculateTemp(t);
// }

// /**
//  * [AD7689::configureSequencer Enables the automatic channel sequencer of the ADC and turn on temperature measurements.]
//  */
// void AD7689::configureSequencer()
// {
//     // load a new configuration with the setting specified by the user
//     AD7689_conf sequence = getADCConfig();

//     // turn on sequencer if it hasn't been turned on yet, and set it to read temperature too
//     sequence.SEQ_conf = SEQ_SCAN_INPUT_TEMP;
//     // disable readback
//     sequence.RB_conf = false;
//     // overwrite existing command
//     sequence.CFG_conf = true;

//     // convert ADC configuration to command word
//     uint16_t command = toCommand(sequence);

//     // send command to configure ADC and enable sequencer
//     shiftTransaction(command, false, NULL);

//     // skip a frame
//     shiftTransaction(0, false, NULL);
//     lastSeqEndTime = micros();

//     // remember that the sequencer is active to prevent unnecessary intializations
//     sequencerActive = true;
// }

// /**
//  * [AD7689::readChannels Reads voltages as raw 16 bit ADC samples from selected channels. Temperature also read.]
//  * @param channels Last channel to read, starting at 0 to max. 7, in differential mode always read even number of channels.
//  * @param mode     Input signal configuration mode: UNIPOLAR_MODE, BIPOLAR_MODE or DIFFERENTIAL_MODE.
//  * @param data     Pointer to a vector holding the data, length depending on channels and mode.
//  * @param temp     Pointer to a variable holding the temperature.
//  */

// // 2017 08 14 update to try to fix micros() overflow bug
// void AD7689::readChannels(uint8_t channels, uint8_t mode, uint16_t data[], uint16_t *temp)
// {

//     // if the sequencer insn't active yet, enable it
//     // occurs after self testing or at start-up
//     if (!sequencerActive)
//         configureSequencer();

//     uint8_t scans = channels; // unipolar mode default
//     if (mode == DIFFERENTIAL_MODE)
//     {
//         scans = channels / 2;
//         if ((channels % 2) > 0)
//             scans++;
//     }

//     uint32_t now = micros(),
//              nowPlus2Frames = now + +framePeriod * 2;

//     // read as many values as there are ADC channels active
//     // when reading differential, only half the number of channels will be read
//     for (uint8_t ch = 0; ch < scans; ch++)
//     {
//         data[ch] = shiftTransaction(0, false, NULL);
//         if (now < lastSeqEndTime)
//         { // micros() overflow
//             lastSeqEndTime = nowPlus2Frames;
//         }
//         if ((timeStamps[ch] < lastSeqEndTime) || timeStamps[ch] < now)
//         { // micros overflow
//             timeStamps[ch] = nowPlus2Frames;
//         }
//         if (ch < 2)
//         { // calculate time stamp based on ending of previous sequence for first 2 frames
//             timeStamps[ch] = (lastSeqEndTime < nowPlus2Frames) ? nowPlus2Frames : lastSeqEndTime - (1 - ch) * framePeriod;
//         }
//         else
//         {
//             timeStamps[ch] = now; // - framePeriod * 2; // sequenceTime in µs, 2 frames lag
//         }
//     }

//     // capture temperature too
//     *temp = shiftTransaction(0, false, NULL);
//     tempTime = now;       //micros() - framePeriod * 2;
//     lastSeqEndTime = now; // micros();
// }

// /**
//  * [AD7689::calculateVoltage Calculate an absolute or relative voltage based on raw ADC input reading and specified voltage reference(s).]
//  * @param  sample The sample to convert. A positive integer between 0 and 65535.
//  * @return        Calculated voltage.
//  */
// float AD7689::calculateVoltage(uint16_t sample) const
// {
//     return (sample * (posref - negref) / TOTAL_STEPS);
// }

// /**
//  * [AD7689::calculateTemp Calculate the ADC temperature based on raw ADC input readin, using internal voltage reference.]
//  * @param  temp The sample to convert. A positive integer between 0 and 65535.
//  * @return      Temperature in °C.
//  */
// float AD7689::calculateTemp(uint16_t temp) const
// {
//     // calculate temperature from ADC value:
//     // output is 283 mV @ 25°C, and sensitivity of 1 mV/°C
//     // calibration is still necessary, so until done properly, this function returns the raw ADC value instead of temperature

//     // preliminary test results:
//     // raw values range from 4260 at room temperature to over 4400 when heated
//     // need calibration with ice cubes (= 0°C) and boiling methanol (= 64.7°C) or boiling ether (= 34.6°C)
//     //return BASE_TEMP + ((temp * posref / TOTAL_STEPS)- TEMP_BASE_VOLTAGE) * TEMP_RICO;
//     return temp;
// }

// /**
//  * [AD7689::initSampleTiming Reset time stamps for all samples and force an update sequence at the start of the next read command.]
//  * @return The current MCU core time.
//  */
// uint32_t AD7689::initSampleTiming()
// {
//     uint32_t curTime = micros(); // retrieve microcontroller run time in microseconds

//     // set time for all samples to current time to force an update sequence
//     for (uint8_t i = 0; i < TOTAL_CHANNELS; i++)
//         timeStamps[i] = curTime;

//     tempTime = curTime;
//     return curTime;
// }

// /**
//  * [AD7689::cycleTimingBenchmark Measures the time required to transceive a complete 16 bit frame, using the current CPU clock speed.
//  * This is required to generate accurate time stamps, if desired.
//  * Should be called once when starting the ADC, or whenever the clock frequency is changed (i.e. dynamic clock switching).]
//  */
// // 2017 08 14 update to try to fix micros() overflow bug
// void AD7689::cycleTimingBenchmark()
// {
//     const static uint32_t testTime = 10000; // 10 millisecond
//     uint32_t startTime = micros();          // record current CPU time
//     uint16_t data;
//     while (micros() < startTime + testTime)
//         ; // give ourselves time for the test
//     startTime = micros();
//     // make 10 transactions, then average the duration
//     // dummy variable 'data' is assigned to emulate a realistic operation with the retrieved data
//     for (uint8_t trans = 0; trans < 10; trans++)
//     {
//         data += shiftTransaction(toCommand(getADCConfig(false)), false, NULL); // default configuration, no readback
//     }
//     framePeriod = (micros() - startTime) / 10;
//     lastSeqEndTime = startTime;
// }

// /** AD7689::getInputConfig returns an inputConfig value according to the following truth table
//  *  differential  polarity        inputConfig
//  *  TRUE          BIPOLAR_MODE    INCC_BIPOLAR_DIFF
//  *  TRUE          !BIPOLAR_MODE   INCC_UNIPOLAR_DIFF
//  *  FALSE         BIPOLAR_MODE    INCC_BIPOLAR_COM
//  *  FALSE         !BIPOLAR_MODE   INCC_UNIPOLAR_REF_GND
//  *
//  * @param polarity     uint8_t polarity value
//  * @param differential bool indcating differntial mode or not
//  * @return uint8_t inputConfig value according to truth table
//  */
// uint8_t AD7689::getInputConfig(uint8_t polarity, bool differential) const
// {
//     uint8_t res;
//     if (differential)
//     {
//         res = ((polarity == BIPOLAR_MODE) ? INCC_BIPOLAR_DIFF : INCC_UNIPOLAR_DIFF);
//     }
//     else
//     {
//         res = ((polarity == BIPOLAR_MODE) ? INCC_BIPOLAR_COM : INCC_UNIPOLAR_REF_GND);
//     }
//     return res;
// }

// /** AD7689::getNegRef returns a negative reference value according to the following truth table
//  *  polarity        negref
//  *  BIPOLAR_MODE    posR/2.0
//  *  !BIPOLAR_MODE   0
//  *   *
//  * @param posR  float value indicating input positive reference
//  * @param polarity     uint8_t polarity value
//  * @return float negative reference value according to truth table
//  */
// float AD7689::getNegRef(float posR, uint8_t polarity) const
// {
//     return ((polarity == BIPOLAR_MODE) ? posR / 2.0 : 0);
// }

// /** AD7689::getRefSrc returns a reference source value according to the following truth table
//  *  refS          posR            refsrc
//  *  REF_INTERNAL  INTERNAL_25     INT_REF_25;
//  *  REF_INTERNAL  INTERNAL_4096   INT_REF_4096
//  *  REF_INTERNAL  anything else   INT_REF_4096
//  *  !REF_INTERNAL anything        EXT_REF_TEMP_BUF;
//  *
//  * @param refS     uint8_t refernece source value
//  * @param posR float positive reference value
//  * @return uint8_t refsrc value according to truth table
//  */
// uint8_t AD7689::getRefSrc(uint8_t refS, float posR) const
// {
//     uint8_t res = INT_REF_4096;
//     if (posR == INTERNAL_25)
//     {
//         res = INT_REF_25;
//     }
//     if (refS != REF_INTERNAL)
//     {
//         res = EXT_REF_TEMP_BUF;
//     }
//     return res;
// }

// /** AD7689::getPosRef returns a positive reference value according to the following truth table
//  *  refS          posR            posref
//  *  REF_INTERNAL  INTERNAL_25     INTERNAL_25;
//  *  REF_INTERNAL  INTERNAL_4096   INTERNAL_4096
//  *  REF_INTERNAL  anything else   INTERNAL_4096
//  *  !REF_INTERNAL anything        posR
//  * @param refS uint8_t reference source value
//  * @param posR  float value indicating input positive reference
//  * @return float positive reference value according to truth table
//  */
// float AD7689::getPosRef(uint8_t refS, float posR) const
// {
//     float res = ((posR == INTERNAL_25) ? posR : INTERNAL_4096);
//     return ((refS == REF_INTERNAL) ? res : posR);
// }

// /**
//  * [AD7689::constructor Create an instance of an AD7689 ADC. using YSPI]
//  * @param y               Pointer to an instance of YSPI
//  * @param numberChannels  The highest channel in use for the application, a value between 1 and 8.
//  * @return                Instance of the ADC.
//  */
// AD7689::AD7689(uint8_t chip_select_pin,
//                SPIClass *spi_bus,
//                uint8_t numberChannels)
// {
//     adc_spi_bus = spi_bus;
//     adc_cs_pin = chip_select_pin;

//     inputCount = numberChannels;
//     inputConfig = getInputConfig(UNIPOLAR_MODE, false);
//     negref = getNegRef(INTERNAL_25, UNIPOLAR_MODE);

//     refsrc = getRefSrc(REF_INTERNAL, INTERNAL_25);
//     posref = getPosRef(REF_INTERNAL, INTERNAL_25);

//     refConfig = INT_REF_25;

//     //negref(getNegRef(INTERNAL_4096, UNIPOLAR_MODE)),
//     //refsrc(getRefSrc(REF_INTERNAL, INTERNAL_4096)),
//     //posref(getPosRef(REF_INTERNAL, INTERNAL_4096)),
//     //refConfig(INT_REF_4096)

//     // set default configuration options
//     filterConfig = false; // full bandwidth

//     // start-up sequence
//     // give ADC time to start up
//     pinMode(adc_cs_pin, OUTPUT);
//     delay(STARTUP_DELAY);
//     digitalWrite(adc_cs_pin, LOW);
//     delayMicroseconds(TACQ); // miniumum 10 ns
//     digitalWrite(adc_cs_pin, HIGH);
//     delayMicroseconds(TCONV); // minimum 3.2 µs

//     // measure how long it takes to complete a 16-bit r/w cycle using current F_CPU for accurate sample timing
//     cycleTimingBenchmark();

//     // reset sample time stamps and force an update sequence at the next read command
//     initSampleTiming();

//     // sequencer disabled by default
//     sequencerActive = false;
// }

// /**
//  * [AD7689::enableFiltering Configure filtering to reduce bandwidth to 25%.]
//  * @param onOff True to turn on filter, False for full bandwidth.
//  */
// void AD7689::enableFiltering(bool onOff)
// {
//     filterConfig = onOff;
// }

// /**
//  * [AD7689::acquireChannel Sample analog input signal along with its time stamp.]
//  * @param  channel   The channel to sample, between 1 and 8.
//  * @param  timeStamp A pointer to a variable in which the time stamp should be stored.
//  * @return           Measured voltage.
//  */
// // 2017 08 14 update to try to fix micros() overflow bug
// float AD7689::acquireChannel(uint8_t channel, uint32_t *timeStamp)
// {
//     uint32_t now = micros();
//     if (now < timeStamps[channel])
//     {                                                                            // micros() overflow !
//         timeStamps[channel] = micros() + framePeriod * (TOTAL_CHANNELS - 1) + 1; // this will force a channel read, I hope!
//     }

//     if (now > (timeStamps[channel] + framePeriod * (TOTAL_CHANNELS - 1)))
//     { // sequence outdated, acquire a new one
//         uint8_t cycles = 1;
//         if (channel < 2)
//             cycles++; // double sequence to update first 2 channels

//         // run 1 or 2 sequences depending on the outdated channel
//         for (uint8_t cycle = 0; cycle < cycles; cycle++)
//             readChannels(inputCount, ((inputConfig == INCC_BIPOLAR_DIFF) || (inputConfig == INCC_UNIPOLAR_DIFF)), samples, &curTemp);
//     }

//     if (timeStamp)
//     {
//         *timeStamp = timeStamps[channel];
//     }

//     return calculateVoltage(samples[channel]);
// }

// /**
//  * [AD7689::acquireChannel Sample analog input signal along with its time stamp.]
//  * @param  channel   The channel to sample, between 1 and 8.
//  * @return           Measured voltage.
//  */
// // 2017 08 14 update to try to fix micros() overflow bug
// float AD7689::acquireChannel(uint8_t channel)
// {
//     return acquireChannel(channel, NULL);
// }

// /**
//  * [AD7689::acquireTemperature Measure temperature.]
//  * @return Temperature in °C.
//  */
// // 2017 08 14 update to try to fix micros() overflow bug
// float AD7689::acquireTemperature()
// {
//     if (sequencerActive)
//     {
//         uint32_t now = micros();
//         // when the sequencer is active, check the time stamp of the last temperature sample and take a new measurement if outdated
//         if (now > (tempTime + framePeriod * (TOTAL_CHANNELS - 1)))
//         { // temperature outdated, acquire a new one
//             readChannels(inputCount, ((inputConfig == INCC_BIPOLAR_DIFF) || (inputConfig == INCC_UNIPOLAR_DIFF)), &samples[0], &curTemp);
//         }
//         else if (now < tempTime)
//         { // micros() have overflowed, start again
//             tempTime = micros();
//             readChannels(inputCount, ((inputConfig == INCC_BIPOLAR_DIFF) || (inputConfig == INCC_UNIPOLAR_DIFF)), &samples[0], &curTemp);
//         }
//         return calculateTemp(curTemp);
//     }
//     else
//     {
//         // sequencer isn't active yet, fetch tempreature directly
//         return readTemperature();
//     }
// }

// // returns a value indicating if the ADC is properly connected and responding
// /**
//  * [AD7689::selftest Verifies that the ADC is properly connected and operational]
//  * @return True if the ADC works properly, False if errors were encountered.
//  * Check SPI connections if selftest fails repeatedly.
//  */
// bool AD7689::selftest()
// {
//     // ADC will be tested with its readback function, which reads back a previous command
//     // this process takes 3 cycles

//     AD7689_conf rb_conf = getADCConfig(true);
//     rb_conf.RB_conf = true; // enable readback

//     // send readback command
//     shiftTransaction(toCommand(rb_conf), false, NULL);

//     // skip second frame
//     shiftTransaction(toCommand(getADCConfig(false)), false, NULL);

//     // capture readback response
//     uint16_t readback;
//     shiftTransaction(toCommand(getADCConfig(false)), true, &readback);

//     // response with initial readback command
//     bool res = (readback == toCommand(rb_conf));
//     if (!res)
//     {
//         //todo: selfTestFailed();
//     }
//     return res;
// }
