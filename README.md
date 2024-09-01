# SSD1306UTF8

SSD1306UTF8 is an unbuffered character only library for small OLED displays like the Adafruit 1.3" and 0.96" Monochrome displays (SSD1306/SH1106).

This respository is an extended fork of the [SSD1306ASCII Library](https://github.com/greiman/SSD1306Ascii), since the original one will not provide UTF8 support.

## Introduction

SSD1306Ascii runs on Arduino AVR boards, Arduino Due and many other
Arduino style boards that have the SPI or Wire library.

Please read the html documentation in the SSD1306Ascii/doc folder.

Click on SSD1306Ascii.html in the doc folder.

Several options can be set by editing `src/SSD1306Ascii.h`.

The SSD1306Ascii library only requires a few bytes of RAM.

Here is Uno memory use for the SPI "Hello world!" example with
scrolling disabled:

Sketch uses 2928 bytes (9%) of program storage space. Maximum is 32256 bytes.

Global variables use 54 bytes (2%) of dynamic memory, leaving 1994 bytes for local variables. Maximum is 2048 bytes.

## About Fonts
The idea of this library is to minize both RAM and ROM usages, so no general utf8 font will be provided, probrbly. 

Hence, **You have to make your custom font to display UTF8 characters.** In order to do that, here is a brief explain about how a font is structured. 

The struct of font is seperated in two sections, defined as follows. You may reference `fonts/HelloWorldUTF8.h` for example.
### ASCII section
#### font_Type_Indicator
- `uint16_t`
- Indicates the type of the font, defined as follows:
  * 00 00 (ASCII only, fixed width font with 1 padding pixel on right and below)
  * 00 01 (ASCII only, fixed width font with no padding pixels) 
  * 00 02 (with utf-8 multi byte support, fixed width font with 1 padding pixel on right and below for ASCII chars) 
  * 00 03 (with utf-8 multi byte support, fixed width font with no padding pixels for ASCII chars) 
  * 00 04 (with utf-8 multi byte support, variable width font with for ASCII chars)
  * 00 05 (ASCII only, variable width font with)

#### font_Width_in_Pixel_for_fixed_drawing
- `uint8_t`
- Char width for ASCII characters, if `font_Type_Indicator` equals 0, 1, 2, or 3. DO NOT OBMIT, set a random value if not used.

#### font_Height_in_Pixel_for_all_characters
- `uint8_t`
- Char height for ASCII characters.

#### font_First_Char
- `uint8_t`
- The ascii value of the first char in ASCII table.

#### font_Ascii_Char_Count
- `uint8_t`
- How many ASCII characters in the font. The value of last char equals `font_First_Char + font_Char_Count - 1`.

#### font_Utf8_Section_Start;
- `uint16_t`
- Indicates the start position of utf8 section, exists only if `font_Type_Indicator` equals 2, 3 or 4.

#### font_Char_Widths
- `uint8_t[font_Last_Char - font_First_Char +1]`
- The width table of every ASCII characters. For each character the separate width in pixels, characters < 128 have an implicit virtual right empty row.

#### font_data
- `uint8_t[]`
-  Bit field of ASCII characters.
-  For each glyph, each byte represents 8 vertical mono pixels, with lower bits for pixels with smaller vertical coords. The bytes "scans" horizontally from left to right with 8 vertical pixels each time, until it comes to the glyph width. That is, the first `Glyph_Width - 1`th bytes represents the area from (0, 0) to (Glyph_Width - 1, 7); the area (0, 8) to (Glyph_Width - 1, 15) is represented by bytes from `Glyph_Width`th to `Glyph_Width * 2 - 1`th. The correponding mode is usually called as `Vertical - Horizon`, or `Column - Row` mode in the Font-LCD dot converters.

### UTF8 Section
#### font_Utf8_Indicator
- `uint16_t`
- Indicates properities of utf8 section, defined as follows:
  * bit0: Whether utf8 characters are fixed width (uesful for Chinese, Japanese & Korean)
  * bit1: Enable right and bottom padding
  * bit2: Enable extension planes (this will make glyph sizes in glyph table 3 bytes each instead of 2)
  * bit3: Enable 24bit glyph address (enable if your font is larger than 65535 bytes)
#### font_Utf8_Glyph_Width
- `uint8_t`
- Char width for utf8 characters, if utf8 fixed width is not enabled. DO NOT OBMIT, set a random value if not used.
#### font_Utf8_Glyph_Count
- `uint16_t`
- Count of utf8 glyphs in the font.
#### utf8_Glyph_Metadata
- `uint8_t[font_Utf8_Glyph_Count * Glyph_Metadata_Length]`
- `Glyph_Metadata_Length = (use_utf8_extension_plane ? 3 : 2) + (utf8_fixed_width ? 0 : 1) + (use_24bit_address ? 3 : 2)`
- Table of utf8 glyph metadata, includes Unicode, width and address for each glyph.

#### utf8_Glyph_Data
- `uint8_t[]`
- Bit data of utf8 glyphs. 
- same format with ASCII font data.
