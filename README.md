# ESP32-Utilities
A collection of libraries used to create IoT embedded applications with the ESP32 family of microcontrollers and the Arduino framework.

A list of the available libraries is given below:

- **Terminal:** This library is helpful for developers interested using RTOS, as it provides an easy an easy way of knowing the time since boot, the core in which the code as running, as well as provide different levels of debugging. This was based on Espressif's own libraries, but with a couple of added features like real-time clock time stamping, & process time measurement.

```C++
#include <esp32_utilities.h>

Terminal terminal; 

void setup(){
    // Begin the terminal. You can use it in UART0 or any UART of your choice
    Serial.begin(115200);
    terminal.begin(&Serial);
}

void loop(){}
```
