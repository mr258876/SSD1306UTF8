// Simple I2C test for ebay 128x64 oled.

#include <Wire.h>
#include "SSD1306UTF8.h"
#include "SSD1306UTF8Wire.h"

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;
//------------------------------------------------------------------------------
void setup() {
  Wire.begin();
  Wire.setClock(400000L);

#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif // RST_PIN >= 0

  oled.setFont(Adafruit5x7);

  uint32_t m = micros();
  oled.clear();
  oled.println("Hello world UTF8");

  oled.setCursor(0, 2);
  oled.setFont(HelloWorldUTF8);
  oled.println("你好世界");
  oled.set2X();
  oled.println("你好世界");

  oled.setFont(Adafruit5x7);
  oled.set1X();
  oled.setCursor(0, 1);
  oled.print("\nmicros: ");
  oled.print(micros() - m);
}
//------------------------------------------------------------------------------
void loop() {}
