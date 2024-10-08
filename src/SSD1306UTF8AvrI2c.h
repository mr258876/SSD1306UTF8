/**
 * Copyright (c) 2011-2023 Bill Greiman
 * This file is part of the Arduino SSD1306Ascii Library
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
/**
 * @file SSD1306UTF8AvrI2c.h
 * @brief Class for I2C displays using AvrI2c.
 */
#ifndef SSD1306UTF8AvrI2c_h
#define SSD1306UTF8AvrI2c_h
#include "SSD1306UTF8.h"
#include "utility/AvrI2c.h"
/**
 * @class SSD1306UTF8AvrI2c
 * @brief Class for I2C displays on AVR.
 *
 * Uses the AvrI2c class that is smaller and faster than the
 * Wire library.
 */
class SSD1306UTF8AvrI2c : public SSD1306UTF8 {
 public:
  /**
   * @brief Initialize the display controller.
   *
   * @param[in] dev A device initialization structure.
   * @param[in] i2cAddr The I2C address of the display controller.
   */
  void begin(const DevType* dev, uint8_t i2cAddr) {
    m_nData = 0;
    m_i2cAddr = i2cAddr;

    m_i2c.begin(AVRI2C_FASTMODE);
    init(dev);
  }
  /**
   * @brief Initialize the display controller.
   *
   * @param[in] dev A device initialization structure.
   * @param[in] i2cAddr The I2C address of the display controller.
   * @param[in] rst The display controller reset pin.
   */
  void begin(const DevType* dev, uint8_t i2cAddr, uint8_t rst) {
    oledReset(rst);
    begin(dev, i2cAddr);
  }
  /**
   * @brief Set the I2C bit rate.
   *
   * @param[in] frequency Desired frequency in Hz.
   *            Valid range for a 16 MHz board is about 40 kHz to 444,000 kHz.
   */
  void setI2cClock(uint32_t frequency) { m_i2c.setClock(frequency); }

 protected:
  void writeDisplay(uint8_t b, uint8_t mode) {
    if ((m_nData && mode == SSD1306_MODE_CMD)) {
      m_i2c.stop();
      m_nData = 0;
    }
    if (m_nData == 0) {
      m_i2c.start((m_i2cAddr << 1) | I2C_WRITE);
      m_i2c.write(mode == SSD1306_MODE_CMD ? 0X00 : 0X40);
    }
    m_i2c.write(b);
    if (mode == SSD1306_MODE_RAM_BUF) {
      m_nData++;
    } else {
      m_i2c.stop();
      m_nData = 0;
    }
  }

 protected:
  AvrI2c m_i2c;
  uint8_t m_i2cAddr;
  uint8_t m_nData;
};
#define SSD1306AsciiAvrI2c SSD1306UTF8AvrI2c
#endif  // SSD1306UTF8AvrI2c_h
