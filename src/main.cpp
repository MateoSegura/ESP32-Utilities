#include <Arduino.h>
#include <esp32_utilities.h>

#define TIME_STAMP_uS false

SystemOnChip SoC;
BluetoothLowEnergyServer myBLEserver;
Terminal myTerminal;
RealTimeClock myRTC;

ADS1015 ads1015;

void setup()
{
  // -- Begin UART port for debug output
  SoC.uart0.begin(500000);

  myTerminal.begin(&SoC.uart0, TIME_STAMP_uS);

  // Print App title
  myTerminal.println("\n\n");
  myTerminal.println("************************************************************************");
  myTerminal.println("*                        My App Name Goes here                         *"); // Replace with your title
  myTerminal.println("************************************************************************");
  myTerminal.println("\n\n");

  TerminalMessage myMessage = TerminalMessage("Hello world",
                                              "APP",
                                              INFO,
                                              micros());

  myTerminal.printMessage(myMessage);
}

void loop()
{
  vTaskDelay(100);
  // put your main code here, to run repeatedly:
}
