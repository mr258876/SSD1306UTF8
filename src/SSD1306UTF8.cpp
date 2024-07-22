/**
 * Original Copyright (c) 2011-2023 Bill Greiman
 * 
 * Modified by mr258876 (c) 2024
 * 
 * This file is part of the Arduino SSD1306UTF8 Library
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
#include "SSD1306UTF8.h"
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::charWidth(uint8_t c) const {
  if (!m_font) {
    return 0;
  }
  uint8_t first = readFontByte(m_font + FONT_FIRST_CHAR);
  uint8_t count = readFontByte(m_font + FONT_CHAR_COUNT);
  if (c < first || c >= (first + count)) {
    return 0;
  }
  if (fontSize() > 3) {
    // Proportional font.
    return m_magFactor * readFontByte(m_font + FONT_WIDTH_TABLE + ((fontSize() > 1 && fontSize() < 5) ? FONT_UTF8_EXTRA_HEADER_LEN : 0) + c - first);
  }
  // Fixed width font.
  return m_magFactor * readFontByte(m_font + FONT_WIDTH);
}
const uint8_t SSD1306UTF8::charWidthUtf8() {
  if (!m_font) {
    return 0;
  }

  const uint8_t *font_utf8 = m_font + readFontBytes16(m_font + FONT_UTF8_HEADER_POS);
  uint16_t fontUtf8Settings = readFontBytes16(font_utf8);
  uint8_t fontUse24bitAddr = fontUtf8Settings & 0b1000;
  uint8_t fontUseExtensionPlanes =  fontUtf8Settings & 0b100;
  uint8_t fontUsePadding = fontUtf8Settings & 0b10;
  uint8_t fontIsFixedWidth = fontUtf8Settings & 0b1;
  uint8_t glyphMetaLen = (fontUseExtensionPlanes ? 3 : 2) + (fontIsFixedWidth ? 0 : 1) + (fontUse24bitAddr ? 3 : 2);
  uint16_t utf8Cnt = readFontBytes16(font_utf8 + FONT_UTF8_CHAR_COUNT);
  
  if (fontIsFixedWidth) {
    return readFontByte(font_utf8 + FONT_UTF8_GLYPH_WIDTH);
  }

  // phrase glyph to Unicode
  uint32_t g = utf8BufferToUnicode();
  
  // find the glyph
  for (size_t i = 0; i < utf8Cnt; i++)
  {
    uint32_t pg = fontUseExtensionPlanes ? readFontBytes24(font_utf8 + FONT_UTF8_GLYPH_TABLE + i * glyphMetaLen) : readFontBytes16(font_utf8 + FONT_UTF8_GLYPH_TABLE + i * glyphMetaLen);
    if (pg == g)
    {
      return readFontByte(font_utf8 + FONT_UTF8_GLYPH_TABLE + i * glyphMetaLen + (fontUseExtensionPlanes ? 3 : 2));
    }
  }
  return 0;
}
//------------------------------------------------------------------------------
void SSD1306UTF8::clear() {
#if INCLUDE_SCROLLING
  m_pageOffset = 0;
  setStartLine(0);
#endif  // INCLUDE_SCROLLING
  fill(0, displayWidth() - 1, 0, displayRows() - 1, 0);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::clear(uint8_t c0, uint8_t c1, uint8_t r0, uint8_t r1) {
  fill(c0, c1, r0, r1, 0);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::clearToEOL() {
  fill(m_col, displayWidth() - 1, m_row, m_row + fontRows() - 1, 0);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::clearField(uint8_t col, uint8_t row, uint8_t n) {
  fill(col, col + fieldWidth(n) - 1, row, row + fontRows() - 1, 0);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::displayRemap(bool mode) {
  ssd1306WriteCmd(mode ? SSD1306_SEGREMAP : SSD1306_SEGREMAP | 1);
  ssd1306WriteCmd(mode ? SSD1306_COMSCANINC : SSD1306_COMSCANDEC);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::fill() {
#if INCLUDE_SCROLLING
  m_pageOffset = 0;
  setStartLine(0);
#endif  // INCLUDE_SCROLLING
  fill(0, displayWidth() - 1, 0, displayRows() - 1, 0xFF);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::fill(uint8_t c0, uint8_t c1, uint8_t r0, uint8_t r1, uint8_t filler) {
  // Cancel skip character pixels.
  m_skip = 0;

  // Insure only rows on display will be filled.
  if (r1 >= displayRows()) r1 = displayRows() - 1;

  for (uint8_t r = r0; r <= r1; r++) {
    setCursor(c0, r);
    for (uint8_t c = c0; c <= c1; c++) {
      // Insure clear() writes zero. result is (filler^m_invertMask).
      ssd1306WriteRamBuf(filler);
    }
  }
  setCursor(c0, r0);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::fillToEOL() {
  fill(m_col, displayWidth() - 1, m_row, m_row + fontRows() - 1, 0xFF);
}
//------------------------------------------------------------------------------
size_t SSD1306UTF8::fieldWidth(uint8_t n) {
  return n * (fontWidth() + letterSpacing());
}
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::fontCharCount() const {
  return m_font ? readFontByte(m_font + FONT_CHAR_COUNT) : 0;
}
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::fontSupportsUtf8() const {
  if (m_font) return (readFontBytes16(m_font + FONT_LENGTH) > 1 && readFontBytes16(m_font + FONT_LENGTH) < 5) ? 1 : 0;
  return 0;
}
//------------------------------------------------------------------------------
uint16_t SSD1306UTF8::fontUtf8CharCount() const {
  if (m_font && fontSupportsUtf8()) return readFontBytes16(m_font + readFontBytes16(m_font + FONT_UTF8_HEADER_POS) + FONT_UTF8_CHAR_COUNT);
  return 0;
}
//------------------------------------------------------------------------------
char SSD1306UTF8::fontFirstChar() const {
  return m_font ? readFontByte(m_font + FONT_FIRST_CHAR) : 0;
}
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::fontHeight() const {
  return m_font ? m_magFactor * readFontByte(m_font + FONT_HEIGHT) : 0;
}
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::fontRows() const {
  return m_font ? m_magFactor * ((readFontByte(m_font + FONT_HEIGHT) + 7) / 8)
                : 0;
}
//------------------------------------------------------------------------------
uint16_t SSD1306UTF8::fontSize() const {
  return (readFontByte(m_font) << 8) | readFontByte(m_font + 1);
}
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::fontWidth() const {
  return m_font ? m_magFactor * readFontByte(m_font + FONT_WIDTH) : 0;
}
//------------------------------------------------------------------------------
void SSD1306UTF8::init(const DevType* dev) {
  m_col = 0;
  m_row = 0;
#ifdef __AVR__
  const uint8_t* table =
      reinterpret_cast<const uint8_t*>(pgm_read_word(&dev->initcmds));
#else   // __AVR__
  const uint8_t* table = dev->initcmds;
#endif  // __AVR
  uint8_t size = readFontByte(&dev->initSize);
  m_displayWidth = readFontByte(&dev->lcdWidth);
  m_displayHeight = readFontByte(&dev->lcdHeight);
  m_colOffset = readFontByte(&dev->colOffset);
  for (uint8_t i = 0; i < size; i++) {
    ssd1306WriteCmd(readFontByte(table + i));
  }
  clear();
}
//------------------------------------------------------------------------------
void SSD1306UTF8::invertDisplay(bool invert) {
  ssd1306WriteCmd(invert ? SSD1306_INVERTDISPLAY : SSD1306_NORMALDISPLAY);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::setCol(uint8_t col) {
  if (col < m_displayWidth) {
    m_col = col;
    col += m_colOffset;
    ssd1306WriteCmd(SSD1306_SETLOWCOLUMN | (col & 0XF));
    ssd1306WriteCmd(SSD1306_SETHIGHCOLUMN | (col >> 4));
  }
}
//------------------------------------------------------------------------------
void SSD1306UTF8::setContrast(uint8_t value) {
  ssd1306WriteCmd(SSD1306_SETCONTRAST);
  ssd1306WriteCmd(value);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::setCursor(uint8_t col, uint8_t row) {
  setCol(col);
  setRow(row);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::setFont(const uint8_t* font) {
  m_font = font;
  if (font && (fontSize() == 1 || fontSize() == 3)) {
    m_letterSpacing = 0;
  } else {
    m_letterSpacing = 1;
  }
}
//------------------------------------------------------------------------------
void SSD1306UTF8::setRow(uint8_t row) {
  if (row < displayRows()) {
    m_row = row;
#if INCLUDE_SCROLLING
    ssd1306WriteCmd(SSD1306_SETSTARTPAGE | ((m_row + m_pageOffset) & 7));
#else   // INCLUDE_SCROLLING
    ssd1306WriteCmd(SSD1306_SETSTARTPAGE | m_row);
#endif  // INCLUDE_SCROLLING
  }
}
#if INCLUDE_SCROLLING
//------------------------------------------------------------------------------
void SSD1306UTF8::setPageOffset(uint8_t page) {
  m_pageOffset = page & 7;
  setRow(m_row);
}
//------------------------------------------------------------------------------
void SSD1306UTF8::setStartLine(uint8_t line) {
  m_startLine = line & 0X3F;
  ssd1306WriteCmd(SSD1306_SETSTARTLINE | m_startLine);
}
#endif  // INCLUDE_SCROLLING
//------------------------------------------------------------------------------
void SSD1306UTF8::ssd1306WriteRam(uint8_t c) {
  if (m_col < m_displayWidth) {
    writeDisplay(c ^ m_invertMask, SSD1306_MODE_RAM);
    m_col++;
  }
}
//------------------------------------------------------------------------------
void SSD1306UTF8::ssd1306WriteRamBuf(uint8_t c) {
  if (m_skip) {
    m_skip--;
  } else if (m_col < m_displayWidth) {
    writeDisplay(c ^ m_invertMask, SSD1306_MODE_RAM_BUF);
    m_col++;
  }
}
//------------------------------------------------------------------------------
GLCDFONTDECL(scaledNibble) = {0X00, 0X03, 0X0C, 0X0F, 0X30, 0X33, 0X3C, 0X3F,
                              0XC0, 0XC3, 0XCC, 0XCF, 0XF0, 0XF3, 0XFC, 0XFF};
//------------------------------------------------------------------------------
const size_t SSD1306UTF8::strWidth(const char* str) {
  size_t sw = 0;
  while (*str) {
    if (*str > 127) {
      // char in utf8 
      if (readUtf8Data(*str) > 1) {
        uint8_t gw = charWidthUtf8();
        if (gw) sw += gw + utf8GlyphSpacing();
      }
    } else {
      uint8_t cw = charWidth(*str);
      if (cw) sw += cw + letterSpacing();
    }
    str++;
  }
  return sw;
}
//------------------------------------------------------------------------------
void SSD1306UTF8::tickerInit(TickerState* state, const uint8_t* font,
                              uint8_t row, bool mag2X, uint8_t bgnCol,
                              uint8_t endCol) {
  state->font = font;
  state->row = row;
  state->mag2X = mag2X;
  state->bgnCol = bgnCol;
  state->endCol = endCol < m_displayWidth ? endCol : m_displayWidth - 1;
  state->nQueue = 0;
}
//------------------------------------------------------------------------------
bool SSD1306UTF8::tickerText(TickerState* state, const char* text) {
  if (!text) {
    state->nQueue = 0;
    return true;
  }
  if (state->nQueue >= TICKER_QUEUE_DIM) {
    return false;
  }
  if (state->nQueue == 0) {
    state->init = true;
  }
  state->queue[state->nQueue++] = text;
  return true;
}
//------------------------------------------------------------------------------
int8_t SSD1306UTF8::tickerTick(TickerState* state) {
  if (!state->font) {
    return -1;
  }
  if (!state->nQueue) {
    return 0;
  }
  setFont(state->font);
  m_magFactor = state->mag2X ? 2 : 1;
  if (state->init) {
    clear(state->bgnCol, state->endCol, state->row,
          state->row + fontRows() - 1);
    state->col = state->endCol;
    state->skip = 0;
    state->init = false;
  }
  // Adjust display width to truncate pixels after endCol.  Find better way?
  uint8_t save = m_displayWidth;
  m_displayWidth = state->endCol + 1;

  // Skip pixels before bgnCol.
  skipColumns(state->skip);
  setCursor(state->col, state->row);
  for (uint8_t i = 0; i < state->nQueue; i++) {
    const char* str = state->queue[i];
    while (*str && m_col <= state->endCol) {
      write(*str++);
    }
    if (m_col > state->endCol) {
      break;
    }
  }
  if (m_col <= state->endCol) {
    clear(m_col, m_col, state->row, state->row + fontRows() - 1);
  }
  // Restore display width.
  m_displayWidth = save;

  if (state->nQueue == 1 && *state->queue[0] == 0) {
    state->nQueue = 0;
    return 0;
  }
  if (state->col > state->bgnCol) {
    state->col--;
  } else {
    state->skip++;
    if (state->skip >= charSpacing(*state->queue[0])) {
      state->skip = 0;
      state->queue[0]++;
      if (*state->queue[0] == 0 && state->nQueue > 1) {
        state->nQueue--;
        for (uint8_t i = 0; i < state->nQueue; i++) {
          state->queue[i] = state->queue[i + 1];
        }
      }
    }
  }
  return state->nQueue;
}
//------------------------------------------------------------------------------
uint32_t SSD1306UTF8::utf8BufferToUnicode() {
  uint32_t g = 0;
  uint8_t b = 0;
  if (m_utf8Buffer[0] >= 0xF0) {
    g = m_utf8Buffer[0] & 0b00000111;
    b = 4;
  } else if (m_utf8Buffer[0] >= 0xE0) {
    g = m_utf8Buffer[0] & 0b00001111;
    b = 3;
  } else if (m_utf8Buffer[0] >= 0xC0) {
    g = m_utf8Buffer[0] & 0b00011111;
    b = 2;
  }
  for (uint8_t i = 1; i < b; i++)
  {
    g = g << 6;
    g |= (m_utf8Buffer[i] & 0b00111111);
  }
  return g;
}
//------------------------------------------------------------------------------
const uint8_t SSD1306UTF8::utf8GlyphSpacing() {
  const uint8_t *font_utf8 = m_font + readFontBytes16(m_font + FONT_UTF8_HEADER_POS);
  uint16_t fontUtf8Settings = readFontBytes16(font_utf8);
  return fontUtf8Settings & 0b10;
}
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::writeUtf8Glyph() {
  //font data
  const uint8_t *font_utf8 = m_font + readFontBytes16(m_font + FONT_UTF8_HEADER_POS);
  uint16_t fontUtf8Settings = readFontBytes16(font_utf8);
  uint8_t fontUse24bitAddr = fontUtf8Settings & 0b1000;
  uint8_t fontUseExtensionPlanes = fontUtf8Settings & 0b100;
  uint8_t fontUsePadding = fontUtf8Settings & 0b10;
  uint8_t fontIsFixedWidth = fontUtf8Settings & 0b1;
  uint8_t glyphMetaLen = (fontUseExtensionPlanes ? 3 : 2) + (fontIsFixedWidth ? 0 : 1) + (fontUse24bitAddr ? 3 : 2);
  uint16_t utf8Cnt = readFontBytes16(font_utf8 + FONT_UTF8_CHAR_COUNT);

  // phrase glyph to Unicode
  uint32_t g = utf8BufferToUnicode();

  // find the glyph
  const uint8_t *p_glyph_md = font_utf8;
  uint8_t w = 0;
  uint8_t h = readFontByte(m_font + FONT_HEIGHT);
  uint8_t nr = (h + 7) / 8;
  for (size_t i = 0; i < utf8Cnt; i++)
  {
    uint32_t pg = fontUseExtensionPlanes ? readFontBytes24(font_utf8 + FONT_UTF8_GLYPH_TABLE + i * glyphMetaLen) : readFontBytes16(font_utf8 + FONT_UTF8_GLYPH_TABLE + i * glyphMetaLen);
    if (pg == g)
    {
      p_glyph_md += FONT_UTF8_GLYPH_TABLE + i * glyphMetaLen;
      w = fontIsFixedWidth ? readFontByte(font_utf8 + FONT_UTF8_GLYPH_WIDTH) : readFontByte(p_glyph_md + (fontUseExtensionPlanes ? 3 : 2));
      break;
    }
  }
  if (p_glyph_md == font_utf8) return 0;

  const uint8_t *p_glyph_d = m_font + (fontUse24bitAddr ? readFontBytes24(p_glyph_md + glyphMetaLen - 3) : readFontBytes16(p_glyph_md + glyphMetaLen - 2));

  uint8_t s = fontUsePadding ? 1 : 0;
  uint8_t thieleShift = 0;
  if (!fontIsFixedWidth)
  {
    if (h & 7) {
      thieleShift = 8 - (h & 7);
    }
  }
  
  uint8_t scol = m_col;
  uint8_t srow = m_row;
  uint8_t skip = m_skip;
  for (uint8_t r = 0; r < nr; r++) {
    for (uint8_t m = 0; m < m_magFactor; m++) {
      skipColumns(skip);
      if (r || m) {
        setCursor(scol, m_row + 1);
      }
      for (uint8_t c = 0; c < w; c++) {
        uint8_t b = readFontByte(p_glyph_d + c + r * w);
        if (thieleShift && (r + 1) == nr) {
          b >>= thieleShift;
        }
        if (m_magFactor == 2) {
          b = m ? b >> 4 : b & 0XF;
          b = readFontByte(scaledNibble + b);
          ssd1306WriteRamBuf(b);
        }
        ssd1306WriteRamBuf(b);
      }
      for (uint8_t i = 0; i < s; i++) {
        ssd1306WriteRamBuf(0);
      }
    }
  }
  setRow(srow);
  return 1;
}
//------------------------------------------------------------------------------
uint8_t SSD1306UTF8::readUtf8Data(uint8_t ch) {
  if (ch < 0xC0) {
    // not first byte
    if (m_utf8BufferedCount < 1 || m_utf8BufferedCount > 3) {
      // Invalid data
      m_utf8BufferedCount = 0;
      return 0;
    }
    
    m_utf8Buffer[m_utf8BufferedCount] = ch;
    m_utf8BufferedCount += 1;

    switch (m_utf8BufferedCount)
    {
    case 2:
      if (m_utf8Buffer[0] >= 0xE0)  return 1;
      // 2 bytes utf8, 0b110xxxxx 0b10xxxxxx
      m_utf8BufferedCount = 0;
      return 2;

    case 3:
      if (m_utf8Buffer[0] >= 0xF0)  return 1;
      // 3 bytes utf8, 0b1110xxxx 0b10xxxxxx 0b10xxxxxx
      m_utf8BufferedCount = 0;
      return 2;

    case 4:
      if (m_utf8Buffer[0] < 0xF0)  return 0;
      // 4 bytes utf8, 0b11110xxx 0b10xxxxxx 0b10xxxxxx 0b10xxxxxx
      m_utf8BufferedCount = 0;
      return 2;

    default:
      m_utf8BufferedCount = 0;
      return 0;
    }
  } else {
    // first byte
    m_utf8Buffer[0] = ch;
    m_utf8BufferedCount = 1;
    return 1;
  }
}
//------------------------------------------------------------------------------
size_t SSD1306UTF8::write(uint8_t ch) {
  if (!m_font) {
    return 0;
  }
  
  if (ch > 127) {
    // char in utf8 
    if (readUtf8Data(ch) > 1) {
      return writeUtf8Glyph();
    } else {
      return 1;
    }
  }
  
  uint8_t w = readFontByte(m_font + FONT_WIDTH);
  uint8_t h = readFontByte(m_font + FONT_HEIGHT);
  uint8_t nr = (h + 7) / 8;
  uint8_t first = readFontByte(m_font + FONT_FIRST_CHAR);
  uint8_t count = readFontByte(m_font + FONT_CHAR_COUNT);
  const uint8_t* base = m_font + FONT_WIDTH_TABLE + ((fontSize() > 1 && fontSize() < 5) ? FONT_UTF8_EXTRA_HEADER_LEN : 0);

  if (ch == '\r') {
    setCol(0);
    return 1;
  }
  if (ch == '\n') {
    setCol(0);
    uint8_t fr = m_magFactor * nr;
#if INCLUDE_SCROLLING
    uint8_t dr = displayRows();
    uint8_t tmpRow = m_row + fr;
    int8_t delta = tmpRow + fr - dr;
    if (m_scrollMode == SCROLL_MODE_OFF || delta <= 0) {
      setRow(tmpRow);
    } else {
      m_pageOffset = (m_pageOffset + delta) & 7;
      m_row = dr - fr;
      // Cursor will be positioned by clearToEOL.
      clearToEOL();
      if (m_scrollMode == SCROLL_MODE_AUTO) {
        setStartLine(8 * m_pageOffset);
      }
    }
#else   // INCLUDE_SCROLLING
    setRow(m_row + fr);
#endif  // INCLUDE_SCROLLING
    return 1;
  }
  bool nfSpace = false;
  if (first <= ch && ch < (first + count)) {
    ch -= first;
  } else if (ENABLE_NONFONT_SPACE && ch == ' ') {
    nfSpace = true;
  } else {
    // Error if not in font.
    return 0;
  }
  uint8_t s = letterSpacing();
  uint8_t thieleShift = 0;
  if (nfSpace) {
    // non-font space.
  } else if (fontSize() < 4) {
    // Fixed width font.
    base += nr * w * ch;
  } else {
    if (h & 7) {
      thieleShift = 8 - (h & 7);
    }
    uint16_t index = 0;
    for (uint8_t i = 0; i < ch; i++) {
      index += readFontByte(base + i);
    }
    w = readFontByte(base + ch);
    base += nr * index + count;
  }
  uint8_t scol = m_col;
  uint8_t srow = m_row;
  uint8_t skip = m_skip;
  for (uint8_t r = 0; r < nr; r++) {
    for (uint8_t m = 0; m < m_magFactor; m++) {
      skipColumns(skip);
      if (r || m) {
        setCursor(scol, m_row + 1);
      }
      for (uint8_t c = 0; c < w; c++) {
        uint8_t b = nfSpace ? 0 : readFontByte(base + c + r * w);
        if (thieleShift && (r + 1) == nr) {
          b >>= thieleShift;
        }
        if (m_magFactor == 2) {
          b = m ? b >> 4 : b & 0XF;
          b = readFontByte(scaledNibble + b);
          ssd1306WriteRamBuf(b);
        }
        ssd1306WriteRamBuf(b);
      }
      for (uint8_t i = 0; i < s; i++) {
        ssd1306WriteRamBuf(0);
      }
    }
  }
  setRow(srow);
  return 1;
}
