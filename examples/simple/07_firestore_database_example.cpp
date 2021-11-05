// #include <Arduino.h>
// #include <esp32_utilities.h>

// SystemOnChip esp;
// Terminal terminal;
// RealTimeClock rtc;
// DateTime system_time;

// static void IRAM_ATTR updateTime()
// {
//   system_time.updateMicroseconds();
// }

// void setup()
// {
//   esp.uart0.begin(115200);

//   // esp.timer0.setup();
//   // esp.timer0.attachInterrupt(updateTime);
//   // esp.timer0.timerPeriodMicroseconds(10); // in milliseconds
//   // esp.timer0.enableInterrupt();

//   pinMode(17, INPUT_PULLUP);
//   attachInterrupt(17, updateTime, FALLING);
// }

// void loop()
// {
//   esp.uart0.println(system_time.toString());
// }