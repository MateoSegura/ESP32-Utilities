#include <Arduino.h>
#include <esp32_utilities.h>

#define TIME_STAMP_uS false

SystemOnChip SoC;
BluetoothLowEnergyServer myBLEserver;
Terminal myTerminal;
RealTimeClock myRTC;

ARD1867 myADC;
ADS1015 ads1015;

// * HSPI
#define ADC_CS_PIN 25
#define HSPI_SDI_PIN 23
#define HSPI_SDO_PIN 19
#define HSPI_SCK_PIN 18
#define HSPI_SCK_FREQUENCY 80000

void setup()
{
  // -- Begin UART port for debug output
  SoC.uart0.begin(115200);

  myTerminal.begin(&SoC.uart0, TIME_STAMP_uS);

  // Print App title
  myTerminal.println("\n\n");
  myTerminal.println("************************************************************************");
  myTerminal.println("*                        My App Name Goes here                         *"); // Replace with your title
  myTerminal.println("************************************************************************");
  myTerminal.println("\n\n");

  TerminalMessage myMessage = TerminalMessage("Testing ADC",
                                              "APP",
                                              INFO,
                                              micros());

  // -- Begin SPI bus for ADC
  SoC.hspi.begin(HSPI_SCK_PIN, HSPI_SDO_PIN, HSPI_SDI_PIN);
  SoC.hspi.setDataMode(1);
  SoC.hspi.setFrequency(HSPI_SCK_FREQUENCY);

  // -- Begin ADC
  byte err = myADC.begin(&SoC.hspi, DEVICE_LTC1867, ARD186X_EEP_DISABLE, ADC_CS_PIN);

  myTerminal.printMessage(TerminalMessage("ADC error code: " + String(err),
                                          "APP",
                                          INFO,
                                          micros()));

  myADC.setFastSPI(1);
  myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);

  myTerminal.printMessage(myMessage);
}
byte loopCounter = 0;

void speedProfile()
{
  unsigned int k = 0;
  unsigned long startTime, endTime;

  myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);

  myTerminal.print("Starting speed profile on channel 0\n");

  startTime = millis();
  for (unsigned long i = 0; i < 100000; i++)
  {
    k += myADC.ltc186xRead();
  }
  endTime = millis();

  myTerminal.print("Max data rate ");
  myTerminal.print(String(100000UL / (endTime - startTime)));
  myTerminal.print(" ksps\n");
}

byte confChan = 0;

void loop()
{
  // print the results to the serial monitor:

  myTerminal.print("Channel ");
  myTerminal.print(String(confChan));
  myTerminal.print(" Single Ended");
  myTerminal.print(" = [");
  myTerminal.print(String(myADC.ltc186xRead()));
  myTerminal.print("]\n");

  confChan++;
  if (confChan >= 8)
  {
    myTerminal.print("\n");
    confChan = 0;

    if (0 == loopCounter)
    {
      // Every 28 loops, run the speed profiler
      speedProfile();
    }
    loopCounter = (loopCounter + 1) % 4;
  }
  switch (confChan)
  {
  case 0:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_0P, 1);
    break;

  case 1:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_1P, 1);
    break;

  case 2:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_2P, 1);
    break;

  case 3:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_3P, 1);
    break;

  case 4:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_4P, 1);
    break;

  case 5:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_5P, 1);
    break;

  case 6:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_6P, 1);
    break;

  case 7:
    myADC.ltc186xChangeChannel(LTC186X_CHAN_SINGLE_7P, 1);
    break;
  }

  delay(500);
}

// End.