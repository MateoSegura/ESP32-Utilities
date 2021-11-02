/*
 * File Name: terminal_example.cpp
 * Hardware needed: No extra hardware needed, just a good ol' ESP32 dev. board
 *
 * Copyright 2021, Mateo Segura, All rights reserved.
 */

#include <Arduino.h>
#include <esp32_utilities.h>

//******************** READ ME
//
// Simple example of how to use an object named "Terminal". Read on...
//
// Due to the limited I/O of the ESP32, app debugging over the serial port is necessary.
//
// If you've used Arduino's "Serial" object before, you have probably experienced some frustration
// when trying to print a lot of information to the terminal in an organized & maintainable manner.
//
// Not to mention the difficulty of limiting the terminal output to only errors, or completely
// turning it off for production releases.
//
// Similarly, in dual-core setups like the ESP32, one can use an RTOS task with low-priotiry to print messages
// from other tasks running on both cores, with accurate timing information about the processes in such tasks
//
// There's an RTOS version of this example under "examples/rtos/terminal_example_rtos.cpp"
//
// Mateo :)

//******************** SETTINGS

#define UART0_BAUD_RATE 115200         // Set the baudrate of the terminal uart port
#define MICROS_TIMESTAMP_ENABLED false // Set to true to enabled microsecond time stamp in terminal messages
#define SYSTEM_TIME_ENABLED false      // Set to true if using a real time clock. Refer to "rtc_example.cpp" for more

//******************** UTILITIES OBJECTS
SystemOnChip esp;
Terminal terminal;

//********************  SETUP
void setup()
{
    // 1. Init UART0 (Serial)
    esp.uart0.begin(UART0_BAUD_RATE);

    esp.uart0.println("\n\n"); // The esp.uart0 object is the "Serial" object from Arduino. Refer to "soc_example.cpp" for more
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("*                                              Terminal Example                                            *");
    esp.uart0.println("************************************************************************************************************");
    esp.uart0.println("\n\n");

    // 2. Init Terminal
    terminal.begin(esp.uart0, MICROS_TIMESTAMP_ENABLED, SYSTEM_TIME_ENABLED);
}

//********************  LOOP
void loop()
{
    // 1. The terminal can print a string in a new line or without adding a new line.
    //    this is the equivalent of "Serial.print(String message)" & "Serial.println(String message)"
    //    from Arduino

    terminal.print("\n\nHello there!"); // \n prints a new line
    terminal.println("\tHello back!");

    //    You can also cast any type to a String, and print it
    int printable_int = 5;
    double printable_double = 1.234;
    byte printable_byte = 0xA5;

    terminal.println(String(printable_int));
    terminal.println(String(printable_double, 3));
    terminal.println(String(printable_byte));

    // 2. The class "TerminalMessage" was create to work directly with the terminal.
    //
    //    This object helps create a message with the following information
    //
    //    - System time (Optiona, need's a real time clock ic)
    //    - Time since boot (the millis() Arduino counter)
    //    - Process time (example shown below. Very useful when using RTOS)
    //    - Process executing core (in dual core processors such as ESP)
    //    - Message type (INFO, WARNING, ERROR)
    //    - Message system (the task/function that is trying to output this debug message)
    //    - Message body
    //
    //    Example terminal output
    //
    //       * Time Stamp *         * Process Time *   * Core *  * Message Type *   * System *              * Debug Message *
    //
    //    [            51 mS]---------[+   11 mS]--------[1]----------[INF]------------[TER]------------- Terminal initialized
    //    [            59 mS]---------[+    0 mS]--------[1]----------[INF]------------[LED]------------- Debugging LED initialized
    //    [           117 mS]---------[+   58 mS]--------[1]----------[INF]------------[MEM]------------- eMMC Storage initialized
    //
    //

    // 3. Simulate dummy task 1 (equivalent to some piece hardware initializing)
    long initial_process_time = micros(); // Get the current time
    for (int i = 0; i < 10; i++)
        delay(10);

    // 4. Print terminal message
    TerminalMessage debug_message; // Create the message object

    debug_message.core = xPortGetCoreID();                        // Core of this process
    debug_message.body = "Info message, with process time";       // String message object
    debug_message.system = "TER";                                 // Limited to 3 characters
    debug_message.type = INFO;                                    // INFO, WARNING, ERROR
    debug_message.time = micros();                                // The time at which the event happened
    debug_message.process_time = micros() - initial_process_time; // Current time - initial time

    terminal.printMessage(debug_message); // Just pass the terminal message object to be printed

    // 5. Simulate dummy process 2 (equivalent to some wireless networking magic...)
    initial_process_time = micros(); // Get the current time
    for (int i = 0; i < 10; i++)
        delay(20);

    // 6. You can also print the terminal object as shown below. I find this an easier way to implement this functionality
    terminal.printMessage(TerminalMessage("Warning message, with process time",
                                          "TER",
                                          WARNING,
                                          micros(),
                                          micros() - initial_process_time)); // You don't always have to use the process time feature.
                                                                             // If uneeded, it's set to off by default

    terminal.printMessage(TerminalMessage("Error message, no process time",
                                          "TER",
                                          ERROR,
                                          micros())); // You don't always have to use the process time feature.
                                                      // If unused, filed will just print blank

    delay(2000);
}

//* End