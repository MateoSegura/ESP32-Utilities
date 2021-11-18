# Why should I use these libraries?

These libraries are the results of working with the ESP32 family of MCUs for 3+ years. These libraries are great for writting performant applications, fast. 

If you've wrote code using the Arduino framework, these libraries are here to help you take it to the next step. Although I love the ESP-IDF for the ESP32, it's only made for the ESP32 MCU, and I would like to use other chips for different applications.

# Getting Started

The **simple** examples folder has enumerated examples that cover each of the libraries in great detail. I recommend you start there & read all the comments & instructions. I've linked several resources where necessary, and I highly encourage you to read these articles before diving into using the libraries. 

## Hardware needed

- ESP32 Development board
- MCP2518FD CAN 2.0B/FD Controller
- AD7689 16-bit ASR Analog to Digital (ADC) converter
- MPU9250 Internal Motion Unit (IMU)
- BME688 Environmental sensor
- RV-3028 Real Time Clock (RTC)
- uSD Card

## Error Handling

These libraries end up being called by a higher abstraction layers, or in some cases many. In order to be able to pass any errors to higher abstraction layers, all functions that **can** return a known error, are of the type **ESP_ERROR**, en example is given below:

# Why should I use these libraries?

These libraries are the results of working with the ESP32 family of MCUs for 3+ years. These libraries are great for writting performant applications, fast. 

If you've wrote code using the Arduino framework, these libraries are here to help you take it to the next step. Although I love the ESP-IDF for the ESP32, it's only made for the ESP32 MCU, and I would like to use other chips for different applications.

# Getting Started

The **simple** examples folder has enumerated examples that cover each of the libraries in great detail. I recommend you start there & read all the comments & instructions. I've linked several resources where necessary, and I highly encourage you to read these articles before diving into using the libraries. 

## Hardware needed

- ESP32 Development board
- MCP2518FD CAN 2.0B/FD Controller
- AD7689 16-bit ASR Analog to Digital (ADC) converter
- MPU9250 Internal Motion Unit (IMU)
- BME688 Environmental sensor
- RV-3028 Real Time Clock (RTC)
- uSD Card

## Error Handling

These libraries end up being called by a higher abstraction layers, or in some cases many. In order to be able to pass any errors to higher abstraction layers, all functions that **can** return a known error, are of the type **ESP_ERROR**, en example is given below:



This hardware is combined in a System on Module (SoM) with a small footprint, in a 4-layer board, and all signals are routed out through high density connectors.

The reason for this board was to create the bases of a eco-system on electronic control units for a variety of products in the automotive/industrial sector. This allows me to re-use a big percentage of the software across all of these projects.

``` C++
void setup(){

}

void loop(){


}
```

This hardware is combined in a System on Module (SoM) with a small footprint, in a 4-layer board, and all signals are routed out through high density connectors.

The reason for this board was to create the bases of a eco-system on electronic control units for a variety of products in the automotive/industrial sector. This allows me to re-use a big percentage of the software across all of these projects.
