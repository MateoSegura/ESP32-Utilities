#include <Arduino.h>
#include <esp32_utilities.h>

#define TIME_STAMP_uS false

Terminal myTerminal;
RealTimeClock myRTC;

void setup()
{
  // -- Begin UART port for debug output
  Serial.begin(115200);

  myTerminal.begin(&Serial, TIME_STAMP_uS);

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