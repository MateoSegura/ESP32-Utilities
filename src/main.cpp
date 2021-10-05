/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: Bottle Bird
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: September 23, 2021
*/

#include <gas_bottle_alarm/app/app.h>

BottleBirdApp app;

void setup()
{
  app.begin();
}

void loop()
{
  esp.uart0.println(
      "App should never get here. Make sure to delete setup & loop using \"vTaskDelete(NULL)\" at the end of your app begin function.");
}

// End.