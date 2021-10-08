/*
* Company: ANZE Suspension
* File Name: main.cpp
* Project: Bottle Bird
* Version: 1.0
* Compartible Hardware: REV1.0
* Date Created: September 23, 2021
* Last Modified: September 23, 2021
*/

#include <Arduino.h>
#include <esp32_utilities.h>

#define VSPI_MOSI_PIN 23
#define VSPI_MISO_PIN 19
#define VSPI_CLCK_PIN 18

#define HSPI_MOSI_PIN 25
#define HSPI_MISO_PIN 17
#define HSPI_CLCK_PIN 16

#define CAN0_CONTROLLER_CS_PIN 27
#define CAN0_CONTROLLER_INT_PIN 35

SystemOnChip esp;
Terminal terminal;
MCP2518FD can_controller(CAN0_CONTROLLER_CS_PIN, esp.vspi, CAN0_CONTROLLER_INT_PIN);

void sendMessage();
void handleReceive();

void setup()
{
  esp.uart0.begin(115200);
  terminal.begin(&esp.uart0);

  esp.vspi.begin(VSPI_CLCK_PIN, VSPI_MISO_PIN, VSPI_MOSI_PIN); // Begin SPI bus

  MCP2518FDSettings settings(MCP2518FDSettings::OSC_40MHz, 1000 * 1000, DataBitRateFactor::x8); // CAN controller settings

  settings.mDriverReceiveFIFOSize = 200; // Setup CAN controller receiving queue size

  const uint32_t err = can_controller.begin(settings, []
                                            { can_controller.isr(); }); // TODO: Read exception about ESP32 interrupt

  if (err != 0)
  {
    terminal.printMessage(TerminalMessage("Could not initialize CAN controller. Error: " + String(err, HEX),
                                          "CAN", ERROR, micros()));
    while (1)
      ;
  }

  terminal.printMessage(TerminalMessage("CAN controller initialized",
                                        "CAN", ERROR, micros()));
}

void loop()
{
  sendMessage();
  handleReceive();
}

void sendMessage()
{
  CANFDMessage frame;
  frame.id = 100;
  frame.len = 32;
  for (uint8_t i = 0; i < frame.len; i++)
  {
    frame.data[i] = i * 2;
  }
  const bool ok = can_controller.tryToSend(frame);
  if (ok)
  {
    terminal.printMessage(TerminalMessage("Message sent succesfully",
                                          "CAN", ERROR, micros()));
  }
  else
  {
    terminal.printMessage(TerminalMessage("Could not send message",
                                          "CAN", ERROR, micros()));
  }
}

void handleReceive()
{
  CANFDMessage frame;
  while (can_controller.receive(frame))
  {

    //ID
    esp.uart0.print("[ID:0x");
    esp.uart0.print(frame.id, HEX);
    esp.uart0.print("] ");

    //EXT ID
    if (frame.ext)
    {
      esp.uart0.print("[EXT:");
      esp.uart0.print("YES] ");
    }
    else
    {
      esp.uart0.print("[EXT:");
      esp.uart0.print("NO] ");
    }

    //TYPE
    esp.uart0.print("[TYPE:");
    switch (frame.type)
    {
    case 0:
      esp.uart0.print("CAN REMOTE");
      break;
    case 1:
      esp.uart0.print("CAN DATA");
      break;
    case 2:
      esp.uart0.print("CANFD NO BRS");
      break;
    case 3:
      esp.uart0.print("CANFD BRS");
      break;
    default:
      break;
    }
    esp.uart0.print("] ");

    //DATA
    esp.uart0.print(" DATA =  ");
    for (uint8_t i = 0; i < frame.len; i++)
    {
      esp.uart0.print(frame.data[i]);
      esp.uart0.print(" ");
    }
    esp.uart0.println();
  }
}

// End.