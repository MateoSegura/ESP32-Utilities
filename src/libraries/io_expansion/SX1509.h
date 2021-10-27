#pragma once

/*
 * File Name: SX1509.h
 * Project: ESP32 Utilities Libraries
 */

//*********************************************************     READ ME    **********************************************************/

// * The purpose for this library is to allow interfacing with the SX1509 I2C 16 I/O expansion. This library is a direct adaptation of
// * the original library create by Sparkfun, which can be found here: https://github.com/sparkfun/SparkFun_SX1509_Arduino_Library
// *
// * As stated by the people over at Sparkfun:
// *
// * There are functions to take advantage of everything the SX1509 provides - input/output setting, writing pins high/low, reading
// * the input value of pins, LED driver utilities (blink, breath, pwm), and keypad engine utilites.
// *
// * Modified by: Mateo Segura

//*****************************************************     LIBRARY SETTINGS    *****************************************************/
#define RECEIVE_TIMEOUT_VALUE 1000 // Timeout for I2C receive

// These are used for setting LED driver to linear or log mode:
#define LINEAR 0
#define LOGARITHMIC 1

// These are used for clock config:
#define INTERNAL_CLOCK_2MHZ 2
#define EXTERNAL_CLOCK 1

#define SOFTWARE_RESET 0
#define HARDWARE_RESET 1

#define ANALOG_OUTPUT 0x3 // To set a pin mode for PWM output

//*****************************************************        LIBRARIES        *****************************************************/
#include <utils.h>
#include <Arduino.h>
#include <Wire.h>

//****************************************************        SX1509 CLASS        ***************************************************/
class SX1509
{
private:
    //* Device settings
    byte device_address;
    byte pin_interrupt;
    byte pin_oscillator;
    byte pin_reset;
    TwoWire *this_i2c_port;

    //* Misc variables:
    unsigned long _clkX;

    //* Read Functions:
    byte readByte(byte register_address);
    unsigned int readWord(byte register_address);
    void readBytes(byte firstRegister_address, byte *destination, byte length);

    //* Write functions:
    void writeByte(byte register_address, byte write_value);
    void writeWord(byte register_address, unsigned int write_value);
    void writeBytes(byte firstRegister_address, byte *write_array, byte length);

    //*  Helper functions:
    // calculateLEDTRegister - Try to estimate an LED on/off duration register,
    // given the number of milliseconds and LED clock frequency.
    byte calculateLEDTRegister(int ms);

    // calculateSlopeRegister - Try to estimate an LED rise/fall duration
    // register, given the number of milliseconds and LED clock frequency.
    byte calculateSlopeRegister(int ms, byte on_intensity, byte off_intensity);

public:
    // -----------------------------------------------------------------------------
    // begin(byte address, byte resetPin): This function initializes the SX1509.
    //  	It begins the Wire library, resets the IC, and tries to read some
    //  	registers to prove it's connected.
    // Inputs:
    //      - i2c_port: I2C port should be initialized outside this class
    //		- address: should be the 7-bit address of the SX1509. This should be
    //		 one of four values - 0x3E, 0x3F, 0x70, 0x71 - all depending on what the
    //		 ADDR0 and ADDR1 pins ar se to. This variable is required.
    //		- reset_pin: This is the Arduino pin tied to the SX1509 RST pin. This
    //		 pin is optional. If not declared, the library will attempt to
    //		 software reset the SX1509.
    // Output: Returns a debug message that can be printed in higher code levels.
    // -----------------------------------------------------------------------------
    ESP_ERROR begin(TwoWire *i2c_port, byte address = 0x3E, byte reset_pin = 0xFF);

    // -----------------------------------------------------------------------------
    // reset(bool hardware): This function resets the SX1509 - either a hardware
    //		reset or software. A hardware reset (hardware parameter = 1) pulls the
    //		reset line low, pausing, then pulling the reset line high. A software
    //		reset writes a 0x12 then 0x34 to the REG_RESET as outlined in the
    //		datasheet.
    //
    //  Input:
    //	 	- hardware: 0 executes a software reset, 1 executes a hardware reset
    // -----------------------------------------------------------------------------
    void reset(bool hardware);

    // -----------------------------------------------------------------------------
    // pinMode(byte pin, byte inOut): This function sets one of the SX1509's 16
    //		outputs to either an INPUT or OUTPUT.
    //
    //	Inputs:
    //	 	- pin: should be a value between 0 and 15
    //	 	- in_out: The Arduino INPUT and OUTPUT constants should be used for the
    //		 inOut parameter. They do what they say!
    // -----------------------------------------------------------------------------
    void pinMode(byte pin, byte in_out);

    // -----------------------------------------------------------------------------
    // digitalWrite(byte pin, byte highLow): This function writes a pin to either high
    //		or low if it's configured as an OUTPUT. If the pin is configured as an
    //		INPUT, this method will activate either the PULL-UP	or PULL-DOWN
    //		resistor (HIGH or LOW respectively).
    //
    //	Inputs:
    //		- pin: The SX1509 pin number. Should be a value between 0 and 15.
    //		- high_low: should be Arduino's defined HIGH or LOW constants.
    // -----------------------------------------------------------------------------
    void digitalWrite(byte pin, byte high_low);

    // -----------------------------------------------------------------------------
    // digitalRead(byte pin): This function reads the HIGH/LOW status of a pin.
    //		The pin should be configured as an INPUT, using the pinDir function.
    //
    //	Inputs:
    //	 	- pin: The SX1509 pin to be read. should be a value between 0 and 15.
    //  Outputs:
    //		This function returns a 1 if HIGH, 0 if LOW
    // -----------------------------------------------------------------------------
    byte digitalRead(byte pin);

    // -----------------------------------------------------------------------------
    // ledDriverInit(byte pin, byte freq, bool log): This function initializes LED
    //		driving on a pin. It must be called if you want to use the pwm or blink
    //		functions on that pin.
    //
    //	Inputs:
    //		- pin: The SX1509 pin connected to an LED. Should be 0-15.
    //   	- freq: Sets LED clock frequency divider.
    //		- log: selects either linear or logarithmic mode on the LED drivers
    //			- log defaults to 0, linear mode
    //			- currently log sets both bank A and B to the same mode
    //	Note: this function automatically decides to use the internal 2MHz osc.
    // -----------------------------------------------------------------------------
    void ledDriverInit(byte pin, byte freq = 1, bool log = false);

    // -----------------------------------------------------------------------------
    // analogWrite(byte pin, byte iOn):	This function can be used to control the intensity
    //		of an output pin connected to an LED.
    //
    //	Inputs:
    //		- pin: The SX1509 pin connecte to an LED.Should be 0-15.
    //		- i_on: should be a 0-255 value setting the intensity of the LED
    //			- 0 is completely off, 255 is 100% on.
    //
    //	Note: ledDriverInit should be called on the pin before calling this.
    // -----------------------------------------------------------------------------
    void analogWrite(byte pin, byte i_on);

    // -----------------------------------------------------------------------------
    // setupBlink(byte pin, byte t_on, byte t_off, byte off_intensity, byte tRise, byte
    //		tFall):  blink performs both the blink and breath LED driver functions.
    //
    // 	Inputs:
    //  	- pin: the SX1509 pin (0-15) you want to set blinking/breathing.
    //		- t_on: the amount of time the pin is HIGH
    //			- This value should be between 1 and 31. 0 is off.
    //		- t_off: the amount of time the pin is at off_intensity
    //			- This value should be between 1 and 31. 0 is off.
    //		- off_intensity: How dim the LED is during the off period.
    //			- This value should be between 0 and 7. 0 is completely off.
    //		- on_intensity: How bright the LED will be when completely on.
    //			- This value can be between 0 (0%) and 255 (100%).
    //		- t_rise: This sets the time the LED takes to fade in.
    //			- This value should be between 1 and 31. 0 is off.
    //			- This value is used with tFall to make the LED breath.
    //		- t_fall: This sets the time the LED takes to fade out.
    //			- This value should be between 1 and 31. 0 is off.
    // 	 Notes:
    //		- The breathable pins are 4, 5, 6, 7, 12, 13, 14, 15 only. If t_rise and
    //			t_fall are set on 0-3 or 8-11 those pins will still only blink.
    // 		- ledDriverInit should be called on the pin to be blinked before this.
    // -----------------------------------------------------------------------------
    void setupBlink(byte pin, byte t_on, byte t_off, byte on_intensity = 255, byte off_intensity = 0, byte t_rise = 0, byte t_fall = 0, bool log = false);

    // -----------------------------------------------------------------------------
    // blink(byte pin, unsigned long t_on, unsigned long t_off, byte on_intensity, byte off_intensity);
    //  	Set a pin to blink output for estimated on/off millisecond durations.
    //
    // 	Inputs:
    //  	- pin: the SX1509 pin (0-15) you want to set blinking
    //   	- t_on: estimated number of milliseconds the pin is LOW (LED sinking current will be on)
    //   	- t_off: estimated number of milliseconds the pin is HIGH (LED sinking current will be off)
    //   	- on_intensity: 0-255 value determining LED on brightness
    //   	- off_intensity: 0-255 value determining LED off brightness
    // 	 Notes:
    //		- The breathable pins are 4, 5, 6, 7, 12, 13, 14, 15 only. If tRise and
    //			tFall are set on 0-3 or 8-11 those pins will still only blink.
    // 		- ledDriverInit should be called on the pin to be blinked before this.
    // -----------------------------------------------------------------------------
    void blink(byte pin, unsigned long t_on, unsigned long t_off, byte on_intensity = 255, byte off_intensity = 0);

    // -----------------------------------------------------------------------------
    // breathe(byte pin, unsigned long t_on, unsigned long t_off, unsigned long rise, unsigned long fall, byte onInt, byte offInt, bool log);
    //  	Set a pin to breathe output for estimated on/off millisecond durations, with
    //  	estimated rise and fall durations.
    //
    // 	Inputs:
    //  	- pin: the SX1509 pin (0-15) you want to set blinking
    //   	- t_on: estimated number of milliseconds the pin is LOW (LED sinking current will be on)
    //   	- t_off: estimated number of milliseconds the pin is HIGH (LED sinking current will be off)
    //   	- rise: estimated number of milliseconds the pin rises from LOW to HIGH
    //   	- falll: estimated number of milliseconds the pin falls from HIGH to LOW
    //   	- on_intensity: 0-255 value determining LED on brightness
    //   	- off_intensity: 0-255 value determining LED off brightness
    // 	 Notes:
    //		- The breathable pins are 4, 5, 6, 7, 12, 13, 14, 15 only. If tRise and
    //			tFall are set on 0-3 or 8-11 those pins will still only blink.
    // 		- ledDriverInit should be called on the pin to be blinked before this,
    //  	  Or call pinMode(<pin>, ANALOG_OUTPUT);
    // -----------------------------------------------------------------------------
    void breathe(byte pin, unsigned long t_on, unsigned long t_off, unsigned long rise, unsigned long fall, byte on_int = 255, byte off_int = 0, bool log = LINEAR);

    // -----------------------------------------------------------------------------
    // keypad(byte rows, byte columns, byte sleepTime, byte scanTime, byte debounceTime)
    //		Initializes the keypad function on the SX1509. Millisecond durations for sleep,
    //		scan, and debounce can be set.
    //
    //	Inputs:
    //		- rows: The number of rows in the butt_on matrix.
    //			- This value must be between 1 and 7. 0 will turn it off.
    //			- eg: 1 = 2 rows, 2 = 3 rows, 7 = 8 rows, etc.
    //		- columns: The number of columns in the butt_on matrix
    //			- This value should be between 0 and 7.
    //			- 0 = 1 column, 7 = 8 columns, etc.
    //		- sleep_time: Sets the auto-sleep time of the keypad engine.
    //  	  Should be a millisecond duration between 0 (OFF) and 8000 (8 seconds).
    //   	  Possible values are 0, 128, 256, 512, 1000, 2000, 4000, 8000
    //		- scan_time: Sets the scan time per row. Must be set above debounce.
    //  	  Should be a millisecond duration between 1 and 128.
    //   	  Possible values are 1, 2, 4, 8, 16, 32, 64, 128.
    //		- debounce_time: Sets the debounc time per butt_on. Must be set below scan.
    //  	  Should be a millisecond duration between 0 and 64.
    //   	  Possible values are 0 (0.5), 1, 2, 4, 8, 16, 32, 64.
    // -----------------------------------------------------------------------------
    void keypad(byte rows, byte columns, unsigned int sleep_time = 0, byte scan_time = 1, byte debounce_time = 0);

    // -----------------------------------------------------------------------------
    // readKeypad(): This function returns a 16-bit value containing the status of
    //		keypad engine.
    //
    //	Output:
    //		A 16-bit value is returned. The lower 8 bits represent the up-to 8 rows,
    //		while the MSB represents the up-to 8 columns. Bit-values of 1 indicate a
    //		button in that row or column is being pressed. As such, at least two
    //		bits should be set.
    // -----------------------------------------------------------------------------
    unsigned int readKeypad();
    unsigned int readKeyData(); // Legacy: use readKeypad();

    // -----------------------------------------------------------------------------
    // getRow(): This function returns the first active row from the return value of
    //  	readKeypad().
    //
    //	Input:
    //      - key_data: Should be the unsigned int value returned from readKeypad().
    //	Output:
    //		A 16-bit value is returned. The lower 8 bits represent the up-to 8 rows,
    //		while the MSB represents the up-to 8 columns. Bit-values of 1 indicate a
    //		button in that row or column is being pressed. As such, at least two
    //		bits should be set.
    // -----------------------------------------------------------------------------
    byte getRow(unsigned int key_data);

    // -----------------------------------------------------------------------------
    // getCol(): This function returns the first active column from the return value of
    //  	readKeypad().
    //
    //	Input:
    //      - key_data: Should be the unsigned int value returned from readKeypad().
    //	Output:
    //		A 16-bit value is returned. The lower 8 bits represent the up-to 8 rows,
    //		while the MSB represents the up-to 8 columns. Bit-values of 1 indicate a
    //		button in that row or column is being pressed. As such, at least two
    //		bits should be set.
    // -----------------------------------------------------------------------------
    byte getCol(unsigned int key_data);

    // -----------------------------------------------------------------------------
    // sync(void): this function resets the PWM/Blink/Fade counters, syncing any
    //		blinking LEDs. Bit 2 of REG_MISC is set, which alters the functionality
    //		of the nReset pin. The nReset pin is toggled low->high, which should
    //		reset all LED counters. Bit 2 of REG_MISC is again cleared, returning
    //		nReset pin to POR functionality
    // -----------------------------------------------------------------------------
    void sync(void);

    // -----------------------------------------------------------------------------
    // debounceConfig(byte configValue): This method configures the debounce time of
    //		every input.
    //
    //	Input:
    //		- config_value: A 3-bit value configuring the debounce time.
    //			000: 0.5ms * 2MHz/fOSC
    //			001: 1ms * 2MHz/fOSC
    //			010: 2ms * 2MHz/fOSC
    //			011: 4ms * 2MHz/fOSC
    //			100: 8ms * 2MHz/fOSC
    //			101: 16ms * 2MHz/fOSC
    //			110: 32ms * 2MHz/fOSC
    //			111: 64ms * 2MHz/fOSC
    //
    //	Note: fOSC is set with the configClock function. It defaults to 2MHz.
    // -----------------------------------------------------------------------------
    void debounceConfig(byte config_value);

    // -----------------------------------------------------------------------------
    // debounceTime(byte configValue): This method configures the debounce time of
    //		every input to an estimated millisecond time duration.
    //
    //	Input:
    //		- time: A millisecond duration estimating the debounce time. Actual
    //		  debounce time will depend on fOSC. Assuming it's 2MHz, debounce will
    //		  be set to the 0.5, 1, 2, 4, 8, 16, 32, or 64 ms (whatever's closest)
    //
    //	Note: fOSC is set with the configClock function. It defaults to 2MHz.
    // -----------------------------------------------------------------------------
    void debounceTime(byte time);

    // -----------------------------------------------------------------------------
    // debouncePin(byte pin): This method enables debounce on SX1509 input pin.
    //
    //	Input:
    //		- pin: The SX1509 pin to be debounced. Should be between 0 and 15.
    // -----------------------------------------------------------------------------
    void debouncePin(byte pin);
    void debounceEnable(byte pin); // Legacy, use debouncePin

    // -----------------------------------------------------------------------------
    // debounceKeypad(byte pin): This method enables debounce on all pins connected
    //  to a row/column keypad matrix.
    //
    //	Input:
    //		- time: Millisecond time estimate for debounce (see debounceTime()).
    //		- numRows: The number of rows in the keypad matrix.
    //		- numCols: The number of columns in the keypad matrix.
    // -----------------------------------------------------------------------------
    void debounceKeypad(byte time, byte num_rows, byte num_cols);

    // -----------------------------------------------------------------------------
    // enableInterrupt(byte pin, byte riseFall): This function sets up an interrupt
    //		on a pin. Interrupts can occur on all SX1509 pins, and can be generated
    //		on rising, falling, or both.
    //
    //	Inputs:
    //		-pin: SX1509 input pin that will generate an input. Should be 0-15.
    //		-riseFall: Configures if you want an interrupt generated on rise fall or
    //			both. For this param, send the pin-change values previously defined
    //			by Arduino:
    //			#define CHANGE 1	<-Both
    //			#define FALLING 2	<- Falling
    //			#define RISING 3	<- Rising
    //
    //	Note: This function does not set up a pin as an input, or configure	its
    //		pull-up/down resistors! Do that before (or after).
    // -----------------------------------------------------------------------------
    void enableInterrupt(byte pin, byte rise_fall);

    // -----------------------------------------------------------------------------
    // interruptSource(void): Returns an unsigned int representing which pin caused
    //		an interrupt.
    //
    //	Output: 16-bit value, with a single bit set representing the pin(s) that
    //		generated an interrupt. E.g. a return value of	0x0104 would mean pins 8
    //		and 3 (bits 8 and 3) have generated an interrupt.
    //  Input:
    //  	- clear: boolean commanding whether the interrupt should be cleared
    //  	  after reading or not.
    // -----------------------------------------------------------------------------
    unsigned int interruptSource(bool clear = true);

    // -----------------------------------------------------------------------------
    // checkInterrupt(void): Checks if a single pin generated an interrupt.
    //
    //	Output: Boolean value. True if the requested pin has triggered an interrupt/
    //  Input:
    //  	- pin: Pin to be checked for generating an input.
    // -----------------------------------------------------------------------------
    bool checkInterrupt(int pin);

    // -----------------------------------------------------------------------------
    // configClock(byte osc_source, byte oscPinFunction, byte osc_freq_out, byte osc_divider)
    //		This function configures the oscillator source/speed
    //		and the clock, which is used to drive LEDs and time debounces.
    //
    //	Inputs:
    //	- osc_source: Choose either internal 2MHz oscillator or an external signal
    //		applied to the OSCIO pin.
    //		- INTERNAL_CLOCK and EXTERNAL_CLOCK are defined in the header file.
    //			Use those.
    //		- This value defaults to internal.
    //	- osc_divider: Sets the clock divider in REG_MISC.
    //		- ClkX = fOSC / (2^(RegMisc[6:4] -1))
    //		- This value defaults to 1.
    //	- oscPinFunction: Allows you to set OSCIO as an input or output.
    //		- You can use Arduino's INPUT, OUTPUT defines for this value
    //		- This value defaults to input
    //	- osc_freq_out: If oscio is configured as an output, this will set the output
    //		frequency
    //		- This should be a 4-bit value. 0=0%, 0xF=100%, else
    //			fOSCOut = FOSC / (2^(RegClock[3:0]-1))
    //		- This value defaults to 0.
    // -----------------------------------------------------------------------------
    void configClock(byte osc_source = 2, byte osc_pin_function = 0, byte osc_freq_out = 0, byte osc_divider = 1); // Legacy, use clock();

    // -----------------------------------------------------------------------------
    // clock(byte osc_source, byte osc_divider, byte oscPinFunction, byte osc_freq_out)
    //		This function configures the oscillator source/speed
    //		and the clock, which is used to drive LEDs and time debounces.
    //  	This is just configClock in a bit more sane order.
    //
    // -----------------------------------------------------------------------------
    void clock(byte osc_source = 2, byte osc_divider = 1, byte osc_pin_function = 0, byte osc_freq_out = 0);
};

// End.
