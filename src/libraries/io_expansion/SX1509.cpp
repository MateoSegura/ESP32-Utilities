/*
 * File Name: SX1509.cpp
 * Project: ESP32 Utilities
 */

//*****************************************************       HEADER FILE       *****************************************************/
#include "SX1509.h"
#include "util/sx1509_registers.h"

//*****************************************        SX1509 CLASS FUNCTIONS DEFINTION        ******************************************/

ESP_ERROR SX1509::begin(TwoWire *i2c_port, byte address, byte reset_pin)
{
    ESP_ERROR err;

    //* 1. Assign external variables
    device_address = address;
    pin_reset = reset_pin;

    //* 2. Get I2C port
    this_i2c_port = i2c_port;

    //* 3. Reset if reset pin is connected
    if (pin_reset != 255)
        reset(1); // Hardware reset
    else
        reset(0); // Software reset

    //* 4. Communication test. We'll read from a register to verify communication
    unsigned int test_registers = 0;
    test_registers = readWord(REG_INTERRUPT_MASK_A); // This should return 0xFF00

    if (test_registers == 0xFF00)
    {
        clock(INTERNAL_CLOCK_2MHZ); // Set the clock to a default of 2MHz using internal
    }
    else
    {
        err.on_error = true;
        err.debug_message = "SX1509 did not return 0xFF00 from register";
    }

    return err;
}

void SX1509::reset(bool hardware)
{
    // if hardware bool is set
    if (hardware)
    {
        // Check if bit 2 of REG_MISC is set
        // if so nReset will not issue a POR, we'll need to clear that bit first
        byte reg_misc = readByte(REG_MISC);
        if (reg_misc & (1 << 2))
        {
            reg_misc &= ~(1 << 2);
            writeByte(REG_MISC, reg_misc);
        }
        // Reset the SX1509, the pin is active low
        pinMode(pin_reset, OUTPUT);    // set reset pin as output
        digitalWrite(pin_reset, LOW);  // pull reset pin low
        delay(1);                      // Wait for the pin to settle
        digitalWrite(pin_reset, HIGH); // pull reset pin back high
    }
    else
    {
        // Software reset command sequence:
        writeByte(REG_RESET, 0x12);
        writeByte(REG_RESET, 0x34);
    }
}

void SX1509::pinMode(byte pin, byte in_out)
{
    // The SX1509 RegDir registers: REG_DIR_B, REG_DIR_A
    //	0: IO is configured as an output
    //	1: IO is configured as an input
    byte mode_bit;
    if ((in_out == OUTPUT) || (in_out == ANALOG_OUTPUT))
        mode_bit = 0;
    else
        mode_bit = 1;

    unsigned int temp_reg_dir = readWord(REG_DIR_B);
    if (mode_bit)
        temp_reg_dir |= (1 << pin);
    else
        temp_reg_dir &= ~(1 << pin);

    writeWord(REG_DIR_B, temp_reg_dir);

    // If INPUT_PULLUP was called, set up the pullup too:
    if (in_out == INPUT_PULLUP)
        digitalWrite(pin, HIGH);

    if (in_out == ANALOG_OUTPUT)
    {
        ledDriverInit(pin);
    }
}

void SX1509::digitalWrite(byte pin, byte high_low)
{
    unsigned int temp_reg_dir = readWord(REG_DIR_B);

    if ((0xFFFF ^ temp_reg_dir) & (1 << pin)) // If the pin is an output, write high/low
    {
        unsigned int temp_reg_data = readWord(REG_DATA_B);
        if (high_low)
            temp_reg_data |= (1 << pin);
        else
            temp_reg_data &= ~(1 << pin);
        writeWord(REG_DATA_B, temp_reg_data);
    }
    else // Otherwise the pin is an input, pull-up/down
    {
        unsigned int temp_pull_up = readWord(REG_PULL_UP_B);
        unsigned int temp_pull_down = readWord(REG_PULL_DOWN_B);

        if (high_low) // if HIGH, do pull-up, disable pull-down
        {
            temp_pull_up |= (1 << pin);
            temp_pull_down &= ~(1 << pin);
            writeWord(REG_PULL_UP_B, temp_pull_up);
            writeWord(REG_PULL_DOWN_B, temp_pull_down);
        }
        else // If LOW do pull-down, disable pull-up
        {
            temp_pull_down |= (1 << pin);
            temp_pull_up &= ~(1 << pin);
            writeWord(REG_PULL_UP_B, temp_pull_up);
            writeWord(REG_PULL_DOWN_B, temp_pull_down);
        }
    }
}

byte SX1509::digitalRead(byte pin)
{
    unsigned int temp_reg_dir = readWord(REG_DIR_B);

    if (temp_reg_dir & (1 << pin)) // If the pin is an input
    {
        unsigned int temp_reg_data = readWord(REG_DATA_B);
        if (temp_reg_data & (1 << pin))
            return 1;
    }

    return 0;
}

void SX1509::ledDriverInit(byte pin, byte freq /*= 1*/, bool log /*= false*/)
{
    unsigned int temp_word;
    byte temp_byte;

    // Disable input buffer
    // Writing a 1 to the pin bit will disable that pins input buffer
    temp_word = readWord(REG_INPUT_DISABLE_B);
    temp_word |= (1 << pin);
    writeWord(REG_INPUT_DISABLE_B, temp_word);

    // Disable pull-up
    // Writing a 0 to the pin bit will disable that pull-up resistor
    temp_word = readWord(REG_PULL_UP_B);
    temp_word &= ~(1 << pin);
    writeWord(REG_PULL_UP_B, temp_word);

    // Set direction to output (REG_DIR_B)
    temp_word = readWord(REG_DIR_B);
    temp_word &= ~(1 << pin); // 0=output
    writeWord(REG_DIR_B, temp_word);

    // Enable oscillator (REG_CLOCK)
    temp_byte = readByte(REG_CLOCK);
    temp_byte |= (1 << 6);  // Internal 2MHz oscillator part 1 (set bit 6)
    temp_byte &= ~(1 << 5); // Internal 2MHz oscillator part 2 (clear bit 5)
    writeByte(REG_CLOCK, temp_byte);

    // Configure LED driver clock and mode (REG_MISC)
    temp_byte = readByte(REG_MISC);
    if (log)
    {
        temp_byte |= (1 << 7); // set logarithmic mode bank B
        temp_byte |= (1 << 3); // set logarithmic mode bank A
    }
    else
    {
        temp_byte &= ~(1 << 7); // set linear mode bank B
        temp_byte &= ~(1 << 3); // set linear mode bank A
    }

    // Use configClock to setup the clock divder
    if (_clkX == 0) // Make clckX non-zero
    {
        _clkX = 2000000.0 / (1 << (1 - 1)); // Update private clock variable

        byte freq = (1 & 0x07) << 4; // freq should only be 3 bits from 6:4
        temp_byte |= freq;
    }
    writeByte(REG_MISC, temp_byte);

    // Enable LED driver operation (REG_LED_DRIVER_ENABLE)
    temp_word = readWord(REG_LED_DRIVER_ENABLE_B);
    temp_word |= (1 << pin);
    writeWord(REG_LED_DRIVER_ENABLE_B, temp_word);

    // Set REG_DATA bit low ~ LED driver started
    temp_word = readWord(REG_DATA_B);
    temp_word &= ~(1 << pin);
    writeWord(REG_DATA_B, temp_word);
}

void SX1509::analogWrite(byte pin, byte i_on)
{
    // Write the on intensity of pin
    // Linear mode: i_on = i_on
    // Log mode: i_on = f(i_on)
    writeByte(REG_I_ON[pin], i_on);
}

void SX1509::blink(byte pin, unsigned long t_on, unsigned long t_off, byte on_intensity, byte off_intensity)
{
    byte on_reg = calculateLEDTRegister(t_on);
    byte off_reg = calculateLEDTRegister(t_off);

    setupBlink(pin, on_reg, off_reg, on_intensity, off_intensity, 0, 0);
}

void SX1509::breathe(byte pin, unsigned long t_on, unsigned long t_off, unsigned long rise, unsigned long fall, byte on_int, byte off_int, bool log)
{
    off_int = constrain(off_int, 0, 7);

    byte on_reg = calculateLEDTRegister(t_on);
    byte off_reg = calculateLEDTRegister(t_off);

    byte rise_time = calculateSlopeRegister(rise, on_int, off_int);
    byte fall_time = calculateSlopeRegister(fall, on_int, off_int);

    setupBlink(pin, on_reg, off_reg, on_int, off_int, rise_time, fall_time, log);
}

void SX1509::setupBlink(byte pin, byte t_on, byte t_off, byte on_intensity, byte off_intensity, byte t_rise, byte t_fall, bool log)
{
    ledDriverInit(pin, log);

    // Keep parameters within their limits:
    t_on &= 0x1F;  // t_on should be a 5-bit value
    t_off &= 0x1F; // t_off should be a 5-bit value
    off_intensity &= 0x07;
    // Write the time on
    // 1-15:  TON = 64 * t_on * (255/ClkX)
    // 16-31: TON = 512 * t_on * (255/ClkX)
    writeByte(REG_T_ON[pin], t_on);

    // Write the time/intensity off register
    // 1-15:  TOFF = 64 * t_off * (255/ClkX)
    // 16-31: TOFF = 512 * t_off * (255/ClkX)
    // linear Mode - IOff = 4 * off_intensity
    // log mode - Ioff = f(4 * off_intensity)
    writeByte(REG_OFF[pin], (t_off << 3) | off_intensity);

    // Write the on intensity:
    writeByte(REG_I_ON[pin], on_intensity);

    // Prepare t_rise and t_fall
    t_rise &= 0x1F; // t_rise is a 5-bit value
    t_fall &= 0x1F; // t_fall is a 5-bit value

    // Write regTRise
    // 0: Off
    // 1-15:  TRise =      (regi_on - (4 * off_intensity)) * t_rise * (255/ClkX)
    // 16-31: TRise = 16 * (regIOn - (4 * off_intensity)) * t_rise * (255/ClkX)
    if (REG_T_RISE[pin] != 0xFF)
        writeByte(REG_T_RISE[pin], t_rise);
    // Write regTFall
    // 0: off
    // 1-15:  TFall =      (regIOn - (4 * off_intensity)) * t_fall * (255/ClkX)
    // 16-31: TFall = 16 * (regIOn - (4 * off_intensity)) * t_fall * (255/ClkX)
    if (REG_T_FALL[pin] != 0xFF)
        writeByte(REG_T_FALL[pin], t_fall);
}

void SX1509::keypad(byte rows, byte columns, unsigned int sleep_time, byte scan_time, byte debounce_time)
{
    unsigned int temp_word;
    byte temp_byte;

    // If clock hasn't been set up, set it to internal 2MHz
    if (_clkX == 0)
        clock(INTERNAL_CLOCK_2MHZ);

    // Set regDir 0:7 outputs, 8:15 inputs:
    temp_word = readWord(REG_DIR_B);
    for (int i = 0; i < rows; i++)
        temp_word &= ~(1 << i);
    for (int i = 8; i < (columns * 2); i++)
        temp_word |= (1 << i);
    writeWord(REG_DIR_B, temp_word);

    // Set regOpenDrain on 0:7:
    temp_byte = readByte(REG_OPEN_DRAIN_A);
    for (int i = 0; i < rows; i++)
        temp_byte |= (1 << i);
    writeByte(REG_OPEN_DRAIN_A, temp_byte);

    // Set regPullUp on 8:15:
    temp_byte = readByte(REG_PULL_UP_B);
    for (int i = 0; i < columns; i++)
        temp_byte |= (1 << i);
    writeByte(REG_PULL_UP_B, temp_byte);

    // Debounce Time must be less than scan time
    debounce_time = constrain(debounce_time, 1, 64);
    scan_time = constrain(scan_time, 1, 128);
    if (debounce_time >= scan_time)
    {
        debounce_time = scan_time >> 1; // Force debounce_time to be less than scan_time
    }
    debounceKeypad(debounce_time, rows, columns);

    // Calculate scan_time_bits, based on scanTime
    byte scan_time_bits = 0;
    for (byte i = 7; i > 0; i--)
    {
        if (scan_time & (1 << i))
        {
            scan_time_bits = i;
            break;
        }
    }

    // Calculate sleep_time_bits, based on sleep_time
    byte sleep_time_bits = 0;
    if (sleep_time != 0)
    {
        for (byte i = 7; i > 0; i--)
        {
            if (sleep_time & ((unsigned int)1 << (i + 6)))
            {
                sleep_time_bits = i;
                break;
            }
        }
        // If sleepTime was non-zero, but less than 128,
        // assume we wanted to turn sleep on, set it to minimum:
        if (sleep_time_bits == 0)
            sleep_time_bits = 1;
    }

    // RegKeyConfig1 sets the auto sleep time and scan time per row
    sleep_time_bits = (sleep_time_bits & 0b111) << 4;
    scan_time_bits &= 0b111; // Scan time is bits 2:0
    temp_byte = sleep_time | scan_time_bits;
    writeByte(REG_KEY_CONFIG_1, temp_byte);

    // RegKeyConfig2 tells the SX1509 how many rows and columns we've got going
    rows = (rows - 1) & 0b111;       // 0 = off, 0b001 = 2 rows, 0b111 = 8 rows, etc.
    columns = (columns - 1) & 0b111; // 0b000 = 1 column, ob111 = 8 columns, etc.
    writeByte(REG_KEY_CONFIG_2, (rows << 3) | columns);
}

unsigned int SX1509::readKeypad()
{
    return readKeyData();
}

unsigned int SX1509::readKeyData()
{
    return (0xFFFF ^ readWord(REG_KEY_DATA_1));
}

byte SX1509::getRow(unsigned int key_data)
{
    byte row_data = byte(key_data & 0x00FF);

    for (int i = 0; i < 8; i++)
    {
        if (row_data & (1 << i))
            return i;
    }
    return 0;
}

byte SX1509::getCol(unsigned int key_data)
{
    byte col_data = byte((key_data & 0xFF00) >> 8);

    for (int i = 0; i < 8; i++)
    {
        if (col_data & (1 << i))
            return i;
    }
    return 0;
}

void SX1509::sync(void)
{
    // First check if nReset functionality is set
    byte reg_misc = readByte(REG_MISC);
    if (!(reg_misc & 0x04))
    {
        reg_misc |= (1 << 2);
        writeByte(REG_MISC, reg_misc);
    }

    // Toggle nReset pin to sync LED timers
    pinMode(pin_reset, OUTPUT);    // set reset pin as output
    digitalWrite(pin_reset, LOW);  // pull reset pin low
    delay(1);                      // Wait for the pin to settle
    digitalWrite(pin_reset, HIGH); // pull reset pin back high

    // Return nReset to POR functionality
    writeByte(REG_MISC, (reg_misc & ~(1 << 2)));
}

void SX1509::debounceConfig(byte config_value)
{
    // First make sure clock is configured
    byte temp_byte = readByte(REG_MISC);
    if ((temp_byte & 0x70) == 0)
    {
        temp_byte |= (1 << 4); // Just default to no divider if not set
        writeByte(REG_MISC, temp_byte);
    }
    temp_byte = readByte(REG_CLOCK);
    if ((temp_byte & 0x60) == 0)
    {
        temp_byte |= (1 << 6); // default to internal osc.
        writeByte(REG_CLOCK, temp_byte);
    }

    config_value &= 0b111; // 3-bit value
    writeByte(REG_DEBOUNCE_CONFIG, config_value);
}

void SX1509::debounceTime(byte time)
{
    if (_clkX == 0)                    // If clock hasn't been set up.
        clock(INTERNAL_CLOCK_2MHZ, 1); // Set clock to 2MHz.

    // Debounce time-to-byte map: (assuming fOsc = 2MHz)
    // 0: 0.5ms		1: 1ms
    // 2: 2ms		3: 4ms
    // 4: 8ms		5: 16ms
    // 6: 32ms		7: 64ms
    // 2^(n-1)
    byte config_value = 0;
    // We'll check for the highest set bit position,
    // and use that for debounceConfig
    for (int i = 7; i >= 0; i--)
    {
        if (time & (1 << i))
        {
            config_value = i + 1;
            break;
        }
    }
    config_value = constrain(config_value, 0, 7);

    debounceConfig(config_value);
}

void SX1509::debounceEnable(byte pin)
{
    unsigned int config_value = readWord(REG_DEBOUNCE_ENABLE_B);
    config_value |= (1 << pin);
    writeWord(REG_DEBOUNCE_ENABLE_B, config_value);
}

void SX1509::debouncePin(byte pin)
{
    debounceEnable(pin);
}

void SX1509::debounceKeypad(byte time, byte num_rows, byte num_cols)
{
    // Set up debounce time:
    debounceTime(time);

    // Set up debounce pins:
    for (int i = 0; i < num_rows; i++)
        debouncePin(i);
    for (int i = 0; i < (8 + num_cols); i++)
        debouncePin(i);
}

void SX1509::enableInterrupt(byte pin, byte rise_fall)
{
    // Set REG_INTERRUPT_MASK
    unsigned int temp_word = readWord(REG_INTERRUPT_MASK_B);
    temp_word &= ~(1 << pin); // 0 = event on IO will trigger interrupt
    writeWord(REG_INTERRUPT_MASK_B, temp_word);

    byte sensitivity = 0;
    switch (rise_fall)
    {
    case CHANGE:
        sensitivity = 0b11;
        break;
    case FALLING:
        sensitivity = 0b10;
        break;
    case RISING:
        sensitivity = 0b01;
        break;
    }

    // Set REG_SENSE_XXX
    // Sensitivity is set as follows:
    // 00: None
    // 01: Rising
    // 10: Falling
    // 11: Both
    byte pin_mask = (pin & 0x07) * 2;
    byte sense_register;

    // Need to select between two words. One for bank A, one for B.
    if (pin >= 8)
        sense_register = REG_SENSE_HIGH_B;
    else
        sense_register = REG_SENSE_HIGH_A;

    temp_word = readWord(sense_register);
    temp_word &= ~(0b11 << pin_mask);       // Mask out the bits we want to write
    temp_word |= (sensitivity << pin_mask); // Add our new bits
    writeWord(sense_register, temp_word);
}

unsigned int SX1509::interruptSource(bool clear /* =true*/)
{
    unsigned int int_source = readWord(REG_INTERRUPT_SOURCE_B);
    if (clear)
        writeWord(REG_INTERRUPT_SOURCE_B, 0xFFFF); // Clear interrupts
    return int_source;
}

bool SX1509::checkInterrupt(int pin)
{
    if (interruptSource(false) & (1 << pin))
        return true;

    return false;
}

void SX1509::clock(byte osc_source, byte osc_divider, byte osc_pin_function, byte osc_freq_out)
{
    configClock(osc_source, osc_pin_function, osc_freq_out, osc_divider);
}

void SX1509::configClock(byte osc_source /*= 2*/, byte osc_pin_function /*= 0*/, byte osc_freq_out /*= 0*/, byte osc_divider /*= 1*/)
{
    // reg_clock constructed as follows:
    //	6:5 - Oscillator frequency souce
    //		00: off, 01: external input, 10: internal 2MHz, 1: reserved
    //	4 - OSCIO pin function
    //		0: input, 1 ouptut
    //	3:0 - Frequency of oscout pin
    //		0: LOW, 0xF: high, else fOSCOUT = FoSC/(2^(reg_clock[3:0]-1))
    osc_source = (osc_source & 0b11) << 5;          // 2-bit value, bits 6:5
    osc_pin_function = (osc_pin_function & 1) << 4; // 1-bit value bit 4
    osc_freq_out = (osc_freq_out & 0b1111);         // 4-bit value, bits 3:0
    byte reg_clock = osc_source | osc_pin_function | osc_freq_out;
    writeByte(REG_CLOCK, reg_clock);

    // Config reg_misc[6:4] with osc_divider
    // 0: off, else ClkX = fOSC / (2^(reg_misc[6:4] -1))
    osc_divider = constrain(osc_divider, 1, 7);
    _clkX = 2000000.0 / (1 << (osc_divider - 1)); // Update private clock variable
    osc_divider = (osc_divider & 0b111) << 4;     // 3-bit value, bits 6:4

    byte reg_misc = readByte(REG_MISC);
    reg_misc &= ~(0b111 << 4);
    reg_misc |= osc_divider;
    writeByte(REG_MISC, reg_misc);
}

byte SX1509::calculateLEDTRegister(int ms)
{
    int reg_on_1, reg_on_2;
    float time_on_1, time_on_2;

    if (_clkX == 0)
        return 0;

    reg_on_1 = (float)(ms / 1000.0) / (64.0 * 255.0 / (float)_clkX);
    reg_on_2 = reg_on_1 / 8;
    reg_on_1 = constrain(reg_on_1, 1, 15);
    reg_on_2 = constrain(reg_on_2, 16, 31);

    time_on_1 = 64.0 * reg_on_1 * 255.0 / _clkX * 1000.0;
    time_on_2 = 512.0 * reg_on_2 * 255.0 / _clkX * 1000.0;

    if (abs(time_on_1 - ms) < abs(time_on_2 - ms))
        return reg_on_1;
    else
        return reg_on_2;
}

byte SX1509::calculateSlopeRegister(int ms, byte on_intensity, byte off_intensity)
{
    int reg_slope_1, reg_slope_2;
    float reg_time_1, reg_time_2;

    if (_clkX == 0)
        return 0;

    float t_factor = ((float)on_intensity - (4.0 * (float)off_intensity)) * 255.0 / (float)_clkX;
    float time_s = float(ms) / 1000.0;

    reg_slope_1 = time_s / t_factor;
    reg_slope_2 = reg_slope_1 / 16;

    reg_slope_1 = constrain(reg_slope_1, 1, 15);
    reg_slope_2 = constrain(reg_slope_2, 16, 31);

    reg_time_1 = reg_slope_1 * t_factor * 1000.0;
    reg_time_2 = 16 * reg_time_1;

    if (abs(reg_time_1 - ms) < abs(reg_time_2 - ms))
        return reg_slope_1;
    else
        return reg_slope_2;
}

// readByte(byte register_address)
//	This function reads a single byte located at the register_address register.
//	- device_address should already be set by the constructor.
//	- Return value is the byte read from register_address
//		- Currently returns 0 if communication has timed out
byte SX1509::readByte(byte register_address)
{
    byte read_value;
    unsigned int timeout = RECEIVE_TIMEOUT_VALUE;

    this_i2c_port->beginTransmission(device_address);
    this_i2c_port->write(register_address);
    this_i2c_port->endTransmission();
    this_i2c_port->requestFrom(device_address, (byte)1);

    while ((this_i2c_port->available() < 1) && (timeout != 0))
        timeout--;

    if (timeout == 0)
        return 0;

    read_value = this_i2c_port->read();

    return read_value;
}

// readWord(byte register_address)
//	This function will read a two-byte word beginning at register_address
//	- A 16-bit unsigned int will be returned.
//		- The msb of the return value will contain the value read from register_address
//		- The lsb of the return value will contain the value read from register_address + 1
unsigned int SX1509::readWord(byte register_address)
{
    unsigned int read_value;
    unsigned int msb, lsb;
    unsigned int timeout = RECEIVE_TIMEOUT_VALUE * 2;

    this_i2c_port->beginTransmission(device_address);
    this_i2c_port->write(register_address);
    this_i2c_port->endTransmission();
    this_i2c_port->requestFrom(device_address, (byte)2);

    while ((this_i2c_port->available() < 2) && (timeout != 0))
        timeout--;

    if (timeout == 0)
        return 0;

    msb = (this_i2c_port->read() & 0x00FF) << 8;
    lsb = (this_i2c_port->read() & 0x00FF);
    read_value = msb | lsb;

    return read_value;
}

// readBytes(byte first_register_address, byte * destination, byte length)
//	This function reads a series of bytes incrementing from a given address
//	- firstRegsiterAddress is the first address to be read
//	- destination is an array of bytes where the read values will be stored into
//	- length is the number of bytes to be read
//	- No return value.
void SX1509::readBytes(byte first_register_address, byte *destination, byte length)
{
    this_i2c_port->beginTransmission(device_address);
    this_i2c_port->write(first_register_address);
    this_i2c_port->endTransmission();
    this_i2c_port->requestFrom(device_address, length);

    while (this_i2c_port->available() < length)
        ;

    for (int i = 0; i < length; i++)
    {
        destination[i] = this_i2c_port->read();
    }
}

// writeByte(byte register_address, byte write_value)
//	This function writes a single byte to a single register on the SX509.
//	- write_value is written to register_address
//	- deviceAddres should already be set from the constructor
//	- No return value.
void SX1509::writeByte(byte register_address, byte write_value)
{
    this_i2c_port->beginTransmission(device_address);
    this_i2c_port->write(register_address);
    this_i2c_port->write(write_value);
    this_i2c_port->endTransmission();
}

// writeWord(byte register_address, ungisnged int write_value)
//	This function writes a two-byte word to register_address and register_address + 1
//	- the upper byte of write_value is written to register_address
//		- the lower byte of write_value is written to register_address + 1
//	- No return value.
void SX1509::writeWord(byte register_address, unsigned int write_value)
{
    byte msb, lsb;
    msb = ((write_value & 0xFF00) >> 8);
    lsb = (write_value & 0x00FF);
    this_i2c_port->beginTransmission(device_address);
    this_i2c_port->write(register_address);
    this_i2c_port->write(msb);
    this_i2c_port->write(lsb);
    this_i2c_port->endTransmission();
}

// writeBytes(byte first_register_address, byte * writeArray, byte length)
//	This function writes an array of bytes, beggining at a specific adddress
//	- first_register_address is the initial register to be written.
//		- All writes following will be at incremental register addresses.
//	- writeArray should be an array of byte values to be written.
//	- length should be the number of bytes to be written.
//	- no return value.
void SX1509::writeBytes(byte first_register_address, byte *writeArray, byte length)
{
    this_i2c_port->beginTransmission(device_address);
    this_i2c_port->write(first_register_address);
    for (int i = 0; i < length; i++)
    {
        this_i2c_port->write(writeArray[i]);
    }
    this_i2c_port->endTransmission();
}