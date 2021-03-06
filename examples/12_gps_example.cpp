/*
  By: Paul Clark
  SparkFun Electronics
  Date: December, 2020
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example configures the High Navigation Rate on the NEO-M8U and then
  reads and displays the attitude solution, vehicle dynamics information
  and high rate position, velocity and time.

  This example uses "autoHNR" to receive the HNR data automatically.

  Please make sure your NEO-M8U is running UDR firmware >= 1.31. Please update using u-center if necessary:
  https://www.u-blox.com/en/product/neo-m8u-module#tab-documentation-resources

  Feel like supporting open source hardware?
  Buy a board from SparkFun!
  NEO-M8U: https://www.sparkfun.com/products/16329

  Hardware Connections:
  Plug a Qwiic cable into the GPS and a Redboard Qwiic
  If you don't have a platform with a Qwiic connection use the
  SparkFun Qwiic Breadboard Jumper (https://www.sparkfun.com/products/14425)
  Open the serial monitor at 115200 baud to see the output

*/
#include <Arduino.h>
#include <Wire.h> //Needed for I2C to GPS

#include <SparkFun_Ublox_Arduino_Library.h> //http://librarymanager/All#SparkFun_u-blox_GNSS
SFE_UBLOX_GPS myGPS;

boolean usingAutoHNRAtt = false;
boolean usingAutoHNRDyn = false;
boolean usingAutoHNRPVT = false;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // Wait for user to open terminal
  Serial.println(F("SparkFun u-blox Example"));

  Wire.begin();

  // myGPS.enableDebugging(); // Uncomment this line to enable debug messages on Serial

  if (myGPS.begin() == false) // Connect to the Ublox module using Wire port
  {
    Serial.println(F("Warning! u-blox GPS did not begin correctly."));
    Serial.println(F("(This may be because the I2C port is busy with HNR messages.)"));
  }

  myGPS.setI2COutput(COM_TYPE_UBX);                 // Set the I2C port to output UBX only (turn off NMEA noise)
  myGPS.saveConfigSelective(VAL_CFG_SUBSEC_IOPORT); // Save (only) the communications port settings to flash and BBR

  if (myGPS.setHNRNavigationRate(10) == true) // Set the High Navigation Rate to 10Hz
    Serial.println(F("setHNRNavigationRate was successful"));
  else
    Serial.println(F("setHNRNavigationRate was NOT successful"));

  usingAutoHNRAtt = myGPS.setAutoHNRAtt(true); // Attempt to enable auto HNR attitude messages
  usingAutoHNRDyn = myGPS.setAutoHNRDyn(true); // Attempt to enable auto HNR vehicle dynamics messages
  usingAutoHNRPVT = myGPS.setAutoHNRPVT(true); // Attempt to enable auto HNR PVT messages
}

void loop()
{
  if (usingAutoHNRAtt && (myGPS.getHNRAtt() == true)) // If setAutoHNRAtt was successful and new data is available
  {
    Serial.print(F("Roll: ")); // Print selected data
    Serial.print(myGPS.hnrAtt.roll);
    Serial.print(F(" Pitch: "));
    Serial.print(myGPS.hnrAtt.pitch);
    Serial.print(F(" Heading: "));
    Serial.println(myGPS.hnrAtt.heading);
  }
  if (usingAutoHNRDyn && (myGPS.getHNRDyn() == true)) // If setAutoHNRDyn was successful and new data is available
  {
    Serial.print(F("xAccel: ")); // Print selected data
    Serial.print(myGPS.hnrVehDyn.xAccel);
    Serial.print(F(" yAccel: "));
    Serial.print(myGPS.hnrVehDyn.yAccel);
    Serial.print(F(" zAccel: "));
    Serial.println(myGPS.hnrVehDyn.zAccel);
  }
  if (usingAutoHNRPVT && (myGPS.getHNRPVT() == true)) // If setAutoHNRPVT was successful and new data is available
  {
    Serial.print(F("ns: ")); // Print selected data
    Serial.print(myGPS.hnrPVT.nano);
    Serial.print(F(" Lat: "));
    Serial.print(myGPS.hnrPVT.lat);
    Serial.print(F(" Lon: "));
    Serial.println(myGPS.hnrPVT.lon);
  }
}
