// Test for minimum program size.

#include <SPI.h>
#include "SSD1306UTF8.h"
#include "SSD1306UTF8Spi.h"

// pin definitions
#define CS_PIN  7
#define RST_PIN 8
#define DC_PIN  9

SSD1306AsciiSpi oled;
//------------------------------------------------------------------------------
void setup() {
  // Use next line if no RST_PIN or reset is not required.
  // oled.begin(&Adafruit128x64, CS_PIN, DC_PIN);  
  oled.begin(&Adafruit128x64, CS_PIN, DC_PIN, RST_PIN);
  oled.setFont(System5x7);
  oled.clear();
  oled.print("Hello world!");
}
//------------------------------------------------------------------------------
void loop() {}