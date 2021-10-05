#include <Arduino.h>
#include <esp32_utilities.h>

#define TIME_STAMP_uS false

SystemOnChip SoC;
BluetoothLowEnergyServer myBLEserver;
Terminal myTerminal;
RealTimeClock myRTC;

AD7689 ad7689;

// * HSPI
#define ADC_CS_PIN 4
#define HSPI_SDI_PIN 23
#define HSPI_SDO_PIN 19
#define HSPI_SCK_PIN 18
#define HSPI_SCK_FREQUENCY 20000

void sampleADC();

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

  // -- Enable ADC (P Channel MOSFET)

  // -- Begin SPI bus for ADC
  SoC.hspi.begin(HSPI_SCK_PIN, HSPI_SDO_PIN, HSPI_SDI_PIN);
  SoC.hspi.setFrequency(HSPI_SCK_FREQUENCY);

  // -- Begin ADC
  ESP_ERROR init_adc = ad7689.begin(ADC_CS_PIN, SoC.hspi, HSPI_SCK_FREQUENCY);

  if (init_adc.on_error)
  {
    myTerminal.println(init_adc.debug_message);
  }
  else
  {
    myTerminal.println("ADC initialized correctly");
  }
}

void loop()
{
  delay(10);
}
