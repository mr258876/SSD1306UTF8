/**
 * Original Copyright (c) 2011-2023 Bill Greiman
 * 
 * Modified by mr258876 (c) 2024
 * 
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
/*
 * allFonts.h font header for GLCD library
 * The fonts listed below will be available in a sketch if this file is included
 *
 * If you create your own fonts you can add the header to this file
 *
 * Note that the build environment only holds a font in Flash if its selected
 * so there is no penalty to including a font file here if its not used
 */
/**
 * @file allFonts.h
 * @brief Font definitions.
 */
#ifndef _allFonts_h_
#define _allFonts_h_

#ifdef __AVR__
#include <avr/pgmspace.h>
/** declare a font for AVR. */
#define GLCDFONTDECL(_n) static const uint8_t __attribute__((progmem)) _n[]
inline uint8_t readFontByte(uint8_t const *addr) { return pgm_read_byte(addr); }
inline uint16_t readFontBytes16(uint8_t const *addr) { return pgm_read_byte(addr) << 8 | pgm_read_byte(addr + 1); }
inline uint32_t readFontBytes24(uint8_t const *addr) { return pgm_read_byte(addr) << 16 | (pgm_read_byte(addr + 1) << 8) | pgm_read_byte(addr + 2); }
#else  // __AVR__
/** declare a font. */
#define GLCDFONTDECL(_n) static const uint8_t _n[]
/** Fake read from flash. */
inline uint8_t readFontByte(uint8_t const *addr) { return *addr; }
/** Fake read from flash. */
inline uint16_t readFontBytes16(uint8_t const *addr) { return (*addr) << 8 | *(addr + 1); }
/** Fake read from flash. */
inline uint32_t readFontBytes24(uint8_t const *addr) { return (*addr) << 16 | (*(addr + 1) << 8) | *(addr + 2); }
#endif  // __AVR__
//------------------------------------------------------------------------------
// Font Indices
/** No longer used Big Endian length field. Now indicates font type.
 *
 * 00 00 (fixed width font with 1 padding pixel on right and below)
 *
 * 00 01 (fixed width font with no padding pixels)
 * 
 * 00 02 (utf-8 multi byte support, fixed width font with 1 padding pixel on right and below)
 * 
 * 00 03 (utf-8 multi byte support, fixed width font with no padding pixels)
 * 
 * 00 04 (utf-8 multi byte support, variable width font with)
 * 
 */
#define FONT_LENGTH 0
/** Maximum character width. */
#define FONT_WIDTH 2
/** Font hight in pixels */
#define FONT_HEIGHT 3
/** Ascii value of first character */
#define FONT_FIRST_CHAR 4
/** count of characters in font. */
#define FONT_CHAR_COUNT 5
/** Offset to width table. */
#define FONT_WIDTH_TABLE 6
//
// FONT_LENGTH is a 16 bit Big Endian length field.
// Unfortunately, FontCreator2 screwed up the value it put in the field
// so it is pretty much meaningless. However it still is used to indicate
// some special things.
// 00 00 (fixed width font with 1 padding pixel on right and below)
// 00 01 (fixed width font with no padding pixels)
// FONT_WIDTH is the max character width.
// 00 02 (utf-8 multi byte support, fixed width font with 1 padding pixel on right and below) 
// 00 03 (utf-8 multi byte support, fixed width font with no padding pixels)
// FONT_WIDTH is the max character width for ASCII characters (val < 128)
// any other value means variable width font in FontCreator2 (thiele)
// format with pixel padding

#define FONT_UTF8_EXTRA_HEADER_LEN  2
// Extra length in original header of utf8 fonts
#define FONT_UTF8_HEADER_POS    6
// Big-endian uint16_t, points to the start pos of utf-8 section
#define FONT_UTF8_INDICATOR     0
/**
 * Indicator of utf8 section.
 *  bit0: Is fixed width
 *  bit1: Enable right and bottom padding
 *  bit2: Enable extension planes (this will make glyphs in glyph table 3 bytes each instead of 2)
 *  bit3: Enable 24bit address for glyph data
 */
#define FONT_UTF8_GLYPH_WIDTH   2
/* Glyph width when fixed width*/
#define FONT_UTF8_CHAR_COUNT    3
/* Offset to glyph table */
#define FONT_UTF8_GLYPH_TABLE   5

#include "Adafruit5x7.h"    // Font from Adafruit GFX library
#include "Arial14.h"        // proportional font
#include "Arial_bold_14.h"  // Bold proportional font
#include "CalBlk36.h"
#include "CalLite24.h"
#include "Callibri10.h"
#include "Callibri11.h"
#include "Callibri11_bold.h"
#include "Callibri11_italic.h"
#include "Callibri14.h"
#include "Callibri15.h"
#include "Cooper19.h"
#include "Cooper21.h"
#include "Cooper26.h"
#include "Corsiva_12.h"
#include "Iain5x7.h"  // similar to system5x7 but proportional
#include "Roosewood22.h"
#include "Roosewood26.h"
#include "Stang5x7.h"
#include "System5x7.h"      // system font (fixed width)
#include "SystemFont5x7.h"  // backward compatibility System5x7 header
#include "TimesNewRoman13.h"
#include "TimesNewRoman13_italic.h"
#include "TimesNewRoman16.h"
#include "TimesNewRoman16_bold.h"
#include "TimesNewRoman16_italic.h"
#include "Verdana12.h"
#include "Verdana12_bold.h"
#include "Verdana12_italic.h"
#include "Verdana_digits_24.h"  // large proportional font contains [0-9] and :
#include "Wendy3x5.h"
#include "X11fixed7x14.h"
#include "X11fixed7x14B.h"
#include "ZevvPeep8x16.h"
#include "fixed_bold10x15.h"
#include "fixednums15x31.h"  // fixed width font - + , - . / [0-9] and :
#include "fixednums7x15.h"   // fixed width font - + , - . / [0-9] and :
#include "fixednums8x16.h"   // fixed width font - + , - . / [0-9] and :
#include "font5x7.h"
#include "lcd5x7.h"
#include "lcdnums12x16.h"  // font that looks like LCD digits
#include "lcdnums14x24.h"  // font that looks like LCD digits
#include "newbasic3x5.h"
#include "HelloWorldUTF8.h"

/*
 * These fonts require no-pad rendering code
 */
#include "cp437font8x8.h"  // fixed Font from 80's IBM PC
#include "font8x8.h"       // fixed wider font but similar to system5x7 font

#endif
