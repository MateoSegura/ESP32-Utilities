# Why should I use these libraries?

These libraries are the results of working with the ESP32 family of MCUs for 3+ years. These libraries are great for writting performant applications, fast. 

If you've written code using the Arduino framework, these libraries are here to help you take it to the **next level**. Although I love the ESP-IDF for the ESP32, it's only made for the ESP32 MCU, and I would like to use other chips for different applications, in the future. 

These libraries are very easy to port to other architectures like the iMXRT processor in the Teensy boards, and I have plans of doing so in the future.

# Getting Started

The *simple* examples folder has enumerated examples that cover each of the libraries in great detail. I recommend you start there & read all the comments & instructions. I've linked several resources where necessary, and I highly encourage you to read these articles before diving into using the libraries. 

1. Create a new [Platform IO](https://platformio.org) project in [Visual Studio Code](https://code.visualstudio.com), and select *DOIT ESP32 DEVKIT V1* as your board.
2. In your *platformio.ini* file, paste the following:

``` ini
lib_deps = 
	https://github.com/MateoSegura/ESP32-Utilities.git
	bblanchon/ArduinoJson@^6.18.5
```
3. Go to the folder *.pio/libdeps* and you should see the libraries folder in there. In the *examples/simple* folder, copy the code in the file *01_soc_example.cpp* to your *main.cpp* and compile the project.

The first time arround it will take a few seconds to compile because the libraries include the Bluetooth & WiFi headers so it'll take some time to compile these sources. 

## Hardware needed

Although these libraries were designed to be used with the [ESP32 System on Module](https://github.com/MateoSegura/ESP32-Internet-of-Things-SoM), there's absoluetly no reason as to why you can't use them as long as you have any of the following hardware:

- [ESP32 Development board](https://www.sparkfun.com/products/18018)
- [MCP2518FD CAN 2.0B/FD Controller](https://www.mikroe.com/mcp2518fd-click)
- [AD7689 16-bit ASR Analog to Digital (ADC) converter](https://www.mateosegura.com/blog/ad7689)
- [MPU9250 Internal Motion Unit (IMU)](https://www.amazon.com/HiLetgo-Gyroscope-Acceleration-Accelerator-Magnetometer/dp/B01I1J0Z7Y)
- [BME688 Environmental sensor](https://www.adafruit.com/product/5046?gclid=Cj0KCQiAkNiMBhCxARIsAIDDKNUuQkEDhbs2lozbv5vCGB4HdOvyR8xC1shPneeGXWJEfm6Sejvvg3AaAswrEALw_wcB)
- [RV-3028 Real Time Clock (RTC)](https://www.digikey.com/en/products/detail/pimoroni-ltd/PIM449/13537132)
- [uSD Card](https://www.sparkfun.com/products/12941)

All of these modules were used in the original prototype of the ESP32 SoM, and all the libraries have been tested to work with them. Make sure you follow the connections described in each example.

## Error Handling

These libraries end up being called by a higher abstraction layers, or in some cases many. In order to be able to pass any errors to higher abstraction layers, all functions that **can** return a known error, are of the type **ESP_ERROR**.

For example, let's say you call the following method to make a new directory without initializing the card:

``` C++
ESP_ERROR EMMC_Memory::makeDirectory(const char *path)
{
    ESP_ERROR err;
    String temp_message;

    if (emmc_initialized)
    {
        if (file_system->mkdir(path))
        {
            temp_message += "Directory \"";
            temp_message += path;
            temp_message += "\"";
            temp_message += " created succesfully";
        }
        else
        {
            err.on_error = true;
            temp_message += "Error creating directory";
        }
    }
    else
    {
        err.on_error = true;
        temp_message += "External storage is not inititalized";
    }

    err.debug_message = temp_message;
    return err;
}
```

On your application layer, this would look something like the following:

``` C++
EMMC_Memory emmc;

void loop(){
  
  ESP_ERROR make_directory = emmc.makeDirectory("/test");
  
  if(make_directory.on_error)
    abort(make_directory.debug_message); // Or any other debugging function call of your choice

}
```

And you will see the following output in your serial terminal

```
[            51 mS] [+   11 mS] [1] [ERR] [MMC] - External storage is not inititalized

Stopping program. Restart
```

Along with these messages, Espressif's debug output will also be sent to the terminal on **abort**. This will help you find bugs & problems in your apps quickly & efficiently.
