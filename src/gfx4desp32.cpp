#include "gfx4desp32.h"

#define swap(a, b)                                                              \
    {                                                                           \
        int16_t tab = a;                                                        \
        a = b;                                                                  \
        b = tab;                                                                \
    }

#define calcAlpha(a, b, c)                                                     \
    {                                                                          \
        __alphatemp = c >> 3;                                                  \
        uint32_t fgu = a;                                                      \
        uint32_t bgu = b;                                                      \
        uint32_t fg = (fgu | (fgu << 16)) & 0x07e0f81f;                        \
        uint32_t bg = (bgu | (bgu << 16)) & 0x07e0f81f;                        \
        bg += (fg - bg) * __alphatemp >> 5; bg &= 0x07e0f81f;                  \
        __colour = (uint16_t)(bg | bg >> 16);                                  \
    }

gfx4desp32::gfx4desp32() {}

gfx4desp32::~gfx4desp32() {}

#ifndef USE_LITTLEFS_FILE_SYSTEM
#ifndef USE_SDMMC_FILE_SYSTEM
SdFat gfx4desp32::uSD;

SdFat& gfx4desp32::getSdFatInstance() { return uSD; }
#else
#include "SD_MMC.h"   
#endif
#endif

/****************************************************************************/
/*!
  @brief  Begin gfx4desp32.
  @param  ips - sets if ips display is being used IPS_DISPLAY or TN_DISPLAY if not ips
                Default is TN_DISPLAY
          pval - sets the value of display clock in Mhz eg 13 = 13000000
          backLightStartOn, true or false. sets startup behaviour of backlight.
  @note   sets global variables, orientation and clears the screen.
          uSD is mounted at this stage and called seperately.
              begin(); start with default settings
              begin(IPS_DISPLAY); // use ips display
              begin(18); // set display clock to 18Mhz
              begin(false); // start without backlight on
              begin(IPS_DISPLAY, 18); // start with ips at 18Mhz
              begin(IPS_DISPLAY, true); // start with ips and backlight on
              begin(20, false); // start at 20Mhz and backlight off
              begin(IPS_DISPLAY, 40, false); // start with ips at 40Mhz and backlight off
*/
/****************************************************************************/
void gfx4desp32::begin(String ips) { begin(ips, 0, true); }
void gfx4desp32::begin(int pval) { begin(DEFAULT_DISPLAY, pval, true); }
void gfx4desp32::begin(bool backLightStartOn) { begin(DEFAULT_DISPLAY, 0, backLightStartOn); }
void gfx4desp32::begin(String ips, int pval) { begin(ips, pval, true); }
void gfx4desp32::begin(String ips, bool backLightStartOn) { begin(ips, 0, backLightStartOn); }
void gfx4desp32::begin(int pval, bool backLightStartOn) { begin(DEFAULT_DISPLAY, pval, backLightStartOn); }

/****************************************************************************/
/*!
  @brief  Begin gfx4desp32.
  @note   sets global variables, orientation and clears the screen.
          uSD is mounted at this stage and called seperately.
*/
/****************************************************************************/
void gfx4desp32::begin(String ips, int pval, bool backLightStartOn) {
    if (pval >= 13) {
        changePCLK = true;
        PCLKval = pval * 1000 * 1000;
    }
    if (ips == IPS_DISPLAY)
        IPS_Display = true;
    if (ips == TN_DISPLAY)
        IPS_Display = false;
    if (backLightStartOn) bkStartOn = true;
    GCItype = GCI_SYSTEM_USD;
    __begin();            // start panel
    panelOrientation(0);  // set default orientation
    width = getWidth();   // retrieve Width in pixels from panel
    height = getHeight(); // retrieve Height in pixels from panel
    screenArea = (width * height) << 1;
    scrollAreaY0 = 0; // set initial scroll area to maximum height - used for auto
    // scroll if enabled
    scrollAreaY1 = height - 1;
    _nlh = height; // set newline cut-off point for autoscroll to maximum height
    Cls();         // clear the screen
    rotation = 0;  // set rotation variable to match initial orientation
    cursor_y = cursor_x = 0; // set text cursor to 0, 0
    textsize = 1;            // set text multiplier to 1
    textcolor = 0xFFFF;      // set default text colour to white
    textbgcolor = 0x0000;    // set default text background to black
    wrap = true;             // turn on wrap
    fno = 1;                 // set system font 1
    fsh = 8;                 // set font height
    lastfsh = 8; // match last font height - used by autoscroll if font changes
    // and scroll is needed
    fsw = 5;     // set font width
    scrolled = false; // legacy variable used for hardware scroll
    sEnable = false;  // disable auto scroll
    nl = false;       // set newline needed flag to false
    ssSpeed = 0; // set default smooth scroll to 5 - maybe we should default to 0,
    // smooth scroll off
    twcolnum = 13; // set default text window column number
    tchen = true;
    twcurson = true; // set default text window cursor on

#ifdef USE_LITTLEFS_FILE_SYSTEM // resolve File system method
#ifdef LITTLEFS_FORMAT_ON_FAIL
    if (LittleFS.begin(true))         // init flash file system
#else
    if (LittleFS.begin())         // init flash file system
#endif
    {
        sdok = true;
    }
    else {
        sdok = false;
    }
#else
#ifdef USE_SDMMC_FILE_SYSTEM
#ifndef SDMMC_4BIT
    pinMode(sd_cs, OUTPUT);
    digitalWrite(sd_cs, HIGH);
    SD_MMC.setPins(sd_sck, sd_mosi, sd_miso);
    if (SD_MMC.begin("/sdcard", true, 50000))
#else
    if (displayBus == DISPLAY_BUS_SPI) SD_MMC.setPins(SPI_SDMMC_4BIT_CLK, SPI_SDMMC_4BIT_CMD, SPI_SDMMC_4BIT_DATA0, SPI_SDMMC_4BIT_DATA1, SPI_SDMMC_4BIT_DATA2, SPI_SDMMC_4BIT_DATA3);
    if (displayBus == DISPLAY_BUS_QSPI) SD_MMC.setPins(QSPI_SDMMC_4BIT_CLK, QSPI_SDMMC_4BIT_CMD, QSPI_SDMMC_4BIT_DATA0, QSPI_SDMMC_4BIT_DATA1, QSPI_SDMMC_4BIT_DATA2, QSPI_SDMMC_4BIT_DATA3);
    if (SD_MMC.begin("/sdcard", false, 50000))
#endif
    {
        sdok |= true;
    }
    else {
        sdok |= false;
    }
#else
    SPI.begin(sd_sck, sd_miso, sd_mosi);

    if (uSD.begin(sd_cs, SD_SCK_MHZ(40))) // initialize SdFat file system
    {
        sdok |= true;
    }
    else {
        sdok |= false;
    }
#endif
#endif
    AllocateFB(0);
    //psRAMbuffer1 = (uint8_t*)ps_malloc(1024000);
    if (bkStartOn) {
        Contrast(BK_LIGHT_STARTUP_LEVEL);
    }
}

/****************************************************************************/
/*!
  @brief  Write a character to the display using chosen font.
  @param  c - character to be written.
  @note works in conjunction with newLine function and Scroll if enabled
*/
/****************************************************************************/
size_t gfx4desp32::write(uint8_t c) {
    if (nl)
        newLine(lastfsh, lastsizeht, 0);
    if (c == 10) {
        nl = true;
        lastfsh = fsh;
        lastfsw = fsw;
        lastsizeht = textsizeht;
    }
    if (c == 13) {
        cursor_x = 0;
    }
    if ((cursor_y + (fsh * textsizeht)) > getScrollareaY1() && sEnable) {
        int tempScroll = (cursor_y + (fsh * textsizeht)) - getScrollareaY1();
        newLine(tempScroll, 1, 0);
        cursor_y -= tempScroll;
    }

    if (c != 10 && c != 13) {
        if (fno == 0 || fno == -1) {
            uint16_t u16chr;

            // First we build the Utf8 character
            if (utf8expLen) {
                // If we already started building the utf-8, we continue
                // we can check this by checking expected length
                if ((c & 0xC0) != 0x80) {
                    // Invalid UTF-8 sequence, handle error or ignore
                    utf8expLen = 0;
                    utf8codepoint = 0;
                    return 0; // Indicate failure
                }
                utf8codepoint = (utf8codepoint << 6) | (c & 0x3F);
                utf8expLen--;
                if (utf8expLen != 0) return 0; // not yet complete
                u16chr = static_cast<uint16_t>(utf8codepoint);
            }
            else {
                // Otherwise, let's figure out how many bytes to expect
                if ((c & 0x80) == 0) {
                    // If the character is ASCII, directly write its Unicode value
                    u16chr = static_cast<uint16_t>(c);
                }
                else if ((c & 0xE0) == 0xC0) {
                    utf8codepoint = c & 0x1F;
                    utf8expLen = 1;
                    return 0;
                }
                else if ((c & 0xF0) == 0xE0) {
                    utf8codepoint = c & 0x0F;
                    utf8expLen = 2;
                    return 0;
                }
                else if ((c & 0xF8) == 0xF0) {
                    utf8codepoint = c & 0x07;
                    utf8expLen = 3;
                    return 0;
                }
                else {
                    // Invalid UTF-8 sequence, handle error or ignore
                    return 0; // Indicate failure
                }
            }

            if (wrap && (cursor_x > (width - textsize * (__gciCharWidth(u16chr) + 1)))) {
                newLine(fsh, textsizeht, 0);
            }

            // draw character here
            if ((fno == -1) && fntCmprs) {
                drawChar4Dcmp(cursor_x, cursor_y, u16chr, textcolor, textbgcolor, textsize,
                    textsizeht);
            }
            else {
                drawChar4D(cursor_x, cursor_y, u16chr, textcolor, textbgcolor, textsize,
                    textsizeht);
            }
        }
        else if (c > 31 && c < 128) {

            if (wrap && (cursor_x > (width - textsize * (charWidth(c) + 1)))) {
                newLine(fsh, textsizeht, 0);
            }

            if (fno == 1) {
                drawChar1(cursor_x, cursor_y, c - 32, textcolor, textbgcolor, textsize,
                    textsizeht);
            }
            if (fno == 2) {
                drawChar2(cursor_x, cursor_y, c - 32, textcolor, textbgcolor, textsize,
                    textsizeht);
            }

            cursor_x += textsize * (fsw + 1);
        }

        // if (wrap && (cursor_x > (width - textsize * (fsw + 1)))) {
        //     newLine(fsh, textsizeht, 0);
        // }
    }
    return 1;
}

/****************************************************************************/
/*!
  @brief  initiate a new line when drawing text.
  @param  f1 - current font height
  @param  ts - current multiplier for selected font
  @param  ux -
  @note Used for moving cursor to newLine and avoid last line of text to be
    not written to when scrolling, scroll will only occur when a new
    character occupies the last line.
*/
/****************************************************************************/
void gfx4desp32::newLine(int8_t f1, int8_t ts, uint16_t ux) {
    fsh1 = f1;
    int ScrollDist;
    nl = false;
    uint16_t remspc = ts * fsh1;
    cursor_y += remspc;
    if (sEnable) {
        if (cursor_y + remspc > getScrollareaY1()) {
            ScrollDist = (cursor_y + remspc) - getScrollareaY1();
            if (ScrollDist > 0)
                Scroll(ScrollDist);
            scrolled = true;
            cursor_y = getScrollareaY1() - remspc;
        }
    }
    cursor_x = ux;
    lastfsh = remspc;
}

/****************************************************************************/
/*!
  @brief  Clear screen to black screen.
  @note May need to consider resseting more functions eg clip, scroll,
  transparency
*/
/****************************************************************************/
void gfx4desp32::Cls() {
    FillScreen(BLACK);
    cursor_x = 0;
    cursor_y = 0;
    // Scroll(0);
    scrolled = false;
    nl = false;
    _nlh = height;
}

/****************************************************************************/
/*!
  @brief  Clear screen to custom colour
  @param  color - RGB565 colour
  @note As Cls above
*/
/****************************************************************************/
void gfx4desp32::Cls(uint16_t color) {
    FillScreen(color);
    cursor_x = 0;
    cursor_y = 0;
    scrolled = false;
    nl = false;
    _nlh = height;
}

/****************************************************************************/
/*!
  @brief  Enable / disable auto scrolling.
  @param  bool sEn
  @note sets global sEnable flag
*/
/****************************************************************************/
void gfx4desp32::ScrollEnable(bool sEn) {
    _ScrollEnable(sEn);
    sEnable = sEn;
}

/****************************************************************************/
/*!
  @brief  Get X text cursor position.
  @note returns cursor_x variable
*/
/****************************************************************************/
int16_t gfx4desp32::getX(void) { return cursor_x; }

/****************************************************************************/
/*!
  @brief  Get Y text cursor position.
  @note returns cursor_y variable
*/
/****************************************************************************/
int16_t gfx4desp32::getY(void) { return cursor_y; }

/****************************************************************************/
/*!
  @brief  Draw system font 1 character.
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  c - character to be drawn
  @param  color - foreground colour of character
  @param  bg - background colour of character
  @param  sizew - multiplier for width
  @param  sizeht - multiplier for width
  @note text size multiplier affects width and height and is not supported
    seperately. Font 1 has selectable drawing styles using FontStyle
    function.
*/
/****************************************************************************/
void gfx4desp32::drawChar1(int16_t x, int16_t y, unsigned char c,
    uint16_t color, uint16_t bg, uint8_t sizew,
    uint8_t sizeht) {
    bool needsEndWrite = StartWrite();
    int crad = 0;
    int co = 0;
    if (fstyle == 1 || fstyle == 2 || fstyle == 3)
        crad = sizew >> 1;
    if (fstyle == DOTMATRIXLED)
        co = crad * 68 / 100;
    if (sizew > 3)
        crad--;
    for (int8_t i = 0; i < 6; i++) {
        uint8_t tcol;
        if (i == (fsw)) {
            tcol = 0x0;
        }
        else {
            tcol = font1[(c * 5) + i];
        }
        for (int8_t j = 0; j < 8; j++) {
            if (i == 5)
                tcol = 0;
            if (tcol & 0x1) {
                if (sizew == 1 && sizeht == 1) {
                    PutPixel(x + i, y + j, color);
                }
                else {
                    if (fstyle == 0)
                        RectangleFilled(x + (i * sizew), y + (j * sizeht),
                            (sizew + x) + (i * sizew) - 1,
                            (sizeht + y) + (j * sizeht) - 1, color);
                    if (fstyle == 2)
                        Circle(x + (i * sizew) + crad, y + (j * sizeht) + crad, crad,
                            color);
                    if (fstyle == 1)
                        CircleFilled(x + (i * sizew) + crad, y + (j * sizeht) + crad, crad,
                            color);
                    if (fstyle == 3) {
                        CircleFilled(x + (i * sizew) + crad, y + (j * sizeht) + crad, crad,
                            color);
                        CircleFilled(x + (i * sizew) + co, y + (j * sizeht) + co, crad / 3,
                            WHITE);
                    }
                    if (fstyle == 4)
                        RectangleFilled(x + (i * sizew), y + (j * sizeht),
                            (sizew + x) + (i * sizew) - 2,
                            (sizeht + y) + (j * sizeht) - 2, color);
                    if (fstyle == 5) {
                        uint16_t fadcol = color;
                        fadcol = HighlightColors(fadcol, 10) & 0xffff;
                        int step = 60 / (sizew / 2);
                        if (step < 1)
                            step = 1;
                        for (int n = sizew / 2; n > -1; n--) {
                            Rectangle(x + (i * sizew) + n, y + (j * sizeht) + n,
                                (sizew + x) + (i * sizew) - 1 - n,
                                (sizeht + y) + (j * sizeht) - 1 - n, fadcol);
                            fadcol = HighlightColors(fadcol, step) >> 16;
                        }
                    }
                }
            }
            else if (opacitystate == false) {
                if (sizew == 1 && sizeht == 1) {
                    PutPixel(x + i, y + j, bg);
                }
                else {
                    RectangleFilled(x + i * sizew, y + j * sizeht,
                        (sizew + x) + i * sizew - 1,
                        (sizeht + y) + j * sizeht - 1, bg);
                }
            }
            tcol >>= 1;
        }
    }

    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw system font 2 character.
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  c - character to be drawn
  @param  color - foreground colour of character
  @param  bg - background colour of character
  @param  sizew - multiplier for width
  @param  sizeht - multiplier for width
  @note text size multiplier affects width and height and is not supported
    seperately.
*/
/****************************************************************************/
void gfx4desp32::drawChar2(int16_t x, int16_t y, unsigned char c,
    uint16_t color, uint16_t bg, uint8_t sizew,
    uint8_t sizeht) {
    bool needsEndWrite = StartWrite();

    for (int8_t j = 0; j < 16; j++) {
        uint8_t trow;
        trow = font2[(c * 16) + j];
        for (int8_t i = 0; i < (fsw + 1); i++) {
            if (i == (fsw)) {
                trow = 0x00;
            }
            if (trow & 0x80) {
                if (sizew == 1 && sizeht == 1) {
                    PutPixel(x + i, y + j, color);
                }
                else {
                    RectangleFilled(x + (i * sizew), y + (j * sizeht),
                        (sizew + x) + (i * sizew) - 1,
                        (sizeht + y) + (j * sizeht) - 1, color);
                }
            }
            else if (opacitystate == false) {
                if (sizew == 1 && sizeht == 1) {
                    PutPixel(x + i, y + j, bg);
                }
                else {
                    RectangleFilled(x + (i * sizew), y + (j * sizeht),
                        (sizew + x) + (i * sizew) - 1,
                        (sizeht + y) + (j * sizeht) - 1, bg);
                }
            }
            trow <<= 1;
        }
    }
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw GCI font character.
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  c - character to be drawn
  @param  color - foreground colour of character
  @param  bg - background colour of character
  @param  sizew - multiplier for width
  @param  sizeht - multiplier for width
  @note text size multiplier affects width and height and is not supported
      seperately. Font 1 has selectable drawing styles using FontStyle
      function.
*/
/****************************************************************************/
void gfx4desp32::drawChar4D(int16_t x, int16_t y, uint16_t c,
    uint16_t color, uint16_t bg, uint8_t sizew,
    uint8_t sizeht) {

    if (!gciFont && !fontPtr)
        return;

    if (c == '\r') {
        cursor_x = 0;
        return;
    }

    if (c == '\n') {
        cursor_y += fsh;
        cursor_x = 0; // ensures that \n works the same as \r\n
        return;
    }

    if (c >= fsc || c < fso) {
        // Character is not included
        return;
    }

    uint8_t _width = fsw;
    uint8_t _data[fsb];

    const uint8_t* data = &_data[2];

    int offset = c * fsb + 8; // character offset (number of bytes per character *
    // character value) +  8-byte header

    if (fno == 0) {
        gciFont.seek(offset);
        gciFont.read(_data, fsb);
        // width is different for each character
        _width = _data[0] << 8 | _data[1];
    }
    else {
        // width is different for each character
        _width = fontPtr[offset] << 8 | fontPtr[offset + 1];
        data = &fontPtr[offset + 2];
    }

    if (wrap && cursor_x + _width > getWidth()) {
        // if next character overflows, move to next line
        cursor_y += fsh;
        cursor_x = 0;
    }

    int bytePerRow = _width << 1;
    uint16_t actualSize = fsh * bytePerRow;

    bool needsEndWrite = StartWrite();
    int j = -1;
    for (int i = 0; i < actualSize; i += 2) {
        int h = i % bytePerRow;
        if (h == 0) {
            j++;
        }
        h >>= 1;
        if ((data[i] << 8 | data[i + 1]) != 0) {
            if (sizew == 1 && sizeht == 1) {
                PutPixel(cursor_x + h, cursor_y + j, color);
            }
            else {
                RectangleFilled(cursor_x + h, cursor_y + j, cursor_x + h + sizew - 1,
                    cursor_y + j + sizeht - 1, color);
            }
        }
        else if (opacitystate == OPAQUE) {
            if (sizew == 1 && sizeht == 1) {
                PutPixel(cursor_x + h, cursor_y + j, bg);
            }
            else {
                RectangleFilled(cursor_x + h, cursor_y + j, cursor_x + h + sizew - 1,
                    cursor_y + j + sizeht - 1, bg);
            }
        }
    }
    if (needsEndWrite)
        EndWrite();
    cursor_x += _width * sizew;
}

/****************************************************************************/
/*!
  @brief  Draw Diablo16's FONT4 formatted font character.
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  c - character to be drawn
  @param  color - foreground colour of character
  @param  bg - background colour of character
  @param  sizew - multiplier for width
  @param  sizeht - multiplier for width
  @note text size multiplier affects width and height and is not supported
      seperately. Font 1 has selectable drawing styles using FontStyle
      function.
*/
/****************************************************************************/
void gfx4desp32::drawChar4Dcmp(int16_t x, int16_t y, uint16_t c,
    uint16_t color, uint16_t bg, uint8_t sizew,
    uint8_t sizeht) {

    if (!fontPtr)
        return;

    if (c == '\r') {
        cursor_x = 0;
        return;
    }

    if (c == '\n') {
        cursor_y += fsh;
        cursor_x = 0; // ensures that \n works the same as \r\n
        return;
    }

    if (c >= fsc || c < fso) {
        // Character is not included
        return;
    }

    uint8_t width = fsw;
    uint8_t bytes_per_row = (fsw + 7) >> 3;
    // +7 ensures the bits will be >= next bit count in multiple of 8
    // >> 3 divides it by 8

    const uint8_t* data = fntData;

    data +=
        (c - fso) * ((bytes_per_row * fsh) + ((fontPtr[FONT_TYPE] != 0) ? 1 : 0));

    if (fontPtr[FONT_TYPE] != 0) {
        // if not simple
        // width is different for each character
        width = data[0];
        data++;
    }

    if (wrap && cursor_x + width > getWidth()) {
        // if next character overflows, move to next line
        cursor_y += fsh;
        cursor_x = 0;
    }

    bool needsEndWrite = StartWrite();

    int16_t _x, _y;

    // loop here
    for (int i = 0; i < fsh; i++) {
        int byteIndex = 0;
        int bitIndex = 7;
        int numBits = width;

        while (numBits-- > 0) {
            _x = cursor_x + (width - numBits) * sizew - 1;
            _y = cursor_y + i * sizeht;
            if ((data[byteIndex] >> bitIndex) & 0x01) {
                if (sizew == 1 && sizeht == 1) {
                    PutPixel(_x, _y, color);
                }
                else {
                    RectangleFilled(_x, _y, _x + sizew - 1, _y + sizeht - 1, color);
                }
            }
            else {
                if (opacitystate == OPAQUE) {
                    if (sizew == 1 && sizeht == 1) {
                        PutPixel(_x, _y, bg);
                    }
                    else {
                        RectangleFilled(_x, _y, x + sizew - 1, y + sizeht - 1, bg);
                    }
                }
            }
            if (--bitIndex < 0) {
                byteIndex++;
                bitIndex = 7;
            }
        }

        data += bytes_per_row;
    }

    if (needsEndWrite)
        EndWrite();
    cursor_x += width * sizew;
}

/****************************************************************************/
/*!
  @brief  Move origin for drawing operations.
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @note text position is set using this function.
*/
/****************************************************************************/
void gfx4desp32::MoveTo(int16_t x, int16_t y) {
    cursor_x = x;
    cursor_y = y;
    if (wrap == true) {
        if (cursor_y > (height - 1))
            cursor_y = height - 1;
        if (cursor_y < 0)
            cursor_y = 0;
        if (cursor_x < 0)
            cursor_x = 0;
        if (cursor_x > (width - 1))
            cursor_x = width - 1;
    }
    nl = false;
}

/****************************************************************************/
/*!
  @brief  Get the current system font number.
  @note System font number
*/
/****************************************************************************/
int8_t gfx4desp32::Font(void) { return fno; }

/****************************************************************************/
/*!
  @brief  Set system font
  @param  f - 1 or 2
  @note sets font and sets width and height variables
*/
/****************************************************************************/
void gfx4desp32::Font(uint8_t f) {
    if (f < 1 || f > 2)
        return;
    fno = f;
    if (fno == 1) {
        fsw = 5;
        fsh = 8;
    }
    if (fno == 2) {
        fsw = 8;
        fsh = 16;
    }
}

/****************************************************************************/
/*!
  @brief  Set Font as GCI font (IFont)
  @param  f - font opened using Open4dFont()
  @note sets font and sets width and height variables
*/
/****************************************************************************/
void gfx4desp32::Font(gfx4d_font f) {
    if (!f)
        return;

    fno = 0;
    gciFont = f;

    f.seek(0);
    fsw = (f.read() << 8) | f.read();
    fsh = (f.read() << 8) | f.read();
    f.seek(6);
    fsc = (f.read() << 8) | f.read();
    fsb = ((fsw * fsh) + 1) << 1;
}

/****************************************************************************/
/*!
  @brief  Set Font as IFont array
  @param  f - font opened using Open4dFont()
  @note sets font and sets width and height variables
*/
/****************************************************************************/
void gfx4desp32::Font(const uint8_t* f, bool compressed) {
    if (!f)
        return;

    fno = -1;
    fontPtr = f;
    fntCmprs = compressed;

    if (compressed) {

        int i = 1;

        fntCharCount = f[i++];
        if (f[FONT_TYPE] == 3) {
            fntCharCount |= f[i++] << 8;
        }
        fso = f[i++];
        fsw = f[i++];
        fsh = f[i++];

        fsc = fntCharCount + fso;

        fsb = fsh * ((fsw + 7) >> 3);
        switch (f[FONT_TYPE]) {
        case 0:
            fntData = &f[i];
            break;
        case 1:
            fntData = &f[i];
            fsb++;
            break;
        case 2:
            fntWidths = &f[i];
            fntData = fntWidths + fntCharCount;
            fsb++;
            break;
        case 3:
            fntWidths = &f[i];
            fntData = fntWidths + fntCharCount;
            fsb++;
            break;
        default:
            break;
        }

    }
    else {

        fsw = (f[0] << 8) | f[1];
        fsh = (f[2] << 8) | f[3];
        fsc = (f[6] << 8) | f[7];
        fsb = ((fsw * fsh) + 1) << 1;
    }
}

void gfx4desp32::__tempFont(int8_t f) {
    fnoBkup = fno;
    gciFontBkup = gciFont;
    fontPtrBkup = fontPtr;
    Font(f);
}

void gfx4desp32::__tempFont(const uint8_t* f, bool compressed) {
    fnoBkup = fno;
    gciFontBkup = gciFont;
    fontPtrBkup = fontPtr;
    fntCmprsBkup = fntCmprs;
    Font(f, compressed);
}

void gfx4desp32::__tempFont(gfx4d_font f) {
    fnoBkup = fno;
    gciFontBkup = gciFont;
    fontPtrBkup = fontPtr;
    Font(f);
}

void gfx4desp32::__restoreFont() {
    switch (fnoBkup) {
    case 0:
        Font(gciFontBkup);
        break;
    case -1:
        Font(fontPtrBkup, fntCmprsBkup);
        break;
    default:
        Font(fnoBkup);
        break;
    }
}

/****************************************************************************/
/*!
  @brief  system font multiplier
  @param  s - multiplier
  @note default is 1
*/
/****************************************************************************/
void gfx4desp32::TextSize(uint8_t s) {
    if (s > 0) {
        lastsizeht = textsizeht;
        textsize = s;
        textsizeht = s;
    }
}
/****************************************************************************/
/*!
  @brief  Set text foreground colour (compatible wrapper)
  @param  c - RGB565 colour
  @note foreground colour is set and transparency is controlled with opacity
        Opacity command.
*/
/****************************************************************************/
void gfx4desp32::FGcolour(uint16_t c) { textcolor = c; }
/****************************************************************************/
/*!
  @brief  Set text background colour (compatible wrapper)
  @param  c - RGB565 colour
  @note background colour is set and transparency is controlled with opacity
        Opacity command.
*/
/****************************************************************************/
void gfx4desp32::BGcolour(uint16_t c) { textbgcolor = c; }
/****************************************************************************/
/*!
  @brief  Set text foreground & background colour
  @param  c - RGB565 colour
  @note foreground and background colour are set the same and character is
    drawn without background and Opacity is set to transparent.
*/
/****************************************************************************/
void gfx4desp32::TextColor(uint16_t c) {
    textcolor = c;
    textbgcolor = c;
    opacitystate = TRANSPARENT;
}
/****************************************************************************/
/*!
  @brief  Sett text foreground & background colour with different colours
  @param  c - RGB565 foreground colour
  @param  b - RGB565 background colour
  @note foreground and background colour set seperatley. Background will be
    drawn if it is different to foreground if Opacity is set to OPAQUE.
    If Colours are the same then it is assumed TRANSPARENT
    and Opacity is set TRANSPARENT.
*/
/****************************************************************************/
void gfx4desp32::TextColor(uint16_t c, uint16_t b) {
    textcolor = c;
    textbgcolor = b;
    if (c == b) {
        opacitystate = TRANSPARENT;
    }
    else {
        opacitystate = OPAQUE;
    }
}
/****************************************************************************/
/*!
  @brief  Set if newLine is called when tet reaches end of screen.
  @param  w enable / disable
*/
/****************************************************************************/
void gfx4desp32::TextWrap(boolean w) { wrap = w; }

/****************************************************************************/
/*!
  @brief  Return height of current selected font
*/
/****************************************************************************/
int8_t gfx4desp32::FontHeight(void) { return fsh; }

/****************************************************************************/
/*!
  @brief  Set screen orientation
  @param  r - orientation - LANDSCAPE, LANDSCAPE_R, PORTRAIT, PORTRAIT_R
  @note Calls orientation function in panel and sets width & height variables
*/
/****************************************************************************/
void gfx4desp32::Orientation(uint8_t r) {
    panelOrientation(r);
    width = getWidth();
    height = getHeight();
    _nlh = height;
    rotation = r;
}

/****************************************************************************/
/*!
  @brief  Get current orientation
*/
/****************************************************************************/
uint8_t gfx4desp32::Orientation() { return rotation; }

/****************************************************************************/
/*!
  @brief  Draw rectangle (outline
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  x1 - right x position in pixels
  @param  y1 - bottom y position in pixels
  @param  color - RGB565 colour
  @note Clipping, if set, is handled by Hline / Vline functions
*/
/****************************************************************************/
void gfx4desp32::Rectangle(int16_t x, int16_t y, int16_t x1, int16_t y1,
    uint16_t color) {
    bool needsEndWrite = StartWrite();
    if (x > x1)
        swap(x, x1);
    if (y > y1)
        swap(y, y1);
    //if (x < 0)
    //    x = 0;
    //if (y < 0)
    //    y = 0;
    int w = x1 - x + 1;
    int h = y1 - y + 1;
    Hline(x, y, w, color);
    Hline(x, y + h - 1, w, color);
    Vline(x, y, h, color);
    Vline(x + w - 1, y, h, color);
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw circle (outline).
  @param  xc - left X position in pixels
  @param  yc - top Y position in pixels
  @param  r - radius of circle
  @param  color - RGB565 colour
  @note clipping is handled by the PutPixel function.
*/
/****************************************************************************/
void gfx4desp32::Circle(int16_t xc, int16_t yc, int16_t r, uint16_t color) {
    bool needsEndWrite = StartWrite();
    int16_t c = 1 - r;
    int16_t xx = 1;
    int16_t yy = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    PutPixel(xc, yc + r, color);
    PutPixel(xc, yc - r, color);
    PutPixel(xc + r, yc, color);
    PutPixel(xc - r, yc, color);
    while (x < y) {
        if (c >= 0) {
            y--;
            yy = yy + 2;
            c = c + yy;
        }
        x++;
        xx = xx + 2;
        c = c + xx;
        PutPixel(xc + x, yc + y, color);
        PutPixel(xc - x, yc + y, color);
        PutPixel(xc + x, yc - y, color);
        PutPixel(xc - x, yc - y, color);
        PutPixel(xc + y, yc + x, color);
        PutPixel(xc - y, yc + x, color);
        PutPixel(xc + y, yc - x, color);
        PutPixel(xc - y, yc - x, color);
    }
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw circle (filled).
  @param  xc - left X position in pixels
  @param  yc - top Y position in pixels
  @param  r - daius of circle
  @param  color - RGB565 colour
  @note clipping is handled by PutPixel and Vline functions.
*/
/****************************************************************************/
void gfx4desp32::CircleFilled(int16_t xc, int16_t yc, int16_t r,
    uint16_t color) {
    bool needsEndWrite = StartWrite();
    Vline(xc, yc - r, 2 * r + 1, color);
    ArcFilled(xc, yc, r, 3, 0, color);
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw Ellipse (outline).
  @param  xe - left X position in pixels
  @param  ye - top Y position in pixels
  @param  radx - radius in a horizontal direction
  @param  rady - radius in a vertical direction
  @param  color - RGB565 colour
  @note clipping is handled by PutPixel function
*/
/****************************************************************************/
void gfx4desp32::Ellipse(int16_t xe, int16_t ye, int16_t radx, int16_t rady,
    uint16_t color) {
    if (radx < 2)
        return;
    if (rady < 2)
        return;
    bool needsEndWrite = StartWrite();
    int16_t x, y;
    int32_t es;
    int32_t radxx = radx * radx;
    int32_t radyy = rady * rady;
    int32_t xr = 4 * radxx;
    int32_t yr = 4 * radyy;
    for (x = 0, y = rady, es = 2 * radyy + radxx * (1 - 2 * rady);
        radyy * x <= radxx * y; x++) {
        PutPixel(xe + x, ye + y, color);
        PutPixel(xe - x, ye + y, color);
        PutPixel(xe - x, ye - y, color);
        PutPixel(xe + x, ye - y, color);
        if (es >= 0) {
            es += xr * (1 - y);
            y--;
        }
        es += radyy * ((4 * x) + 6);
    }
    for (x = radx, y = 0, es = 2 * radxx + radyy * (1 - 2 * radx);
        radxx * y <= radyy * x; y++) {
        PutPixel(xe + x, ye + y, color);
        PutPixel(xe - x, ye + y, color);
        PutPixel(xe - x, ye - y, color);
        PutPixel(xe + x, ye - y, color);
        if (es >= 0) {
            es += yr * (1 - x);
            x--;
        }
        es += radxx * ((4 * y) + 6);
    }
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw Ellipse (filled).
  @param  xe - left X position in pixels
  @param  ye - top Y position in pixels
  @param  radx - radius in a horizontal direction
  @param  rady - radius in a vertical direction
  @param  color - RGB565 colour
  @note clipping is handled by Hline function
*/
/****************************************************************************/
void gfx4desp32::EllipseFilled(int16_t xe, int16_t ye, int16_t radx,
    int16_t rady, uint16_t color) {
    if (radx < 2)
        return;
    if (rady < 2)
        return;
    bool needsEndWrite = StartWrite();
    int16_t x, y;
    int32_t es;
    int32_t radxx = radx * radx;
    int32_t radyy = rady * rady;
    int32_t xr = 4 * radxx;
    int32_t yr = 4 * radyy;
    for (x = 0, y = rady, es = 2 * radyy + radxx * (1 - 2 * rady);
        radyy * x <= radxx * y; x++) {
        Hline(xe - x, ye - y, 1 + x + x, color);
        Hline(xe - x, ye + y, 1 + x + x, color);
        if (es >= 0) {
            es += xr * (1 - y);
            y--;
        }
        es += radyy * ((4 * x) + 6);
    }
    for (x = radx, y = 0, es = 2 * radxx + radyy * (1 - 2 * radx);
        radxx * y <= radyy * x; y++) {
        Hline(xe - x, ye - y, 1 + x + x, color);
        Hline(xe - x, ye + y, 1 + x + x, color);
        if (es >= 0) {
            es += yr * (1 - x);
            x--;
        }
        es += radxx * ((4 * y) + 6);
    }
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw Arc (outline).
  @param  xa - left X position in pixels
  @param  ya - top Y position in pixels
  @param  r - radius
  @param  sa - start angle
  @param  ea - end angle
  @param  color - RGB565 colour
  @note clipping is handled by Vline function
*/
/****************************************************************************/
void gfx4desp32::ArcFilled(int16_t xa, int16_t ya, int16_t r, int16_t sa,
    int16_t ea, uint16_t color) {
    int16_t c = 1 - r;
    int16_t x = 0;
    int16_t y = r;
    int16_t xx = 1;
    int16_t yy = -2 * r;
    while (x < y) {
        if (c >= 0) {
            y--;
            yy = yy + 2;
            c = c + yy;
        }
        x++;
        xx = xx + 2;
        c = c + xx;
        if (sa & 0x1) {
            Vline(xa + x, ya - y, 2 * y + 1 + ea, color);
            Vline(xa + y, ya - x, 2 * x + 1 + ea, color);
        }
        if (sa & 0x2) {
            Vline(xa - x, ya - y, 2 * y + 1 + ea, color);
            Vline(xa - y, ya - x, 2 * x + 1 + ea, color);
        }
    }
}

/****************************************************************************/
/*!
  @brief  Draw Arc (outline).
  @param  x0 - left X position in pixels
  @param  y0 - top Y position in pixels
  @param  r - radius
  @param  sa - start angle
  @param  color - RGB565 colour
  @note clipping is handled by PutPixel function
*/
/****************************************************************************/
void gfx4desp32::Arc(int16_t x0, int16_t y0, int16_t r, uint16_t sa,
    uint16_t color) {
    int16_t c = 1 - r;
    int16_t xx = 1;
    int16_t yy = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    while (x < y) {
        if (c >= 0) {
            y--;
            yy = yy + 2;
            c = c + yy;
        }
        x++;
        xx = xx + 2;
        c = c + xx;
        if (sa & 0x4) {
            PutPixel(x0 + x, y0 + y, color);
            PutPixel(x0 + y, y0 + x, color);
        }
        if (sa & 0x2) {
            PutPixel(x0 + x, y0 - y, color);
            PutPixel(x0 + y, y0 - x, color);
        }
        if (sa & 0x8) {
            PutPixel(x0 - y, y0 + x, color);
            PutPixel(x0 - x, y0 + y, color);
        }
        if (sa & 0x1) {
            PutPixel(x0 - y, y0 - x, color);
            PutPixel(x0 - x, y0 - y, color);
        }
    }
}

/****************************************************************************/
/*!
  @brief  Draw Rounded rectangle (filled).
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  x1 - right X position in pixels
  @param  y1 - bottom Y position in pixels
  @param  r - arc radius
  @param  color - RGB565 colour
  @note clipping is handled by filled rectangle function and filled arc function
*/
/****************************************************************************/
void gfx4desp32::RoundRectFilled(int16_t x, int16_t y, int16_t x1, int16_t y1,
    int16_t r, uint16_t color) {
    bool needsEndWrite = StartWrite();
    if (x > x1) {
        swap(x, x1);
    }
    if (y > y1) {
        swap(y, y1);
    }
    int w = x1 - x + 1;
    int h = y1 - y + 1;
    int maxR = 0;
    if (w >= h)
        maxR = (h - 1) / 2;
    else if (h > w)
        maxR = (w - 1) / 2;
    if (r > maxR)
        r = maxR;
    RectangleFilled(x + r, y, x + (w - r) - 1, y + h - 1, color);
    ArcFilled(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    ArcFilled(x + r, y + r, r, 2, h - 2 * r - 1, color);
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw Rounded rectangle (outline).
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  x1 - right X position in pixels
  @param  y1 - bottom Y position in pixels
  @param  r - arc radius
  @param  color - RGB565 colour
  @note clipping is handled by Hline, Vline functions and arc function
*/
/****************************************************************************/
void gfx4desp32::RoundRect(int16_t x, int16_t y, int16_t x1, int16_t y1,
    int16_t r, uint16_t color) {
    bool needsEndWrite = StartWrite();
    int w = x1 - x + 1;
    int h = y1 - y + 1;
    Hline(x + r, y, w - 2 * r, color);
    Hline(x + r, y + h - 1, w - 2 * r, color);
    Vline(x, y + r, h - 2 * r, color);
    Vline(x + w - 1, y + r, h - 2 * r, color);
    Arc(x + r, y + r, r, 1, color);
    Arc(x + w - r - 1, y + r, r, 2, color);
    Arc(x + w - r - 1, y + h - r - 1, r, 4, color);
    Arc(x + r, y + h - r - 1, r, 8, color);
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw line between 2 points
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  x1 - right X position in pixels
  @param  y1 - bottom Y position in pixels
  @param  color - RGB565 colour
  @note clipping is handled by PutPixel function
*/
/****************************************************************************/
void gfx4desp32::Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    uint16_t color) {
    bool needsEndWrite = StartWrite();
    int16_t angH = abs(y1 - y0) > abs(x1 - x0);
    if (angH) {
        swap(x0, y0);
        swap(x1, y1);
    }
    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }
    int16_t xx;
    int16_t yy;
    xx = x1 - x0;
    yy = abs(y1 - y0);
    int16_t edx = xx / 2;
    int16_t incy;
    if (y0 < y1) {
        incy = 1;
    }
    else {
        incy = -1;
    }
    for (; x0 <= x1; x0++) {
        if (angH) {
            PutPixel(y0, x0, color);
        }
        else {
            PutPixel(x0, y0, color);
        }
        edx = edx - yy;
        if (edx < 0) {
            y0 = y0 + incy;
            edx = edx + xx;
        }
    }
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw Rounded rectangle (filled).
  @param  x0 - first X position in pixels
  @param  y0 - first Y position in pixels
  @param  x1 - second X position in pixels
  @param  y1 - second Y position in pixels
  @param  x2 - third X position in pixels
  @param  y2 - third Y position in pixels
  @param  color - RGB565 colour
  @note clipping is handled by Line function
*/
/****************************************************************************/
void gfx4desp32::Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    int16_t x2, int16_t y2, uint16_t color) {
    bool needsEndWrite = StartWrite();
    Line(x0, y0, x1, y1, color);
    Line(x1, y1, x2, y2, color);
    Line(x2, y2, x0, y0, color);
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw Rounded rectangle (filled).
  @param  x0 - first X position in pixels
  @param  y0 - first Y position in pixels
  @param  x1 - second X position in pixels
  @param  y1 - second Y position in pixels
  @param  x2 - third X position in pixels
  @param  y2 - third Y position in pixels
  @param  color - RGB565 colour
  @note clipping is handled by Hline function
*/
/****************************************************************************/
void gfx4desp32::TriangleFilled(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
    int16_t x2, int16_t y2, uint16_t color) {
    int16_t p0, p1, y, last;
    if (y0 > y1) {
        swap(y0, y1);
        swap(x0, x1);
    }
    if (y1 > y2) {
        swap(y2, y1);
        swap(x2, x1);
    }
    if (y0 > y1) {
        swap(y0, y1);
        swap(x0, x1);
    }
    if (y0 == y2) {
        p0 = p1 = x0;
        if (x1 < p0)
            p0 = x1;
        else if (x1 > p1)
            p1 = x1;
        if (x2 < p0)
            p0 = x2;
        else if (x2 > p1)
            p1 = x2;
        Hline(p0, y0, p1 - p0 + 1, color);
        return;
    }
    bool needsEndWrite = StartWrite();
    int16_t xx01 = x1 - x0, yy01 = y1 - y0, xx02 = x2 - x0, yy02 = y2 - y0;
    int16_t xx12 = x2 - x1, yy12 = y2 - y1;
    int32_t z1 = 0, z2 = 0;
    if (y1 == y2) {
        last = y1;
    }
    else {
        last = y1 - 1;
    }
    for (y = y0; y <= last; y++) {
        p0 = x0 + z1 / yy01;
        p1 = x0 + z2 / yy02;
        z1 += xx01;
        z2 += xx02;
        if (p0 > p1) {
            swap(p0, p1);
        }
        Hline(p0, y, p1 - p0 + 1, color);
    }
    z1 = xx12 * (y - y1);
    z2 = xx02 * (y - y0);
    for (; y <= y2; y++) {
        p0 = x1 + z1 / yy12;
        p1 = x0 + z2 / yy02;
        z1 += xx12;
        z2 += xx02;
        if (p0 > p1) {
            swap(p0, p1);
        }
        Hline(p0, y, p1 - p0 + 1, color);
    }
    if (needsEndWrite)
        EndWrite();
}

/****************************************************************************/
/*!
  @brief  Set maximum allowed widgets.
  @param  mw - total number
  @note Not necessary on ESP32
*/
/****************************************************************************/
void gfx4desp32::SetMaxWidgets(int mw) { MAX_WIDGETS = mw; }

/****************************************************************************/
/*!
  @brief  Allocate custom PSRAM space for gci & dat laoded into PSRAM.
  @param  datS - size of dat file
  @param  gciS - size of gci file
  @note Open4DGFXtoPSRAM will automatically allocate the correct size of PSRAM
        but if the gci & dat files are expected to change in the sketch progmatically
        then it would be necessary to create space for the expected largest
*/
/****************************************************************************/
void gfx4desp32::AllocatePSRAMgciSpace(uint32_t datS, uint32_t gciS) {
    DAT_PSRAM_allocated = datS;
    GCI_PSRAM_allocated = gciS;
}

/****************************************************************************/
/*!
  @brief  Open4dGFXtoPSRAM helper function to load uSD gci & dat to PSRAM
  @param  file4d - previously selected filename
  @note using this method for displaying uSD is very fast but care needs to taken
        that gci & dat does not exceed available PSRAM. The project can still be
        edited as normal in WS4 and graphics need to be saved to uSD. a 320 x 240
        display can have as much as 6mB free PSRAM space wheras an 800 x 480 display
        would have between 4 and 5 mB free.
*/
/****************************************************************************/
void gfx4desp32::Open4dGFXtoPSRAM(String file4d) {
    dat4d = file4d + ".dat";
    gci4d = file4d + ".gci";
#ifdef USE_LITTLEFS_FILE_SYSTEM
    dat4d = "/" + dat4d;
    gci4d = "/" + gci4d;
    userDat = LittleFS.open((char*)dat4d.c_str(), "r");
#else
#ifdef USE_SDMMC_FILE_SYSTEM
    userDat = SD_MMC.open(dat4d);
#else
    userDat = uSD.open(dat4d);
#endif
#endif
    cache_DAT_size = userDat.size();
    if (cache_DAT_size > 0) {
        if (DAT_PSRAM_allocated > 0) {
            cache_DAT = (uint8_t*)ps_malloc(DAT_PSRAM_allocated); // Create PSRAM cache space
        }
        else {
            cache_DAT = (uint8_t*)ps_malloc(cache_DAT_size); // Create PSRAM cache space
        }
        userDat.read(cache_DAT, cache_DAT_size);
    }
    userDat.close();
#ifdef USE_LITTLEFS_FILE_SYSTEM
    userImag = LittleFS.open((char*)gci4d.c_str(), "r");
#else
#ifdef USE_SDMMC_FILE_SYSTEM
    userImag = SD_MMC.open(gci4d);
#else
    userImag = uSD.open(gci4d);
#endif
#endif
    cache_GCI_size = userImag.size();
    if (cache_GCI_size > 0) {
        if (GCI_PSRAM_allocated > 0) {
            cache_GCI = (uint8_t*)ps_malloc(GCI_PSRAM_allocated); // Create PSRAM cache space
        }
        else {
            cache_GCI = (uint8_t*)ps_malloc(cache_GCI_size); // Create PSRAM cache space
        }
        userImag.read(cache_GCI, cache_GCI_size);
    }
    userImag.close();
    Open4dGFX(cache_DAT, cache_DAT_size, cache_GCI, cache_GCI_size);
}

/****************************************************************************/
/*!
  @brief  Open4dGFX helper function
  @param  file4d - previously selected filename
  @param  alloxMAX - choose between allocating memory for amound in uSD of set
  MAX
  @note to save memory, allocMAX can be set to allocate enough for counted
  objects
*/
/****************************************************************************/
void gfx4desp32::Open4dGFX(String file4d) {
    if (!gciImagesUsed) {
        gciImagesUsed = true;
        cdv = (uint8_t*)malloc(MAX_WIDGETS);
        gciobjframes = (uint16_t*)malloc(MAX_WIDGETS << 1);
        tuix = (int16_t*)malloc(MAX_WIDGETS << 1);
        tuiy = (int16_t*)malloc(MAX_WIDGETS << 1);
        tuiw = (int16_t*)malloc(MAX_WIDGETS << 1);
        tuih = (int16_t*)malloc(MAX_WIDGETS << 1);
        tuiImageIndex = (uint16_t*)malloc(MAX_WIDGETS << 1);
        tuiIndex = (uint32_t*)malloc(MAX_WIDGETS << 2);
        tuiExtra1 = (uint16_t*)malloc(MAX_WIDGETS << 1);
        tuiExtra2 = (uint16_t*)malloc(MAX_WIDGETS << 1);
    }
    _Open4dGFX(file4d, false);
    opgfx = 1;
}

/****************************************************************************/
/*!
  @brief  Open4dGFX helper function
  @param  file4d - previously selected filename
  @param  scan - pre count number of objects for memory saving allocation
*/
/****************************************************************************/
void gfx4desp32::_Open4dGFX(String file4d, bool scan) {
    if (userImag)
        Close4dGFX();
    uint8_t strpos = 0;
    uint8_t gotchar = 0;
    uint8_t ofset = 0;
    gciobjnum = 0;
    if (file4d != "gfx4dDummy")
        GCItype = GCI_SYSTEM_USD;

    String inputString;
    dat4d = file4d + ".dat";
    gci4d = file4d + ".gci";
#ifdef USE_LITTLEFS_FILE_SYSTEM
    if (GCItype == GCI_SYSTEM_USD) {
        dat4d = "/" + dat4d;
        gci4d = "/" + gci4d;
        userDat = LittleFS.open((char*)dat4d.c_str(), "r");
    }
    if (GCItype == GCI_SYSTEM_PROGMEM) {
        datArrayPos = 0;
        gciArrayPos = 0;
        gcidatArray = false;
        if (datArraySize > 0 && gciArraySize > 0) {
            gcidatArray = true;
        }
    }
#else
    if (GCItype == GCI_SYSTEM_USD) {
#ifdef USE_SDMMC_FILE_SYSTEM
        userDat = SD_MMC.open(dat4d);
#else
        userDat = uSD.open(dat4d);
#endif
    }
    if (GCItype == GCI_SYSTEM_PROGMEM) {
        datArrayPos = 0;
        gciArrayPos = 0;
        gcidatArray = false;
        if (datArraySize > 0 && gciArraySize > 0) {
            gcidatArray = true;
        }
    }
#endif
    if (userDat || gcidatArray) {
        char c;
        char prevc = 0;
        if (GCItype == GCI_SYSTEM_USD) {
            while (userDat.available() > 0) {
                c = userDat.read();
                if (c != 13 && c != 10) {
                    strpos++;
                    if (c == 34) {
                        gotchar++;
                    }
                    if (gotchar == 2) {
                        ofset = strpos;
                        gotchar = 0;
                    }
                    inputString = inputString + char(c);
                }
                if (c == 13 || (c == 10 && prevc != 13)) {
                    strpos = 0;
                    String tempis = inputString;
                    uint32_t tuindex = getIndexfromString(tempis, (2 + ofset));
                    if (!(scan))
                        tuiIndex[gciobjnum] = tuindex;
                    getCoordfromString(tempis, (12 + ofset)); // dummy read required
                    if (!(scan))
                        tuix[gciobjnum] = xic;
                    if (!(scan))
                        tuiy[gciobjnum] = yic;
                    inputString = "";
                    gciobjnum++;
                }
                prevc = c;
            }
        }
        if (GCItype == GCI_SYSTEM_PROGMEM) {
            while (datArrayPos < datArraySize) {
                c = (char)DATarray[datArrayPos++];
                if (c != 13 && c != 10) {
                    strpos++;
                    if (c == 34) {
                        gotchar++;
                    }
                    if (gotchar == 2) {
                        ofset = strpos;
                        gotchar = 0;
                    }
                    inputString = inputString + char(c);
                }
                if (c == 13 || (c == 10 && prevc != 13)) {
                    strpos = 0;
                    String tempis = inputString;
                    uint32_t tuindex = getIndexfromString(tempis, (2 + ofset));
                    if (!(scan))
                        tuiIndex[gciobjnum] = tuindex;
                    getCoordfromString(tempis, (12 + ofset)); // dummy read required
                    if (!(scan))
                        tuix[gciobjnum] = xic;
                    if (!(scan))
                        tuiy[gciobjnum] = yic;
                    inputString = "";
                    gciobjnum++;
                }
                prevc = c;
            }
        }
    }
    if (GCItype == GCI_SYSTEM_USD) {
        userDat.close();
    }
    if (GCItype == GCI_SYSTEM_PROGMEM) {
        datArrayPos = 0;
    }
    if (scan)
        return;
#ifdef USE_LITTLEFS_FILE_SYSTEM
    if (GCItype == GCI_SYSTEM_USD) {
        userImag = LittleFS.open((char*)gci4d.c_str(), "r");
    }
    if (GCItype == GCI_SYSTEM_PROGMEM) {
        gciArrayPos = 0;
    }
#else
    if (GCItype == GCI_SYSTEM_USD) {
#ifdef USE_SDMMC_FILE_SYSTEM
        userImag = SD_MMC.open((char*)gci4d.c_str());
#else
        userImag = uSD.open((char*)gci4d.c_str());
#endif
    }
    if (GCItype == GCI_SYSTEM_PROGMEM) {
        gciArrayPos = 0;
    }
#endif

    uint32_t tIndex;

    for (int n = 0; n < gciobjnum; n++) {
        tIndex = tuiIndex[n];
        GCIseek(tIndex);
        if (!(scan))
            tuiw[n] = (GCIread() << 8) + GCIread();
        if (!(scan))
            tuih[n] = (GCIread() << 8) + GCIread();
        if (!(scan))
            cdv[n] = GCIread();
        int frms = GCIread();
        if (!(scan)) gciobjframes[n] = 0;
        if (frms != 0 && !(scan)) {
            gciobjframes[n] = (GCIread() << 8) + GCIread();
        }
    }
}

gfx4d_font gfx4desp32::Open4dFont(String font4d) {
#ifdef USE_LITTLEFS_FILE_SYSTEM
    String tstring = "/" + font4d;
    return LittleFS.open((char*)tstring.c_str(), "r");
#else
#ifdef USE_SDMMC_FILE_SYSTEM
    return SD_MMC.open(font4d);
#else
    return uSD.open(font4d);
#endif
#endif
}

/****************************************************************************/
/*!
  @brief  Get frames count for each widget
  @param  widget - widget ID
*/
/****************************************************************************/
uint16_t gfx4desp32::getWidgetNumFrames(int widget) {
    if (opgfx) {
        return gciobjframes[widget];
    }
    else {
        return 0;
    }
}

/****************************************************************************/
/*!
  @brief  Open4dGFX helper function
*/
/****************************************************************************/
uint32_t gfx4desp32::getIndexfromString(String strval, uint8_t indx) {
    String tempstrval;
    for (int n = 0; n < 4; n++) {
        tempstrval = tempstrval + strval.charAt(indx + n + 4);
    }
    for (int n = 0; n < 4; n++) {
        tempstrval = tempstrval + strval.charAt(indx + n - 1);
    }
    uint32_t tempaddr;
    tempaddr = strtol(&tempstrval[0], NULL, 16);
    return tempaddr;
}

/****************************************************************************/
/*!
  @brief  Open4dGFX helper function
*/
/****************************************************************************/
uint32_t gfx4desp32::getCoordfromString(String strval, uint8_t indx) {
    String tempstrval = "";
    char c;
    for (int n = 0; n < 18; n++) {
        c = strval.charAt(indx + n - 1);
        if (c != char(32) && c != char(0)) {
            tempstrval = tempstrval + char(c);
        }
        if (c == char(32)) {
            if (tempstrval.length() > 7) {
                String tbuild = "";
                for (int o = 0; o < 4; o++) {
                    tbuild = tbuild + tempstrval.charAt(o + 4);
                }
                tempstrval = tbuild;
            }
            xic = strtol(&tempstrval[0], NULL, 16);
            tempstrval = "";
        }
        if (c == char(0)) {
            if (tempstrval.length() > 7) {
                String tbuild = "";
                for (int o = 0; o < 4; o++) {
                    tbuild = tbuild + tempstrval.charAt(o + 4);
                }
                tempstrval = tbuild;
            }
            yic = strtol(&tempstrval[0], NULL, 16);
            break;
        }
    }
    uint32_t tempcoord;
    tempcoord = (xic << 16) & yic;
    return tempcoord;
}

/****************************************************************************/
/*!
  @brief  close opened gci file
*/
/****************************************************************************/
void gfx4desp32::Close4dGFX() {
    if (userImag) {
        if (GCItype == GCI_SYSTEM_USD)
            userImag.close();
        if (GCItype == GCI_SYSTEM_PROGMEM) {
            gcidatArray = false;
            gciArrayPos = 0;
        }
        gciobjnum = 0;
    }
}

/****************************************************************************/
/*!
  @brief  Draw widget from GCI file. UserImage / UserImages draw function
  @param  Index - position on uSD of image
  @param  uix - x position of widget
  @param  uiy - y position of widgte
  @param  uiw - width of widget
  @param  uih - height of widget
  @param  frame - number of frame in widget
  @param  bar - used in spectrum widgets
  @param  images - if single image or image set
  @param  cdv - not really used
  @note clipping and out of bounds now handled by WrGRAMs functions
*/
/****************************************************************************/
void gfx4desp32::DrawWidget(uint32_t Index, int16_t uix, int16_t uiy,
    int16_t uiw, int16_t uih, uint16_t frame,
    int16_t bar, bool images, byte cdv) {
    if (bar != 0)
        uix = uix + bar;
    if ((GCItype == GCI_SYSTEM_USD && (!userImag)) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    byte ofst = 6;
    if (images)
        ofst = 8;

    int mul = 2;
    uint32_t isize;

    uint32_t pos;
    if (cdv == 8)
        mul = 1;
    isize = (uiw * uih) << (mul - 1);
    int x1 = uix + uiw - 1;
    int y1 = uiy + uih - 1;
    usePushColors = (DisplayType == DISP_INTERFACE_RGB) && (uix >= clipx1) &&
        (uiy >= clipy1) && (x1 <= clipx2) && (y1 <= clipy2) &&
        (!transalpha) && (!WriteFBonly) && (frame_buffer == 0);
    SetGRAM(uix, uiy, x1, y1);
    pos = (isize * frame);
    //GCIreadToBuff(Index + ofst + pos, isize);
    if (GCItype == GCI_SYSTEM_USD) {
        GCIreadToBuff(Index + ofst + pos, isize);
        if (usePushColors && frame_buffer == 0) {
            pushColors(psRAMbuffer1, isize >> 1);
        }
        else {
            WrGRAMs(psRAMbuffer1, isize >> 1);
        }
    }
    else {
        if (usePushColors && frame_buffer == 0) {
            pushColors(GCIarray + Index + ofst + pos, isize >> 1);
        }
        else {
            WrGRAMs(GCIarray + Index + ofst + pos, isize >> 1);
        }
    }
}

void gfx4desp32::UserImages2image(int16_t ui, int16_t val, int16_t range,
    bool orientation, int16_t gap1,
    int16_t gap2) {
    if ((GCItype == GCI_SYSTEM_USD && (!userImag)) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    if (val > range)
        val = range;
    if (val < 0)
        val = 0;

    int16_t pixelRange = 0;
    int16_t w2i = tuiw[ui];
    int16_t h2i = tuih[ui];
    int16_t changePoint = 0;
    float inc = 0;
    if (orientation == HORIZONTAL) {
        pixelRange = w2i - (gap1 + gap2);
        inc = (float)pixelRange / (float)range;
        changePoint = (int)(inc * (float)val) + gap1;
        LastLinearPointerPosition = (int16_t)changePoint;
        if (changePoint > 0)
            UserImagesDR(ui, 1, 0, 0, changePoint, h2i);
        if ((w2i - changePoint - 1) > 0)
            UserImagesDR(ui, 0, changePoint, 0, w2i - changePoint - 1, h2i);
    }
    if (orientation == VERTICAL) {
        pixelRange = h2i - (gap1 + gap2);
        val = range - val;
        inc = (float)pixelRange / (float)range;
        changePoint = (int)(inc * (float)val) + gap1;
        LastLinearPointerPosition = (int16_t)changePoint;
        if (changePoint > 0)
            UserImagesDR(ui, 0, 0, 0, w2i, changePoint);
        if ((h2i - changePoint - 1) > 0)
            UserImagesDR(ui, 1, 0, changePoint, w2i, h2i - changePoint - 1);
    }
}

void gfx4desp32::UserImages3image(int16_t ui, int16_t val, int16_t range,
    bool orientation, int16_t gap1, int16_t gap2,
    int32_t tc) {
    if ((GCItype == GCI_SYSTEM_USD && (!userImag)) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    if (val > range)
        val = range;
    if (val < 0)
        val = 0;
    int16_t ui2 = ui + 1;

    int pixelRange;
    uint32_t w = tuiw[ui];
    uint32_t h = tuih[ui];
    uint32_t oh = h;
    uint32_t kw = tuiw[ui2];
    uint32_t kh = tuih[ui2];
    uint32_t changePoint;
    uint32_t isize = w * h;
    uint32_t pos;
    uint32_t posk;
    uint8_t tc1 = tc >> 8;
    uint8_t tc2 = tc & 0xff;
    float inc;
    uint32_t tempos = (isize << 1);
    int x1 = tuix[ui] + w - 1;
    int y1 = tuiy[ui] + h - 1;
    usePushColors = (DisplayType == DISP_INTERFACE_RGB) && (tuix[ui] >= clipx1) &&
        (tuiy[ui] >= clipy1) && (x1 <= clipx2) && (y1 <= clipy2) &&
        (!transalpha) && (!WriteFBonly) && (frame_buffer == 0);
    setGRAM(tuix[ui], tuiy[ui], x1, y1);
    posk = tuiIndex[ui2] + 6;
    if (orientation == HORIZONTAL) {
        pixelRange = w - (gap1 + gap2);
        inc = (float)pixelRange / (float)range;
        changePoint = (int)(inc * (float)val) + gap1;
        LastLinearPointerPosition = (int16_t)changePoint;
        pos = tuiIndex[ui] + 8;
        uint32_t pc;
        uint32_t p = 0;
        pc = pos + (isize << 1);
        GCIseek(pc);
        while (h--) {
            GCIreadToBuf(p, changePoint << 1);
            GCIseek(pc - (isize << 1) + (changePoint << 1));
            GCIreadToBuf(p + (changePoint << 1), (w - changePoint) << 1);
            pc += (w << 1);
            p += (w << 1);
            GCIseek(pc);
        }
        GCIseek(posk);
        p = (((oh - kh) >> 1) * (w << 1)) + (changePoint << 1) - (kw & 0xfffe);
        while (kh--) {
            if (tc == -1) {
                GCIreadToBuf(p, kw << 1);
            }
            else {
                GCIreadToBuf(tempos, kw << 1);
                for (int n = 0; n < kw; n++) {
                    if (psRAMbuffer1[tempos + (n << 1)] != tc1 &&
                        psRAMbuffer1[tempos + (n << 1) + 1] != tc2) {
                        psRAMbuffer1[p + (n << 1)] = psRAMbuffer1[tempos + (n << 1)];
                        psRAMbuffer1[p + (n << 1) + 1] =
                            psRAMbuffer1[tempos + (n << 1) + 1];
                    }
                }
            }
            p += (w << 1);
        }
        if (usePushColors && frame_buffer == 0) {
            pushColors(psRAMbuffer1, isize);
        }
        else {
            WrGRAMs(psRAMbuffer1, isize);
        }
    }
    if (orientation == VERTICAL) {
        pixelRange = h - (gap1 + gap2);
        val = range - val;
        inc = (float)pixelRange / (float)range;
        changePoint = (inc * (float)val) + gap1;
        LastLinearPointerPosition = (int16_t)changePoint;
        pos = tuiIndex[ui] + 8;
        uint32_t pc;
        uint32_t p = 0;
        pc = pos;
        GCIseek(pc);
        GCIreadToBuf(p, changePoint * (w << 1));
        GCIseek(pc + (isize << 1) + (changePoint * (w << 1)));
        GCIreadToBuf(p + (changePoint * (w << 1)), ((oh - changePoint) * (w << 1)));
        GCIseek(posk);
        p = ((changePoint - (kh >> 1)) * (w << 1)) + ((w - kw) & 0xfffe);

        while (kh--) {
            if (tc == -1) {
                GCIreadToBuf(p, kw << 1);
            }
            else {
                GCIreadToBuf(tempos, kw << 1);
                for (int n = 0; n < kw; n++) {
                    if (psRAMbuffer1[tempos + (n << 1)] != tc1 &&
                        psRAMbuffer1[tempos + (n << 1) + 1] != tc2) {
                        psRAMbuffer1[p + (n << 1)] = psRAMbuffer1[tempos + (n << 1)];
                        psRAMbuffer1[p + (n << 1) + 1] =
                            psRAMbuffer1[tempos + (n << 1) + 1];
                    }
                }
            }
            p += (w << 1);
        }
        if (usePushColors && frame_buffer == 0) {
            pushColors(psRAMbuffer1, isize);
        }
        else {
            WrGRAMs(psRAMbuffer1, isize);
        }
    }
}

/****************************************************************************/
/*!
  @brief  get the x or y value of the change position of 2 & 3 image widgets
  @param  none
  @note   returns x value if horizontal or y value if vertical.
*/
/****************************************************************************/
int16_t gfx4desp32::getLastPointerPos() { return LastLinearPointerPosition; }

/****************************************************************************/
/*!
  @brief  Draw widget from GCI file. UserImage / UserImages draw function
  @param  Fname - filename(String) of GCI format image file
  @note clipping and out of bounds now handled by WrGRAMs functions
*/
/****************************************************************************/
void gfx4desp32::DrawImageFile(String Fname) {
#ifndef USE_LITTLEFS_FILE_SYSTEM
#ifdef USE_SDMMC_FILE_SYSTEM
    dataFile = SD_MMC.open((char*)Fname.c_str());
#else
    dataFile = uSD.open((char*)Fname.c_str());
#endif
#else
    Fname = "/" + Fname;
    dataFile = LittleFS.open((char*)Fname.c_str(), "r");
#endif
    if (!dataFile)
        return;
    byte ofst = 6;
    int uix = cursor_x;
    int uiy = cursor_y;
    int uiw = (DATAread() << 8) + DATAread();
    int uih = (DATAread() << 8) + DATAread();
    uint32_t isize;
    isize = (uiw * uih) << 1;
    SetGRAM(uix, uiy, uix + uiw - 1, uiy + uih - 1);
    DATAreadToBuff(ofst, isize);
    WrGRAMs(psRAMbuffer1, isize >> 1);
    dataFile.close();
}

/****************************************************************************/
/*!
  @brief  Draw widget from GCI file. UserImage / UserImages draw function
  @param  Fname - filename(const char) of GCI format image file
  @note clipping and out of bounds now handled by WrGRAMs functions
*/
/****************************************************************************/
void gfx4desp32::DrawImageFile(const char* Fname) {
#ifndef USE_LITTLEFS_FILE_SYSTEM
#ifdef USE_SDMMC_FILE_SYSTEM
    dataFile = SD_MMC.open(Fname);
#else
    dataFile = uSD.open(Fname);
#endif
#else
    String temp = "/" + String(Fname);
    dataFile = LittleFS.open((char*)temp.c_str(), "r");
#endif
    if (!dataFile)
        return;
    byte ofst = 6;
    int uix = cursor_x;
    int uiy = cursor_y;
    int uiw = (DATAread() << 8) + DATAread();
    int uih = (DATAread() << 8) + DATAread();
    uint32_t isize;
    isize = (uiw * uih) << 1;
    SetGRAM(uix, uiy, uix + uiw - 1, uiy + uih - 1);
    DATAreadToBuff(ofst, isize);
    WrGRAMs(psRAMbuffer1, isize >> 1);
    dataFile.close();
}

/****************************************************************************/
/*!
  @brief  Draw widget from GCI array. (UserImage)
  @param  ImageArray array of GCI format image file
  @note clipping and out of bounds now handled by WrGRAMs functions
*/
/****************************************************************************/
void gfx4desp32::DrawImageArray(uint16_t* ImageArray) {
    int uix = cursor_x;
    int uiy = cursor_y;
    int uiw = (int)ImageArray[0];
    int uih = (int)ImageArray[1];
    uint32_t isize;
    isize = (uiw * uih);
    SetGRAM(uix, uiy, uix + uiw - 1, uiy + uih - 1);
    WrGRAMs(ImageArray + 3, isize);
}

/****************************************************************************/
/*!
  @brief  Draw widget from GCI array. (UserImage)
  @param  ImageArray array of GCI format image file
  @note clipping and out of bounds now handled by WrGRAMs functions
*/
/****************************************************************************/
void gfx4desp32::DrawImageArray(uint8_t* ImageArray) {
    int uix = cursor_x;
    int uiy = cursor_y;
    int uiw = ((int)ImageArray[0] << 8) + (int)ImageArray[1];
    int uih = ((int)ImageArray[2] << 8) + (int)ImageArray[3];
    uint32_t isize;
    isize = (uiw * uih) << 1;
    SetGRAM(uix, uiy, uix + uiw - 1, uiy + uih - 1);
    WrGRAMs(ImageArray + 6, isize);
}

/****************************************************************************/
/*!
  @brief  Draw single Userimage
  @param  ui - UserImage ID
  @note pre - drawWidget function
*/
/****************************************************************************/
void gfx4desp32::UserImage(uint16_t ui) { UserImage(ui, 0x7fff, 0x7fff); }


/****************************************************************************/
/*!
  @brief  Draw single Userimage in alternative location
  @param  ui - UserImage ID
  @param  altx - Alternative x position in pixels
  @param  alty - Alternative y position in pixels
  @note pre - drawWidget function
*/
/****************************************************************************/
void gfx4desp32::UserImage(uint16_t ui, int altx, int alty) {
    boolean setemp = sEnable;
    ScrollEnable(false);
    if (altx == 0x7fff && alty == 0x7fff) {
        DrawWidget(tuiIndex[ui], tuix[ui], tuiy[ui], tuiw[ui], tuih[ui], 0, 0,
            false, cdv[ui]);
    }
    else {
        DrawWidget(tuiIndex[ui], altx, alty, tuiw[ui], tuih[ui], 0, 0, false,
            cdv[ui]);
    }
    ScrollEnable(setemp);
}


/****************************************************************************/
/*!
  @brief  Draw frame from UserImages set
  @param  uisnb - UserImage ID
  @param  framenb - frame number
  @note pre - drawWidget function
*/
/****************************************************************************/
void gfx4desp32::UserImages(uint16_t uisnb, int16_t framenb) {
    tuiImageIndex[uisnb] = framenb;
    boolean setemp = sEnable;
    ScrollEnable(false);
    if (framenb > (gciobjframes[uisnb] - 1) || framenb < 0) {
        outofrange(tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb]);
    }
    else {
        DrawWidget(tuiIndex[uisnb], tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb], framenb, 0,
            true, cdv[uisnb]);
    }
    ScrollEnable(setemp);
}

void gfx4desp32::imageShow(uint16_t uisnb) {
    boolean setemp = sEnable;
    ScrollEnable(false);
    if (gciobjframes[uisnb] > 0) {
        DrawWidget(tuiIndex[uisnb], tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb], tuiImageIndex[uisnb], 0,
            true, cdv[uisnb]);
    }
    else {
        DrawWidget(tuiIndex[uisnb], tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb], 0, 0,
            false, cdv[uisnb]);
    }
    ScrollEnable(setemp);
}


/****************************************************************************/
/*!
  @brief  Draw frame from UserImages set in alternative location
  @param  uis - UserImage ID
  @param  frame - frame number
  @param  offset - used for spectrum widgets
  @param  altx - alternative x position in pixels
  @param  alty - alternative y position in pixels
  @note pre - drawWidget function
*/
/****************************************************************************/
void gfx4desp32::UserImages(uint16_t uis, int16_t frame, int offset,
    int16_t altx, int16_t alty) {
    tuiImageIndex[uis] = frame;
    boolean setemp = sEnable;
    ScrollEnable(false);
    if (frame > (gciobjframes[uis] - 1) || frame < 0) {
        outofrange(altx, alty, tuiw[uis], tuih[uis]);
    }
    else {
        DrawWidget(tuiIndex[uis], altx, alty, tuiw[uis], tuih[uis], frame, offset,
            true, cdv[uis]);
    }
    ScrollEnable(setemp);
}

/****************************************************************************/
/*!
  @brief  Draw graphic box and red cross to signal widget out of range
  @param  euix - x position in pixels
  @param  euiy - y position in pixels
  @param  euiw - width in pixels
  @param  euih - height in pixels
  @note pre - drawWidget function
*/
/****************************************************************************/
void gfx4desp32::outofrange(int16_t euix, int16_t euiy, int16_t euiw,
    int16_t euih) {
    if (euix >= width || euiy >= height)
        return;
    if ((euix + euiw - 1) < 0 || (euiy + euih - 1) < 0)
        return;
    int cuix = euix;
    int cuiy = euiy;
    int cuiw = euiw;
    int cuih = euih;
    if (euix < 0) {
        cuix = 0;
        cuiw = euiw + euix;
    }
    if (euiy < 0) {
        cuiy = 0;
        cuih = euih + euiy;
    }
    if ((euix + euiw - 1) >= width)
        cuiw = euiw - ((euix + euiw - 1) - width) - 1;
    if ((euiy + euih - 1) >= height)
        cuih = euih - ((euiy + euih - 1) - height) - 1;
    RectangleFilled(cuix, cuiy, cuix + cuiw - 1, cuiy + cuih - 1, BLACK);
    Rectangle(cuix + 1, cuiy + 1, cuix + cuiw - 2, cuiy + cuih - 2, RED);
    StartWrite();
    Line(cuix + 1, cuiy + 1, cuix + cuiw - 2, cuiy + cuih - 2, RED);
    Line(cuix + cuiw - 2, cuiy + 1, cuix + 1, cuiy + cuih - 2, RED);
    EndWrite();
}

/****************************************************************************/
/*!
  @brief  Draw Led digits value
  @param  see manual
  @note accepts signed values upto 64 bits
*/
/****************************************************************************/
void gfx4desp32::LedDigitsDisplaySigned(int64_t newval, uint16_t index,
    int16_t Digits, int16_t MinDigits,
    int16_t WidthDigit,
    int16_t LeadingBlanks) {
    LedDigitsDisplaySigned(newval, index, Digits, MinDigits, WidthDigit,
        LeadingBlanks, 0x7fff, 0x7fff);
}

void gfx4desp32::LedDigitsDisplaySigned(int64_t newval, uint16_t index,
    int16_t Digits, int16_t MinDigits,
    int16_t WidthDigit,
    int16_t LeadingBlanks, int16_t altx,
    int16_t alty) {
    int16_t i, m, lstb, nv, digita[7];
    int leftpos = 0;
    nv = newval;
    lstb = 1;
    for (i = Digits; i > 0; i--) {
        m = nv % 10;
        if (LeadingBlanks && (i <= Digits - MinDigits)) {
            if (nv == 0) {
                m = 10;
                if (lstb == 1)
                    lstb = i;
            }
        }
        digita[i] = abs(m);
        nv /= 10;
    }
    if (newval < 0) {
        digita[lstb] = 11;
    }
    for (i = 1; i <= Digits; i++) {
        if (altx == 0x7fff && alty == 0x7fff) {
            UserImages(index, digita[i], leftpos);
        }
        else {
            UserImages(index, digita[i], leftpos, altx, alty);
        }
        leftpos += WidthDigit;
    }
}

/****************************************************************************/
/*!
  @brief  Draw Led digits value
  @param  see manual
  @note accepts un-signed values upto 64 bits
*/
/****************************************************************************/
void gfx4desp32::LedDigitsDisplay(int64_t newval, uint16_t index,
    int16_t Digits, int16_t MinDigits,
    int16_t WidthDigit, int16_t LeadingBlanks) {
    LedDigitsDisplay(newval, index, Digits, MinDigits, WidthDigit, LeadingBlanks,
        0x7fff, 0x7fff);
}

void gfx4desp32::LedDigitsDisplay(int64_t newval, uint16_t index,
    int16_t Digits, int16_t MinDigits,
    int16_t WidthDigit, int16_t LeadingBlanks,
    int16_t altx, int16_t alty) {
    int16_t i, k, lb;
    int64_t l;
    l = 1;
    for (i = 1; i < Digits; i++)
        l *= 10;
    lb = LeadingBlanks;
    for (i = 0; i < Digits; i++) {
        k = newval / l;
        newval -= k * l;
        if (lb && (i < Digits - MinDigits)) {
            if (k == 0)
                k = 10;
            else
                lb = 0;
        }
        l /= 10;
        if (altx == 0x7fff && alty == 0x7fff) {
            UserImages(index, k, i * WidthDigit);
        }
        else {
            UserImages(index, k, i * WidthDigit, altx, alty);
        }
    }
}

/****************************************************************************/
/*!
  @brief  Draw frame from UserImages set in new location
  @param  uisnb - UserImages widget number
  @param  framenb - selected frame
  @param  newx - new x position in pixels
  @param  newy - new y position in pixels
  @note maybe duplicate function
*/
/****************************************************************************/
void gfx4desp32::UserImages(uint16_t uisnb, int16_t framenb, int16_t newx,
    int16_t newy) {
    tuiImageIndex[uisnb] = framenb;
    uimage = false;
    boolean setemp = sEnable;
    ScrollEnable(false);
    if (framenb > (gciobjframes[uisnb] - 1) || framenb < 0) {
        outofrange(tuix[uisnb], tuiy[uisnb], tuiw[uisnb], tuih[uisnb]);
    }
    else {
        DrawWidget(tuiIndex[uisnb], newx, newy, tuiw[uisnb], tuih[uisnb], framenb,
            0, true, cdv[uisnb]);
    }
    ScrollEnable(setemp);
}


/****************************************************************************/
/*!
  @brief  Draw frame from UserImages set in default location
  @param  uis - Userimages widget number
  @param  frame - selected frame
  @param  offset - used for spectrum widgets
  @note pre drawWidget function
*/
/****************************************************************************/
void gfx4desp32::UserImages(uint16_t uis, int16_t frame, int offset) {
    tuiImageIndex[uis] = frame;
    boolean setemp = sEnable;
    ScrollEnable(false);
    if (frame > (gciobjframes[uis] - 1) || frame < 0) {
        outofrange(tuix[uis], tuiy[uis], tuiw[uis], tuih[uis]);
    }
    else {
        DrawWidget(tuiIndex[uis], tuix[uis], tuiy[uis], tuiw[uis], tuih[uis], frame,
            offset, true, cdv[uis]);
    }
    ScrollEnable(setemp);
}


/****************************************************************************/
/*!
  @brief  Draws rectangular segment of frame from UserImages set in default
  location
  @param  uino - Userimages widget number
  @param  frames - selected frame
  @param  uxpos - position of start of segment from top left corner of widget
  @param  uypos - position of start of segment from top left corner of widget
  @param  uwidth - width of segment
  @param  uheight - height of segment
*/
/****************************************************************************/
void gfx4desp32::UserImagesDR(uint16_t uino, int frames, int16_t uxpos,
    int16_t uypos, int16_t uwidth, int16_t uheight) {
    if (uxpos >= width || uypos >= height || uxpos < 0 || uypos < 0)
        return;
    if (uwidth < 1 || uheight < 1)
        return;
    tuiImageIndex[uino] = frames;
    uint32_t bgoff = 0;
    boolean setemp = sEnable;
    ScrollEnable(false);
    if (uxpos + uwidth > tuiw[uino])
        uwidth = tuiw[uino] - uxpos;
    if (uypos + uheight > tuih[uino])
        uheight = tuih[uino] - uypos;
    if ((GCItype == GCI_SYSTEM_USD && !userImag) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    uint32_t isize = tuiw[uino] * tuih[uino];
    uint32_t isize2 = uwidth * uheight;
    int16_t wuid = uwidth;
    int16_t huid = uheight;
    uint32_t pos;
    uint32_t uoff;
    int16_t x0 = 0;
    int16_t y0 = 0;
    int16_t x1 = 0;
    int16_t y1 = 0;
    pos = (isize * frames) << 1;
    uoff = ((uypos * tuiw[uino]) + uxpos) << 1;
    bgoff = tuiIndex[uino] + 8 + pos + uoff + 0;
    if (frames > (gciobjframes[uino] - 1) || frames < 0) {
        outofrange(tuix[uino] + uxpos, tuiy[uino] + uypos, uwidth, uheight);
        ScrollEnable(setemp);
        return;
    }
    x0 = tuix[uino] + uxpos;
    y0 = tuiy[uino] + uypos;
    x1 = tuix[uino] + uxpos + uwidth - 1;
    y1 = tuiy[uino] + uypos + huid - 1;
    usePushColors = (DisplayType == DISP_INTERFACE_RGB) && (x0 >= clipx1) &&
        (y0 >= clipy1) && (x1 <= clipx2) && (y1 <= clipy2) &&
        (!transalpha) && (!WriteFBonly) && (frame_buffer == 0);
    SetGRAM(x0, y0, x1, y1);
    GCIseek(bgoff);
    uint32_t p = 0;
    while (huid--) {
        if (GCItype == GCI_SYSTEM_USD) {
            GCIreadToBuf(p, wuid << 1);
            p += (wuid << 1);
            bgoff += (tuiw[uino] << 1);
            GCIseek(bgoff);

        }
        else {
            if (usePushColors && frame_buffer == 0) {
                pushColors(GCIarray + bgoff, wuid);
            }
            else {
                WrGRAMs(GCIarray + bgoff, wuid);
            }
            bgoff += (tuiw[uino] << 1);
        }
    }
    if (GCItype == GCI_SYSTEM_USD) {
        if (usePushColors && frame_buffer == 0) {
            pushColors(psRAMbuffer1, isize2);
        }
        else {
            WrGRAMs(psRAMbuffer1, isize2);
        }
    }
    ScrollEnable(setemp);
}

/****************************************************************************/
/*!
  @brief  compatible Set GRAM window ready for wrGRAM/s functions.
  @param  x1 - left X position in pixels
  @param  y1 - top Y position in pixels
  @param  w - width in pixels
  @param  h - height in pixels
  @note Uses SetGRAM / setGRAM
*/
/****************************************************************************/
void gfx4desp32::setAddrWindow(int16_t x1, int16_t y1, int16_t w, int16_t h) {
    SetGRAM(x1, y1, x1 + w - 1, y1 + h - 1);
}

/****************************************************************************/
/*!
  @brief  Draws UserImage at current text cursor position
  @param  ui - UserImage widget ID number
  @note Draws image in the same manner as drawing text. If Scroll is enabled
    then image will also scroll while being drawn.
*/
/****************************************************************************/
void gfx4desp32::PrintImage(uint16_t ui) {
    if (cursor_x > (width - 1))
        return;
    boolean tempnl = false;
    if (nl) {
        cursor_x = 0;
        tempnl = true;
        newLine(lastfsh, textsizeht, 0);
    }
    if (cursor_y > (height - 1))
        return;
    if ((GCItype == GCI_SYSTEM_USD && !userImag) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    uint16_t iwidth = tuiw[ui];
    uint16_t iheight = tuih[ui];
    uint8_t mul = cdv[ui] / 8;
    uint32_t pos = tuiIndex[ui] + 6;
    GCIseek(pos);
    uint16_t ichunk = iwidth << (mul - 1);
    uint8_t buf[width << (mul - 1)];
    if (sEnable == false) {
        if (((cursor_y + iheight) - 1) > height - 1)
            iheight = iheight - ((cursor_y + iheight) - height);
    }
    boolean off = false;
    int cuiw = iwidth;
    if ((cursor_x + iwidth - 1) >= width) {
        cuiw = iwidth - ((cursor_x + iwidth - 1) - width) - 1;
        off = true;
    }
    for (int idraw = 0; idraw < iheight; idraw++) {
        nl = true;
        newLine(1, 1, cursor_x);
        if ((cursor_y - 1) < 0) {
            setGRAM(cursor_x, cursor_y + height - 1, cursor_x + cuiw - 1,
                cursor_y + height - 1);
        }
        else {
            setGRAM(cursor_x, cursor_y - 1, cursor_x + cuiw - 1, cursor_y - 1);
        }
        if (off) {
            GCIread(buf, cuiw << (mul - 1));
            WrGRAMs(buf, cuiw);
            pos = pos + (iwidth << (mul - 1));
            GCIseek(pos);
        }
        else {
            GCIread(buf, ichunk);
            WrGRAMs(buf, ichunk >> 1);
        }
    }
    if (tempnl) {
        nl = true;
        lastfsh = 1;
    }
}

/****************************************************************************/
/*!
  @brief  Draws rectangular segment of UserImage set in Alternate location
  @param  ui - Userimage widget number
  @param  uxpos - position of start of segment from top left corner of widget
  @param  uypos - position of start of segment from top left corner of widget
  @param  uwidth - width of segment
  @param  uheight - height of segment
  @param  uix - new x position
  @param  uiy - new y position
*/
/****************************************************************************/
void gfx4desp32::UserImageDR(uint16_t ui, int16_t uxpos, int16_t uypos,
    int16_t uwidth, int16_t uheight, int16_t uix,
    int16_t uiy) {
    if (uix > width || uiy > height || uix < 0 || uiy < 0)
        return;
    if ((uix + uwidth - 1) < 0 || (uiy + uheight - 1) < 0)
        return;
    if ((uix + uwidth) > width || (uiy + uheight) > height)
        return;
    ScrollEnable(false);
    uint32_t bgoff;
    if ((GCItype == GCI_SYSTEM_USD && !userImag) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    if (uxpos + uwidth > tuiw[ui])
        uwidth = tuiw[ui] - uxpos;
    if (uypos + uheight > tuih[ui])
        uheight = tuih[ui] - uypos;
    uint32_t isize2 = uwidth * uheight;
    uint32_t p = 0;
    uint32_t uoff;
    uoff = ((uypos * tuiw[ui]) + uxpos) << 1;
    bgoff = tuiIndex[ui] + 6 + uoff + 0;
    GCIseek(bgoff);
    uint32_t ichunk = isize2 << 1;
    uint16_t steps;
    int bufsize = uwidth << 1;
    SetGRAM(uix, uiy, uix + uwidth - 1, uiy + uheight - 1);
    uint32_t fgap = tuiw[ui] << 1;
    steps = ichunk / bufsize;
    while (steps--) {
        if (GCItype == GCI_SYSTEM_USD) {
            GCIreadToBuf(p, bufsize);
            p += bufsize;
            bgoff = bgoff + fgap;
            GCIseek(bgoff);
        }
        else {
            WrGRAMs(GCIarray + bgoff, uwidth);
            bgoff += fgap;
        }
    }
    if (GCItype == GCI_SYSTEM_USD)
        WrGRAMs(psRAMbuffer1, p >> 1);
}

void gfx4desp32::UserImageDRcache(uint16_t ui, int16_t uxpos, int16_t uypos,
    int16_t uwidth, int16_t uheight, int16_t uix,
    int16_t uiy) {
    if (uix > width || uiy > height || uix < 0 || uiy < 0)
        return;
    if ((uix + uwidth - 1) < 0 || (uiy + uheight - 1) < 0)
        return;
    if ((uix + uwidth) > width || (uiy + uheight) > height)
        return;
    ScrollEnable(false);
    uint32_t bgoff;
    if ((GCItype == GCI_SYSTEM_USD && !userImag) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    if (!cache_Enabled) {
        AllocateDRcache(DRcache);
        //psRAMbuffer2 = (uint8_t*)ps_malloc(DRcache);
        //cache_Enabled = true;
    }
    int32_t bwidth = tuiw[ui] << 1;
    uint32_t slen;
    if (!cached || lastui != ui) {
        cache_Start = tuiIndex[ui] + (bwidth * uypos) + 6;
        slen = bwidth * uheight;
        GCIreadToBuff2(cache_Start, slen);
        cached = true;
    }
    if (uxpos + uwidth > tuiw[ui])
        uwidth = tuiw[ui] - uxpos;
    if (uypos + uheight > tuih[ui])
        uheight = tuih[ui] - uypos;

    uint32_t isize2 = uwidth * uheight;
    uint32_t p = 0;
    uint32_t uoff;
    uint32_t bchunk = bwidth * uheight;
    uoff = ((uypos * tuiw[ui]) + uxpos) << 1;
    bgoff = tuiIndex[ui] + 6 + uoff + 0;
    GCIseek(bgoff);
    uint32_t ichunk = isize2 << 1;
    uint16_t steps;
    int bufsize = uwidth << 1;
    int x1 = uix + uwidth - 1;
    int y1 = uiy + uheight - 1;
    usePushColors = (DisplayType == DISP_INTERFACE_RGB) && (uix >= clipx1) &&
        (uiy >= clipy1) && (x1 <= clipx2) && (y1 <= clipy2) &&
        (!transalpha) && (!WriteFBonly) && (frame_buffer == 0);
    SetGRAM(uix, uiy, x1, y1);
    uint32_t fgap = tuiw[ui] << 1;
    steps = ichunk / bufsize;
    int32_t bsize;
    int32_t dist = (int32_t)bgoff - (int32_t)cache_Start;
    if (abs(dist) > bwidth) {
        int32_t lines = dist / bwidth;
        bsize = abs(lines) * bwidth;
        if (abs(lines) > uheight) {
            cache_Start = tuiIndex[ui] + (bwidth * uypos) + 6;
            slen = bwidth * uheight;
            GCIreadToBuff2(cache_Start, slen);
        }
        else {
            if (lines < 0) {
                memmove(psRAMbuffer2 + bsize, psRAMbuffer2, bchunk - bsize);
                cache_Start -= bsize;
                GCIreadToBuff2(cache_Start, bsize);
            }
            if (lines > 0) {
                memmove(psRAMbuffer2, psRAMbuffer2 + bsize, bchunk - bsize);
                cache_Start += bsize;
                GCIreadToBuff2(cache_Start + (bchunk - bsize), bchunk - bsize, bsize);
            }
        }
    }
    while (steps--) {
        memcpy(psRAMbuffer1 + p, psRAMbuffer2 + bgoff - cache_Start, bufsize);
        p += bufsize;
        bgoff = bgoff + fgap;
    }
    if (usePushColors && frame_buffer == 0) {
        pushColors(psRAMbuffer1, p >> 1);
    }
    else {
        WrGRAMs(psRAMbuffer1, p >> 1);
    }
    lastui = ui;
}

/****************************************************************************/
/*!
  @brief  calculate x & y co-ordinate using given angle and length using current
    x - y origin (MoveTo)
  @param  angle - Degrees
  @param  length - Pixels from x - y origin
  @param  oxy - prefined 2 int array containing x at location 0 and y at
  location 1
*/
/****************************************************************************/
void gfx4desp32::Orbit(int angle, int lngth, int* oxy) {
    float sx = cos((angle - 90) * 0.0174532925);
    float sy = sin((angle - 90) * 0.0174532925);
    oxy[0] = (int)(sx * lngth + cursor_x);
    oxy[1] = (int)(sy * lngth + cursor_y);
}

/****************************************************************************/
/*!
  @brief  calculate float x & y co-ordinate using given angle and length using current
    x - y origin (MoveTo)
  @param  angle - Degrees
  @param  length - Pixels from x - y origin
  @param  oxy - prefined 2 float array containing x at location 0 and y at
  location 1
*/
/****************************************************************************/
void gfx4desp32::Orbit(float angle, float lngth, float* oxy) {
    float sx = cos((angle - 90) * 0.0174532925);
    float sy = sin((angle - 90) * 0.0174532925);
    oxy[0] = (sx * lngth + cursor_x);
    oxy[1] = (sy * lngth + cursor_y);
}

/****************************************************************************/
/*!
  @brief  convert  R, G, B to RGB565
  @param  rc - RED
  @param  gc - GREEN
  @param  bc - BLUE
  @note returns 16bit RGB565 colour
*/
/****************************************************************************/
uint16_t gfx4desp32::RGBto565(uint8_t rc, uint8_t gc, uint8_t bc) {
    return (((rc & 0xF8) << 8) | ((gc & 0xFC) << 3) | (bc >> 3));
}

/****************************************************************************/
/*!
  @brief  various colour conversion functions
  @note see manual
*/
/****************************************************************************/
uint32_t gfx4desp32::bevelColor(uint16_t colorb) {
    return HighlightColors(colorb, 18);
}

uint32_t gfx4desp32::HighlightColors(uint16_t colorh, int step) {
    c565toRGBs(colorh);
    RGB2HLS();
    uint8_t oldred = GFX4dESP32_RED;
    uint8_t oldgreen = GFX4dESP32_GREEN;
    uint8_t oldblue = GFX4dESP32_BLUE;
    uint8_t tl = l;
    uint8_t th = h;
    uint8_t ts = s;
    HLS2RGB(th, tl - step, ts);
    if (GFX4dESP32_RED > oldred) {
        GFX4dESP32_RED = 0;
    }
    if (GFX4dESP32_GREEN > oldgreen) {
        GFX4dESP32_GREEN = 0;
    }
    if (GFX4dESP32_BLUE > oldblue) {
        GFX4dESP32_BLUE = 0;
    }
    uint16_t _dark = RGBs2COL(GFX4dESP32_RED, GFX4dESP32_GREEN, GFX4dESP32_BLUE);
    HLS2RGB(th, tl + step, ts);
    uint16_t _light = RGBs2COL(GFX4dESP32_RED, GFX4dESP32_GREEN, GFX4dESP32_BLUE);
    uint32_t bevcol = (_dark << 16) + _light;
    return bevcol;
}

uint16_t gfx4desp32::RGBs2COL(uint8_t r, uint8_t g, uint8_t b) {
    return (b >> 2) | (g & 0x7E) << 4 | (r & 0x7c) << 9;
}

void gfx4desp32::RGB2HLS() {
    uint8_t cMax, cMin, Rdelta, Gdelta, Bdelta, cMpM, cMmM;
    if (GFX4dESP32_RED >= GFX4dESP32_GREEN) {
        cMax = GFX4dESP32_RED;
    }
    else {
        cMax = GFX4dESP32_GREEN;
    }
    if (GFX4dESP32_BLUE >= cMax) {
        cMax = GFX4dESP32_BLUE;
    }
    if (GFX4dESP32_RED <= GFX4dESP32_GREEN) {
        cMin = GFX4dESP32_RED;
    }
    else {
        cMin = GFX4dESP32_GREEN;
    }
    if (GFX4dESP32_BLUE <= cMin) {
        cMin = GFX4dESP32_BLUE;
    }
    cMpM = cMax + cMin;
    cMmM = cMax - cMin;
    l = ((cMpM * HLSMAX) + RGBMAX) / RGBMAXm2;
    if (cMax == cMin) {
        s = 0;
        h = UNDEFINED;
    }
    else {
        if (l <= (HLSMAX / 2)) {
            s = ((cMmM * HLSMAX) + (cMpM / 2)) / cMpM;
        }
        else {
            s = ((cMmM * HLSMAX) + ((RGBMAXm2 - cMpM) / 2)) / (RGBMAXm2 - cMpM);
        }
        Rdelta = (((cMax - GFX4dESP32_RED) * HLSMAXd6) + (cMmM / 2)) / cMmM;
        Gdelta = (((cMax - GFX4dESP32_GREEN) * HLSMAXd6) + (cMmM / 2)) / cMmM;
        Bdelta = (((cMax - GFX4dESP32_BLUE) * HLSMAXd6) + (cMmM / 2)) / cMmM;
        if (GFX4dESP32_RED == cMax) {
            h = Bdelta - Gdelta;
        }
        else if (GFX4dESP32_GREEN == cMax) {
            h = HLSMAXd3 + Rdelta - Bdelta;
        }
        else {
            h = HLSMAXm2d3 + Gdelta - Rdelta;
        }
        if (h < 0) {
            h += HLSMAX;
        }
        if (h > HLSMAX) {
            h -= HLSMAX;
        }
    }
}

void gfx4desp32::c565toRGBs(uint16_t i565) {
    GFX4dESP32_RED = (i565 & 0xF800) >> 9;
    GFX4dESP32_GREEN = (i565 & 0x07E0) >> 4;
    GFX4dESP32_BLUE = (i565 & 0x001F) << 2;
}

void gfx4desp32::HLS2RGB(int H, int L, int S) {
    uint8_t M1, M2;
    if (S == 0) {
        GFX4dESP32_RED = L;
        GFX4dESP32_GREEN = L;
        GFX4dESP32_BLUE = L;
    }
    else {
        if (L <= HLSMAXd2) {
            M2 = (L * (HLSMAX + S) + HLSMAXd2) / HLSMAX;
        }
        else {
            M2 = L + S - ((L * S + HLSMAXd2) / HLSMAX);
        }
        M1 = 2 * L - M2;
        if ((H > HLSMAX) || (H < 0))
            h = 0;
        GFX4dESP32_RED = hue_RGB(H + HLSMAXd3, M1, M2);
        GFX4dESP32_GREEN = hue_RGB(H, M1, M2);
        GFX4dESP32_BLUE = hue_RGB(H - HLSMAXd3, M1, M2);
    }
}

uint8_t gfx4desp32::hue_RGB(int Hin, int M1, int M2) {
    uint8_t Value;
    if (Hin < 0) {
        Hin += HLSMAX;
    }
    else if (Hin > HLSMAX) {
        Hin -= HLSMAX;
    }
    if (Hin < HLSMAXd6) {
        Value = M1 + ((M2 - M1) * Hin + HLSMAXd12) / HLSMAXd6;
    }
    else if (Hin < HLSMAXd2) {
        Value = M2;
    }
    else if (Hin < HLSMAXm2d3) {
        Value = M1 + ((M2 - M1) * (HLSMAXm2d3 - Hin) + HLSMAXd12) / HLSMAXd6;
    }
    else {
        Value = M1;
    }
    return Value;
}

void gfx4desp32::Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t colorb, String btext, int8_t tfont,
    uint16_t tcolor) {
    if (ButtonxInitial) {
        ButtonxInitial = false;
    }
    nl = false;
    bactive[hndl] = true;
    bposx[hndl] = x;
    bposy[hndl] = y;
    bposw[hndl] = w;
    bposh[hndl] = h;
    bposc[hndl] = colorb;
    drawButton(0, x, y, w, h, colorb, btext, tfont, 1, 1, tcolor);
}

void gfx4desp32::Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t colorb, String btext,
    const uint8_t* tfont, uint16_t tcolor,
    bool compressed) {
    if (ButtonxInitial) {
        ButtonxInitial = false;
    }
    nl = false;
    bactive[hndl] = true;
    bposx[hndl] = x;
    bposy[hndl] = y;
    bposw[hndl] = w;
    bposh[hndl] = h;
    bposc[hndl] = colorb;
    drawButton(0, x, y, w, h, colorb, btext, tfont, 1, 1, tcolor, compressed);
}

void gfx4desp32::Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t colorb, String btext,
    gfx4d_font tfont, uint16_t tcolor) {
    if (ButtonxInitial) {
        ButtonxInitial = false;
    }
    nl = false;
    bactive[hndl] = true;
    bposx[hndl] = x;
    bposy[hndl] = y;
    bposw[hndl] = w;
    bposh[hndl] = h;
    bposc[hndl] = colorb;
    drawButton(0, x, y, w, h, colorb, btext, tfont, 1, 1, tcolor);
}

void gfx4desp32::Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb,
    uint16_t tcolor, int8_t tfont, int8_t tfontsizeh,
    int8_t tfontsize, String btext) {
    uint8_t sl = btext.length();
    uint8_t fsww;
    uint8_t fshh;
    if (tfont == 1) {
        fsww = 6;
        fshh = 8;
    }
    else {
        fsww = 8;
        fshh = 16;
    }
    uint16_t sw = sl * fsww * tfontsize;
    uint16_t sh = fshh * tfontsizeh;
    drawButton(state, x, y, sw + (19 * tfontsize), sh + (9 * tfontsizeh), colorb,
        btext, tfont, tfontsize, tfontsizeh, tcolor);
}

void gfx4desp32::Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb,
    uint16_t tcolor, const uint8_t* tfont,
    int8_t tfontsizeh, int8_t tfontsize, String btext,
    bool compressed) {
    __tempFont(tfont, compressed);
    uint8_t fshh = fsh;
    uint16_t sw = strWidth(btext);
    uint16_t sh = fshh * tfontsizeh;
    __restoreFont();
    drawButton(state, x, y, sw + (19 * tfontsize), sh + (9 * tfontsizeh), colorb,
        btext, tfont, tfontsize, tfontsizeh, tcolor, compressed);
}

void gfx4desp32::Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb,
    uint16_t tcolor, gfx4d_font tfont, int8_t tfontsizeh,
    int8_t tfontsize, String btext) {
    __tempFont(tfont);
    uint8_t fshh = fsh;
    uint16_t sw = strWidth(btext);
    uint16_t sh = fshh * tfontsizeh;
    __restoreFont();
    drawButton(state, x, y, sw + (19 * tfontsize), sh + (9 * tfontsizeh), colorb,
        btext, tfont, tfontsize, tfontsizeh, tcolor);
}

void gfx4desp32::Slider(uint8_t state, int16_t x, int16_t y, int16_t r,
    int16_t b, uint16_t colorb, uint16_t colort,
    int16_t scale, int16_t value) {
    int w = r - x;
    int h = b - y;
    drawButton(state, x, y, w, h, colorb, "", 1, 1, 1, colorb);
    uint16_t thw;
    uint16_t thh = h - 4;
    uint16_t ra = w - 4;
    thw = ra / 10;
    if (thw < 5)
        thw = 5;
    float rs = (((float)ra - (float)thw) / (float)scale) * (float)value;
    int rsc = (int)rs;
    RectangleFilled(x + 2 + rsc, y + 2, x + 2 + rsc + thw - 1, y + 2 + thh - 1,
        colort);
    Hline(x + 2 + rsc, y + 2, thw, tlight);
    Vline(x + 2 + rsc, y + 2, thh, tlight);
    Hline(x + 2 + rsc, y + 1 + thh, thw, tdark);
    Vline(x + 2 + rsc + thw, y + 2, thh, tdark);
}

void gfx4desp32::drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t colorb, String btext,
    const uint8_t* tfont, int8_t tfontsize,
    int8_t tfontsizeht, uint16_t tcolor,
    bool compressed) {
    boolean twrap = wrap;
    boolean nlbckp = nl;
    nl = false;
    wrap = false;

    int8_t tfh;
    uint8_t fsizebckup = textsize;
    uint8_t fsizehbckup = textsizeht;
    textsize = tfontsize;
    textsizeht = tfontsizeht;
    uint16_t tcolorbckup = textcolor;
    uint16_t tcolorbgbckup = textbgcolor;
    uint16_t curxbckup = cursor_x;
    uint16_t curybckup = cursor_y;
    uint32_t tcol = bevelColor(colorb);
    uint16_t _dark = tcol >> 16;
    uint16_t _light = tcol & 0xffff;
    tdark = _dark;
    tlight = _light;
    if (bxStyle == 0) {
        RectangleFilled(x + 2, y + 2, (x + 2) + w - 4, (y + 2) + h - 4, colorb);
        if (updn == 0) {
            Hline(x, y, w, _light);
            Hline(x + 1, y + 1, w - 2, _light);
            Vline(x, y, h, _light);
            Vline(x + 1, y + 1, h - 2, _light);
            Hline(x, y + h - 1, w, _dark);
            Hline(x + 1, y + h - 2, w - 2, _dark);
            Vline(x + w - 1, y, h, _dark);
            Vline(x + w - 2, y + 1, h - 2, _dark);
        }
        if (updn == 1) {
            Hline(x, y, w, _dark);
            Hline(x + 1, y + 1, w - 2, _dark);
            Vline(x, y, h, _dark);
            Vline(x + 1, y + 1, h - 2, _dark);
            Hline(x, y + h - 1, w, _light);
            Hline(x + 1, y + h - 2, w - 2, _light);
            Vline(x + w - 1, y, h, _light);
            Vline(x + w - 2, y + 1, h - 2, _light);
        }
    }
    if (bxStyle > 0) {
        int nh, g1, g2, nw;
        nw = h / 10;
        if (nw == 0)
            nw = 1;
        if (bxStyle == 1) {
            nh = h / 6;
            g1 = -1;
            g2 = 0;
        }
        if (bxStyle > 1) {
            nh = h >> 1;
            g1 = 28;
            g2 = 0;
        }
        if (bxStyle == 3) {
            g1 = 48;
            g2 = 28;
        }
        if (updn == 0)
            gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
                GRADIENT_RAISED, g1, colorb, GRADIENT_RAISED, g2, 0);
        if (updn == 1)
            gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
                GRADIENT_RAISED, 0, colorb, GRADIENT_RAISED, g2, 0);
    }
    TextColor(tcolor, tcolor);
    __tempFont(tfont, compressed);
    tfh = fsh;
    uint8_t blen = btext.length();
    size_t strw = strWidth(btext);
    if (blen > 0) {
        MoveTo(((x + (w / 2) - ((strw * textsize) / 2)) + updn),
            ((y + (h / 2) - ((tfh * textsizeht) / 2)) + 1 + updn));
        print(btext);
    }
    TextColor(tcolorbckup, tcolorbgbckup);
    __restoreFont();
    textsize = fsizebckup;
    textsizeht = fsizehbckup;
    MoveTo(curxbckup, curybckup);
    nl = nlbckp;
    wrap = twrap;
}

void gfx4desp32::drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t colorb, String btext,
    gfx4d_font tfont, int8_t tfontsize,
    int8_t tfontsizeht, uint16_t tcolor) {
    boolean twrap = wrap;
    boolean nlbckp = nl;
    nl = false;
    wrap = false;
    int8_t tfh;
    uint8_t fsizebckup = textsize;
    uint8_t fsizehbckup = textsizeht;
    textsize = tfontsize;
    textsizeht = tfontsizeht;
    uint16_t tcolorbckup = textcolor;
    uint16_t tcolorbgbckup = textbgcolor;
    uint16_t curxbckup = cursor_x;
    uint16_t curybckup = cursor_y;
    uint32_t tcol = bevelColor(colorb);
    uint16_t _dark = tcol >> 16;
    uint16_t _light = tcol & 0xffff;
    tdark = _dark;
    tlight = _light;
    if (bxStyle == 0) {
        RectangleFilled(x + 2, y + 2, (x + 2) + w - 4, (y + 2) + h - 4, colorb);
        if (updn == 0) {
            Hline(x, y, w, _light);
            Hline(x + 1, y + 1, w - 2, _light);
            Vline(x, y, h, _light);
            Vline(x + 1, y + 1, h - 2, _light);
            Hline(x, y + h - 1, w, _dark);
            Hline(x + 1, y + h - 2, w - 2, _dark);
            Vline(x + w - 1, y, h, _dark);
            Vline(x + w - 2, y + 1, h - 2, _dark);
        }
        if (updn == 1) {
            Hline(x, y, w, _dark);
            Hline(x + 1, y + 1, w - 2, _dark);
            Vline(x, y, h, _dark);
            Vline(x + 1, y + 1, h - 2, _dark);
            Hline(x, y + h - 1, w, _light);
            Hline(x + 1, y + h - 2, w - 2, _light);
            Vline(x + w - 1, y, h, _light);
            Vline(x + w - 2, y + 1, h - 2, _light);
        }
    }
    if (bxStyle > 0) {
        int nh, g1, g2, nw;
        nw = h / 10;
        if (nw == 0)
            nw = 1;
        if (bxStyle == 1) {
            nh = h / 6;
            g1 = -1;
            g2 = 0;
        }
        if (bxStyle > 1) {
            nh = h >> 1;
            g1 = 28;
            g2 = 0;
        }
        if (bxStyle == 3) {
            g1 = 48;
            g2 = 28;
        }
        if (updn == 0)
            gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
                GRADIENT_RAISED, g1, colorb, GRADIENT_RAISED, g2, 0);
        if (updn == 1)
            gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
                GRADIENT_RAISED, 0, colorb, GRADIENT_RAISED, g2, 0);
    }
    TextColor(tcolor, tcolor);
    __tempFont(tfont);
    tfh = fsh;
    uint8_t blen = btext.length();
    size_t strw = strWidth(btext);
    if (blen > 0) {
        MoveTo(((x + (w / 2) - ((strw * textsize) / 2)) + updn),
            ((y + (h / 2) - ((tfh * textsizeht) / 2)) + 1 + updn));
        print(btext);
    }
    TextColor(tcolorbckup, tcolorbgbckup);
    __restoreFont();
    textsize = fsizebckup;
    textsizeht = fsizehbckup;
    MoveTo(curxbckup, curybckup);
    nl = nlbckp;
    wrap = twrap;
}

void gfx4desp32::drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w,
    int16_t h, uint16_t colorb, String btext,
    int8_t tfont, int8_t tfontsize, int8_t tfontsizeht,
    uint16_t tcolor) {
    boolean twrap = wrap;
    boolean nlbckp = nl;
    nl = false;
    wrap = false;
    int8_t tfw;
    int8_t tfh;
    if (tfont < 2) {
        tfw = 6;
        tfh = 8;
    }
    else {
        tfw = 9;
        tfh = 16;
    }
    uint8_t fsizebckup = textsize;
    uint8_t fsizehbckup = textsizeht;
    textsize = tfontsize;
    textsizeht = tfontsizeht;
    uint16_t tcolorbckup = textcolor;
    uint16_t tcolorbgbckup = textbgcolor;
    uint16_t curxbckup = cursor_x;
    uint16_t curybckup = cursor_y;
    uint32_t tcol = bevelColor(colorb);
    uint16_t _dark = tcol >> 16;
    uint16_t _light = tcol & 0xffff;
    tdark = _dark;
    tlight = _light;
    if (bxStyle == 0) {
        RectangleFilled(x + 2, y + 2, (x + 2) + w - 4, (y + 2) + h - 4, colorb);
        if (updn == 0) {
            Hline(x, y, w, _light);
            Hline(x + 1, y + 1, w - 2, _light);
            Vline(x, y, h, _light);
            Vline(x + 1, y + 1, h - 2, _light);
            Hline(x, y + h - 1, w, _dark);
            Hline(x + 1, y + h - 2, w - 2, _dark);
            Vline(x + w - 1, y, h, _dark);
            Vline(x + w - 2, y + 1, h - 2, _dark);
        }
        if (updn == 1) {
            Hline(x, y, w, _dark);
            Hline(x + 1, y + 1, w - 2, _dark);
            Vline(x, y, h, _dark);
            Vline(x + 1, y + 1, h - 2, _dark);
            Hline(x, y + h - 1, w, _light);
            Hline(x + 1, y + h - 2, w - 2, _light);
            Vline(x + w - 1, y, h, _light);
            Vline(x + w - 2, y + 1, h - 2, _light);
        }
    }
    if (bxStyle > 0) {
        int nh;
        int g1 = 0; int g2 = 0;
        int nw = h / 10;
        if (nw == 0)
            nw = 1;
        if (bxStyle == 1) {
            nh = h / 6;
            g1 = -1;
            g2 = 0;
        }
        if (bxStyle > 1) {
            nh = h >> 1;
            g1 = 28;
            g2 = 0;
        }
        if (bxStyle == 3) {
            g1 = 48;
            g2 = 28;
        }
        if (updn == 0)
            gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
                GRADIENT_RAISED, g1, colorb, GRADIENT_RAISED, g2, 0);
        if (updn == 1)
            gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
                GRADIENT_RAISED, 0, colorb, GRADIENT_RAISED, g2, 0);
    }
    TextColor(tcolor, tcolor);
    __tempFont(tfont);
    uint8_t blen = btext.length();
    if (blen > 0) {
        MoveTo(((x + (w / 2) - ((blen * tfw * textsize) / 2)) + updn),
            ((y + (h / 2) - ((tfh * textsizeht) / 2)) + 1 + updn));
        print(btext);
    }
    TextColor(tcolorbckup, tcolorbgbckup);
    __restoreFont();
    textsize = fsizebckup;
    textsizeht = fsizehbckup;
    MoveTo(curxbckup, curybckup);
    nl = nlbckp;
    wrap = twrap;
}

void gfx4desp32::ButtonXstyle(byte bs) { bxStyle = bs; }

void gfx4desp32::ButtonActive(uint8_t butno, boolean act) {
    bactive[butno] = act;
}

void gfx4desp32::DeleteButton(int hndl) {
    if (hndl > 0) {
        RectangleFilled(bposx[hndl], bposy[hndl], bposx[hndl] + bposw[hndl] - 1,
            bposy[hndl] + bposh[hndl] - 1, bposc[hndl]);
        bactive[hndl] = false;
    }
    else {
        for (int n = 0; n > 128; n++) {
            RectangleFilled(bposx[n], bposy[n], bposx[n] + bposw[n] - 1,
                bposy[n] + bposh[n] - 1, bposc[n]);
            bactive[n] = false;
        }
    }
}

void gfx4desp32::DeleteButton(int hndl, uint16_t color) {
    if (hndl > 0) {
        RectangleFilled(bposx[hndl], bposy[hndl], bposx[hndl] + bposw[hndl] - 1,
            bposy[hndl] + bposh[hndl] - 1, color);
        bactive[hndl] = false;
    }
    else {
        for (int n = 0; n > 128; n++) {
            RectangleFilled(bposx[n], bposy[n], bposx[n] + bposw[n] - 1,
                bposy[n] + bposh[n] - 1, color);
            bactive[n] = false;
        }
    }
}

void gfx4desp32::DeleteButtonBG(int hndl, int objBG) {
    if (hndl > 0) {
        UserImageDR(objBG, bposx[hndl], bposy[hndl], bposw[hndl], bposh[hndl],
            bposx[hndl], bposy[hndl]);
        bactive[hndl] = false;
    }
    else {
        for (int n = 0; n > 128; n++) {
            UserImageDR(objBG, bposx[n], bposy[n], bposw[n], bposh[n], bposx[n],
                bposy[n]);
            bactive[n] = false;
        }
    }
}

void gfx4desp32::gradientShape(int vert, int ow, int xPos, int yPos, int w,
    int h, int r1, int r2, int r3, int r4,
    int darken, uint16_t color, int sr1, int gl1,
    uint16_t colorD, int sr3, int gl3, int gtb) {

    int arcn = 0; int xc1 = 0; int xc2 = 0; int xc3 = 0; int xc4 = 0; int yc1 = 0; int mi = 0;
    int cDraw = 0; int xgxl = 0; int lastxc1 = 0; int xgxr = 0; int tcount = 0; int tgss1 = 0;
    int tgss2 = 0; int drgs = 0; int yag = 0; int lastarc = 0; int lachk = 0;
    int c = 0; int xx = 0; int yy = 0; int x = 0; int y = 0; int c1 = 0; int xx1 = 0;
    int yy1 = 0; int xin = 0; int y1 = 0; int lasty1 = 0; int sCxPos = 0; int owMin = 0;
    int owMax = 0; int mirror = 0; int dbl1 = 0; int dbl2 = 0; int gradofs = 0; int hm = 0;
    int hmd = 0; int gof = 0; int owSet = 0; int rtSide = 0; int lasty = 0; int incy = 0;
    int incy1 = 0; int maxR = 0; int mh = 0; int mw = 0; int xa = 0; int xb = 0; int xc = 0;
    int ya = 0; int radsMinOw = 0; int txPos = 0; int tyPos = 0; int cErase = 0;
    int tow = 0; int tsr1 = 0; int tsr3 = 0; int tgl3 = 0; int tdarken = 0; int tgl1 = 0;
    int fill = 0; int yb = 0; int Lfill = 0; int Rfill = 0; int Ldw = 0; int Rdw = 0; int Lw = 0;
    int Rw = 0; int mfill = 0; int start = 0; int fin = 0; int fCmp = 0; int tFillL = 0;
    int tFillR = 0; int cropfill = 0;
    uint16_t tcolor = 0;
    uint16_t tcolorD = 0;
    uint16_t tgrad = 0;
    if (vert) {
        swap(xPos, yPos);
        swap(h, w);
        swap(r2, r3);
    }
    int oddf = h % 2;
    radsGS[0] = r1 + !(r1);
    radsGS[1] = r2 + !(r2);
    radsGS[2] = r3 + !(r3);
    radsGS[3] = r4 + !(r4);
    mh = max(radsGS[0] + radsGS[2], radsGS[1] + radsGS[3]);
    mw = max(radsGS[0] + radsGS[1], radsGS[2] + radsGS[3]);
    if (mh > h - 1)
        h = mh - 1;
    if (mw > w - 1)
        w = mw - 1;
    maxR = max(max(radsGS[0], radsGS[1]), max(radsGS[2], radsGS[3]));
    if (maxR > MAX_ARCSIZE)
        return;
    mirror = ((radsGS[0] == radsGS[2]) && (radsGS[1] == radsGS[3]));
    dbl1 = (radsGS[0] == radsGS[1]);
    dbl2 = (radsGS[2] == radsGS[3]);
    xpGSaPos[0] = radsGS[0] - 1;
    ypGSaPos[0] = radsGS[0] - 1;
    xpGSaPos[1] = w - radsGS[1];
    ypGSaPos[1] = radsGS[1] - 1;
    xpGSaPos[2] = radsGS[2] - 1;
    ypGSaPos[2] = h - radsGS[2];
    xpGSaPos[3] = w - radsGS[3];
    ypGSaPos[3] = h - radsGS[3];
    owMin = yPos + ow;
    owMax = yPos + h - ow - 1;
    if (!(lastAsize))
        lastAsize = maxR;
    if (!(keepLastArc))
        memcpy(inx, lastArcOld, lastAsize << 1);
    arcn = 0;
    if (oddf && (GSCropArcLeft > -1 || GSCropArcRight > -1))
        cropfill = 1;
    while (arcn < 4) {
        arcn += (radsGS[arcn] < 1);
        if (arcn == 1 && dbl1 && mirror)
            break;
        arcn += (arcn == 1 && dbl1);
        if (arcn == 2 && dbl1 == 0 && mirror)
            break;
        if (arcn == 3 && dbl2)
            break;
        if (arcn && !(lachk))
            memset(inx, 0, radsGS[arcn] << 1);
        incy1 = radsGS[arcn] - ow;
        incy = radsGS[arcn];
        if (radsGS[arcn]) {
            radsMinOw = radsGS[arcn] - ow;
            c = 1 - radsGS[arcn];
            c1 = 1 - radsMinOw;
            xx = 1;
            xx1 = 1;
            yy = -2 * radsGS[arcn];
            yy1 = -2 * radsMinOw;
            x = 0;
            xin = 0;
            y = radsGS[arcn];
            y1 = radsMinOw;
            cErase = 0;
            while (cErase < 1 + GSErase) { // If Erase flag is set do twice, first to
                // erase last and immediately draw new
                if (GSErase) {               // ** function for rotaries **
                    if (!(cErase)) {
                        txPos = xPos;
                        tyPos = yPos;
                        xPos = GSEraseXpos;
                        yPos = GSEraseYpos;
                        tow = ow;
                        ow = 0;
                        tcolor = color;
                        color = GSEraseColour;
                        tcolorD = colorD;
                        colorD = GSEraseHeight;
                        tsr1 = sr1;
                        sr1 = GSERaisedSunk;
                        tgl1 = gl1;
                        gl1 = GSEraseGLevel;
                        tsr3 = sr3;
                        sr3 = -2;
                        tgl3 = gl3;
                        gl3 = GSErasePHeight;
                        tdarken = darken;
                        darken = 0;
                    }
                    else {
                        xPos = txPos;
                        yPos = tyPos;
                        ow = tow;
                        colorD = tcolorD;
                        sr3 = tsr3;
                        gl3 = tgl3;
                        sr1 = tsr1;
                        gl1 = tgl1;
                        color = tcolor;
                        darken = tdarken;
                    }
                }
                // ***************************************
                cDraw = 0;
                tcount = radsGS[arcn];
                if ((GSCropArcLeft > -1 || GSCropArcRight > -1) && GSSsxpos != -9999)
                    tcount = lastAsize; // For Slider
                lachk = (lastarc != radsGS[arcn]);
                while (cDraw < tcount) {
                    if (x < y && lachk) {
                        if (xin < y1) {
                            if (c1 >= 0) {
                                y1--;
                                yy1 += 2;
                                c1 += yy1;
                            }
                            inx[xin] = (inx[xin] & 0xFF00) | y1;
                            if (lasty1 != y1) {
                                inx[incy1] = (inx[incy1] & 0xFF00) | xin;
                                incy1--;
                            }
                            xin++;
                            xx1 += 2;
                            c1 += xx1;
                            lasty1 = y1;
                        }
                        if (c >= 0) {
                            y--;
                            yy += 2;
                            c += yy;
                        }
                        inx[x] = (inx[x] & 0x00FF) | ((y - 1) << 8);
                        if (lasty != y) {
                            inx[incy] = (inx[incy] & 0x00FF) | ((x - 1) << 8);
                            incy--;
                        }
                        x++;
                        xx += 2;
                        c += xx;
                        lasty = y;
                    }
                    xc3 = xPos + xpGSaPos[arcn];

                    xc4 = xc3;
                    xa = inx[cDraw] & 0x00FF;
                    xb = inx[cDraw] >> 8;
                    xc = lastArcOld[cDraw] >> 8; // Get outer and inner arc position
                    // ** Fixes error if old value isn't overwritten in the array **
                    if (cDraw && xa > lastxc1)
                        xa = lastxc1;
                    if (xb > radsGS[arcn])
                        xb = 0;
                    if (xa > radsGS[arcn])
                        xa = 0;
                    lastxc1 = xa;
                    xc2 = 0;

                    //  *************************************************************
                    if (arcn == 0) { // Calculate first arc
                        sCxPos = xPos + xpGSaPos[0];
                        yc1 = yPos + ypGSaPos[0] - cDraw;
                        xc1 = sCxPos - xb;
                        xc2 = sCxPos - xa;

                        if (GSCropArcLeft > -1) { // Modify if crop value set, only works
                            // for doubled and mirrored shapes
                            xc1 = xPos + GSCropArcLeft + xc;
                            if (cDraw >= lastAsize)
                                xc1 = xPos + GSCropArcLeft;
                            xc2 = xc1;
                        }
                    }
                    if (arcn ==
                        1 - dbl1) { // Calculate second arc or modify first if a double
                        sCxPos = xPos + xpGSaPos[1] - vert;
                        if (dbl1) {
                            xc3 = sCxPos + xa;
                            xc4 = sCxPos + xb;
                            if (GSCropArcLeft > 0 && xc2 > xc3)
                                xc3 = xc2;
                        }
                        else {
                            yc1 = yPos + ypGSaPos[1] - cDraw;
                            xc2 = sCxPos + xb;
                            xc1 = sCxPos + xa;
                        }
                    }
                    if (!(mirror)) {   // If mirrored, ignore calcs for next arcs
                        if (arcn == 2) { // Calculate third arc
                            sCxPos = xPos + xpGSaPos[2];
                            yc1 = yPos + ypGSaPos[2] + cDraw;
                            xc1 = sCxPos - xb;
                            xc2 = sCxPos - xa;
                        }
                        if (arcn ==
                            (3 - dbl2)) { // Calculate third arc or modify third if double
                            sCxPos = xPos + xpGSaPos[3] - vert;
                            if (dbl2) {
                                xc3 = sCxPos + xa;
                                xc4 = sCxPos + xb;
                            }
                            else {
                                yc1 = yPos + ypGSaPos[3] + cDraw;
                                xc2 = sCxPos + xb;
                                xc1 = sCxPos + xa;
                            }
                        }
                    }
                    if (GSCropArcRight > -1) { // Modify if crop value set, only works for
                        // doubled and mirrored shapes
                        xc3 = xPos + GSCropArcRight - xc;
                        if (cDraw >= lastAsize)
                            xc3 = xPos + GSCropArcRight + 1;
                        xc4 = xc3;
                        if (xc3 < xc2)
                            xc2 = xc3;
                    }
                    if (sr3 ==
                        -2) { // Check for erase flag and use new parameters for gradient
                        hm = colorD;
                        gradofs = gl3;
                    }
                    else {
                        hm = h;
                        gradofs = 0;
                    }
                    if (gtb) {
                        hmd = h << 1;
                        if (gtb == 2)
                            gof = h;
                        gradofs = h >> 1;
                        hm += gradofs;
                    }
                    else {
                        hmd = h;
                    }
                    mi = 0;
                    while (mi < 1 + mirror + cropfill) { // Do twice if mirrored

                        if (cropfill)
                            oddf = 0;
                        if (mi >= 1 && mi <= 2)
                            yc1 = yPos + (h - 1 - (yc1 - yPos)) + (mi == 2);
                        ya = yc1 - yPos;
                        owSet = (yc1 >= owMin && yc1 <= owMax);
                        rtSide = (arcn == 1 || arcn == 3);
                        tgss1 = GSSsxpos + GSSLastSliderVal + xc + 1;
                        tgss2 = GSSsxpos + GSSLastSliderVal - xc - 1;
                        yag = ya + gradofs;
                        if (cDraw < radsGS[arcn]) {
                            tgrad = Grad(sr1, darken, gl1, hm, yag, color);
                            if (ow) {
                                if (sr3 > -1 &&
                                    ((owSet && xc2 < xc3) || (rtSide && owSet && xc2 > xc3)))
                                    (vert == 1)
                                    ? VlineD(yc1, xc2, xc3,
                                        Grad(sr3, darken, gl3, hmd, ya + gof, colorD))
                                    : HlineD(yc1, xc2, xc3,
                                        Grad(sr3, darken, gl3, hmd, ya + gof, colorD));
                                if (xc1 < xc2)
                                    (vert == 1) ? VlineD(yc1, xc1, xc2, tgrad)
                                    : HlineD(yc1, xc1, xc2, tgrad);
                                if (!(owSet)) {
                                    if ((xc1 < xc4) || (rtSide && xc1 > xc4))
                                        (vert == 1) ? VlineD(yc1, xc1, xc4, tgrad)
                                        : HlineD(yc1, xc1, xc4, tgrad);
                                }
                                else {
                                    ///// ***** Added 9/12/2019 -- Condition for Slider alider
                                    ///animation to remove flicker *****
                                    if (!(GSSsxpos != -9999))
                                        (vert == 1) ? VlineD(yc1, xc1, xc2, tgrad)
                                        : HlineD(yc1, xc1, xc2, tgrad);
                                    if (xc3 < xc4)
                                        (vert == 1) ? VlineD(yc1, xc3, xc4, tgrad)
                                        : HlineD(yc1, xc3, xc4, tgrad);
                                }
                            }
                            else {
                                if ((xc1 < xc4) || (rtSide && xc4 < xc1))
                                    (vert == 1) ? VlineD(yc1, xc1, xc4, tgrad)
                                    : HlineD(yc1, xc1, xc4, tgrad);
                            }
                            if (GSSsxpos != -9999) {
                                drgs = 0;
                                if (GSCropArcRight > -1) {
                                    xgxl = tgss2;
                                    if (xc1 > xc4) {
                                        xgxr = xc4;
                                        drgs = 1;
                                    }
                                    else {
                                        if (xgxl < xc1) {
                                            xgxr = xc1 - 1;
                                            drgs = 1;
                                        }
                                    }
                                }
                                if (GSCropArcLeft > -1) {
                                    xgxl = tgss1;
                                    if (xc1 > xc4) {
                                        xgxr = xc1;
                                        drgs = 1;
                                    }
                                    else {
                                        if (xgxl > xc4) {
                                            xgxr = xc4 + 1;
                                            drgs = 1;
                                        }
                                    }
                                }

                                if (drgs)
                                    (vert == 1) ? VlineD(yc1, xgxl, xgxr, GSSBGColor)
                                    : HlineD(yc1, xgxl, xgxr, GSSBGColor);
                            }
                        }
                        else {
                            if (GSSsxpos != -9999) {
                                if (GSCropArcLeft > -1) {
                                    xgxl = GSSsxpos + xc + GSCropArcLeft - 1;
                                    xgxr = tgss1;
                                }
                                if (GSCropArcRight > -1) {
                                    xgxl = tgss2;
                                    xgxr = GSSsxpos + GSCropArcRight - xc;
                                }

                                (vert == 1) ? VlineD(yc1, xgxl, xgxr, GSSBGColor)
                                    : HlineD(yc1, xgxl, xgxr, GSSBGColor);
                            }
                        }
                        mi++;
                    }
                    cDraw++;
                }
                lastarc = radsGS[arcn];

                cErase++;
            }
        }
        arcn++;
    }
    // ** Fill space not occupied by arcs **
    if (!((radsGS[0] == radsGS[1] && radsGS[2] == radsGS[3]) &&
        (radsGS[0] == radsGS[2] && radsGS[0] + radsGS[2] >= h - 2)) ||
        (oddf && !(cropfill))) { // check to see if fill is needed
        fin = h;
        start = 0;
        if (radsGS[0] == radsGS[1])
            start = radsGS[0];
        if (radsGS[2] == radsGS[3])
            fin = h - radsGS[2];
        fill = start;
        while (fill < fin) {
            yb = yPos + fill;
            Lfill = xPos;
            Rfill = xPos + w - 1;
            Ldw = Lfill + ow;
            Rdw = Rfill - ow;
            if (fill <= ypGSaPos[0])
                Lfill = (xPos + xpGSaPos[0]) * (!dbl1);
            if (fill <= ypGSaPos[1])
                Rfill = (xPos + xpGSaPos[1]) * (!dbl1);
            if (fill >= ypGSaPos[2])
                Lfill = (xPos + xpGSaPos[2]) * (!dbl2);
            if (fill >= ypGSaPos[3])
                Rfill = (xPos + xpGSaPos[3]) * (!dbl2);
            Lw = ow - 1;
            Rw = ow - 1;
            if (Lfill > Ldw)
                Lw = 0;
            if (Rfill < Rdw)
                Rw = 0;
            mfill = fill + gradofs;
            fCmp = (Lfill < Rfill);
            tgrad = Grad(sr1, darken, gl1, hm, mfill, color);
            tFillL = Lfill + Lw; // ** added 28/11/2019 **
            tFillR = Rfill - Rw; // ** added 28/11/2019 **
            if (ow > 0) {
                if (yb >= owMin && yb <= owMax && fCmp) {
                    if (Lfill <= Ldw)
                        (vert == 1)
                        ? VlineD(yb, Lfill, tFillL, tgrad)
                        : HlineD(yb, Lfill, tFillL, tgrad); // ** modified 28/11/2019 **
                    if (sr3 > -1 && tFillL + 1 < tFillR)
                        (vert == 1) ? VlineD(yb, tFillL + 1, tFillR,
                            Grad(sr3, darken, gl3, hm, mfill, colorD))
                        : HlineD(yb, tFillL + 1, tFillR,
                            Grad(sr3, darken, gl3, hm, mfill,
                                colorD)); // ** modified 28/11/2019 **
                    if (Rfill >= Rdw)
                        (vert == 1)
                        ? VlineD(yb, tFillR, Rfill, tgrad)
                        : HlineD(yb, tFillR, Rfill, tgrad); // ** modified 28/11/2019 **
                }
                else {
                    if (fCmp)
                        (vert == 1) ? VlineD(yb, Lfill, Rfill, tgrad)
                        : HlineD(yb, Lfill, Rfill, tgrad);
                }
            }
            else {
                if (fCmp)
                    (vert == 1) ? VlineD(yb, Lfill, Rfill, tgrad)
                    : HlineD(yb, Lfill, Rfill, tgrad);
            }
            fill++;
        }
    }
    // ** reset used external intiables
    GSCropArcLeft = -1;
    GSCropArcRight = -1;
    keepLastArc = 0;
    GSErase = 0;
    GSSsxpos = -9999;
    GSSArconly = 0;
    if (!(protectLA))
        lastAsize = maxR; // Used for slider
    protectLA = 0;
    return;
}

/****************************************************************************/
/*!
  @brief  4DGL compatible Hline function
  @param  y - vertical start position
  @param  x1 - horizontal start position
  @param  y1 - horizontal end position
*/
/****************************************************************************/
void gfx4desp32::HlineD(int y, int x1, int x2, uint16_t color) {
    if (x1 > x2)
        swap(x1, x2);
    Hline(x1, y, x2 - x1 + 1, color);
}

void gfx4desp32::VlineD(int x, int y1, int y2, uint16_t color) {
    if (y1 > y2)
        swap(y1, y2);
    Vline(x, y1, y2 - y1 + 1, color);
}

void gfx4desp32::GradTriangleFilled(int x0, int y0, int x1, int y1, int x2,
    int y2, int color, int ncCol, int h,
    int ypos, int lev, int erase) {
    int a, b, y, last;
    if (y0 > y1) {
        swap(y0, y1);
        swap(x0, x1);
    }
    if (y1 > y2) {
        swap(y2, y1);
        swap(x2, x1);
    }
    if (y0 > y1) {
        swap(y0, y1);
        swap(x0, x1);
    }
    if (y0 == y2) {
        a = x0;
        b = x0;
        if (x1 < a) {
            a = x1;
        }
        else if (x1 > b) {
            b = x1;
        }
        if (x2 < a) {
            a = x2;
        }
        else if (x2 > b) {
            b = x2;
        }
        if (erase)
            color = Grad(0, 0, lev, h, y0 - ypos, ncCol);
        HlineD(y0, a, b, color);
        return;
    }
    int dx01, dy01, dx02, dy02, dx12, dy12, sa, sb, t1, t2, t3;
    dx01 = x1 - x0;
    dy01 = y1 - y0;
    dx02 = x2 - x0;
    dy02 = y2 - y0;
    dx12 = x2 - x1;
    dy12 = y2 - y1;
    sa = 0;
    sb = 0;
    if (y1 == y2) {
        last = y1; // Include y1 scanline
    }
    else {
        last = y1 - 1; // Skip it
    }
    if (last < 0)
        last = 0;
    for (y = y0; y <= last; y++) {
        t1 = x0;
        t2 = dy01;
        t3 = dx01;
        a = t1 + sa / t2;
        b = x0 + sb / dy02;
        sa += t3;
        sb += dx02;
        if (erase)
            color = Grad(0, 0, lev, h, y - ypos, ncCol);
        HlineD(y, a, b, color);
    }
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (y = y; y <= y2; y++) {
        t1 = x1;
        t2 = dy12;
        t3 = dx12;
        a = t1 + sa / t2;
        b = x0 + sb / dy02;
        sa += t3;
        sb += dx02;
        if (erase)
            color = Grad(0, 0, lev, h, y - ypos, ncCol);
        HlineD(y, a, b, color);
    }
}

uint16_t gfx4desp32::Grad(int GraisSunk, int Gstate, int Gglev, int Gh1,
    int Gpos, uint16_t colToAdj) {
    // **** Modified gradients for vertical again ****        *new*
    //uint32_t __alpha;
    //uint32_t __alphatemp;
    //uint16_t __colour;
    int tgcol = colToAdj;
    int CTAr, CTAg, CTAb;
    int CTALevel;
    int BflatMin = 0;
    int BflatMax = 0;
    int w = 0;
    int t = 0;

    if (Gglev > 63)
        Gglev = 63;
    if (GraisSunk > 99) { // **** new ****
        w = GraisSunk & 0x00ff;
        GraisSunk = GraisSunk >> 8;
    }
    if (GraisSunk >= 4) { // **** end of new ****
        Gpos <<= 1;
        if (Gpos > Gh1)
            Gpos = Gh1 - (Gpos - Gh1); // **** new ****
        if (Gpos > w << 1 && w > 0 && GraisSunk == 4) {
            t = Gpos - (w << 1);
            Gpos = (Gpos - t) + (t >> 1);
        }
        if (GraisSunk == 5 && w > 0) {
            Gh1 -= w;
            Gpos -= w;
        } // **** end of new ****

        if (GraisSunk == 4) {
            GraisSunk = 5;
        }
        else {
            GraisSunk = 4;
        }
    }
    Gpos -= (Gh1 >> 1);
    if (Gglev == -1 || GraisSunk >= 6) {
        Gglev = 31;
        BflatMax = Gh1 >> 2;
        BflatMin = 0 - BflatMax;
        if (Gpos > BflatMin && Gpos < BflatMax)
            Gpos = 0;
        if (Gpos <= BflatMin)
            Gpos += BflatMax;
        if (Gpos >= BflatMax)
            Gpos -= BflatMax;
    }

    CTALevel = (Gglev * Gpos) / Gh1;
    CTAr = (tgcol >> 11) - Gstate;
    CTAg = ((tgcol >> 6) & 0x001F) - Gstate;
    CTAb = (tgcol & 0x001F) - Gstate;
    if (!((GraisSunk & 0x01)))
        CTALevel = 0 - CTALevel;
    CTAr += CTALevel;
    CTAg += CTALevel;
    CTAb += CTALevel;
    CTAr = (CTAr > 0) * CTAr;
    CTAg = (CTAg > 0) * CTAg;
    CTAb = (CTAb > 0) * CTAb;
    if (CTAr > 31)
        CTAr = 31;
    if (CTAg > 31)
        CTAg = 31;
    if (CTAb > 31)
        CTAb = 31;
    if (CTAr < 0)
        CTAr = 0;
    if (CTAg < 0)
        CTAg = 0;
    if (CTAb < 0)
        CTAb = 0;

    tgcol = (CTAr << 11) | (CTAg << 6) | CTAb;

    return tgcol;
}

void gfx4desp32::drawChar2tw(int16_t x, int16_t y, unsigned char c,
    uint16_t color, uint16_t bg, uint8_t size) {
    if ((x >= width) || (y >= height) || ((x + (8 + 1) * size - 1) < 0) ||
        ((y + 16 * size - 1) < 0))
        return;
    if (c < 32 && c > 128)
        return;
    uint16_t temppix[257];
    uint16_t pc = 0;
    SetGRAM(x, y, x + 7, y + 15);
    uint8_t trow = 0x80;
    uint8_t chb;
    uint16_t c2pos = c * 16;
    for (uint8_t j = 0; j < 16; j++) {
        chb = font2[c2pos + j];
        for (uint8_t i = 0; i < 8; i++) {
            if (chb & trow) {
                temppix[pc] = color;
            }
            else {
                temppix[pc] = bg;
            }
            chb <<= 1;
            pc++;
        }
        trow = 0x80;
    }
    WrGRAMs(temppix, 128);
}

void gfx4desp32::drawChar1tw(int16_t x, int16_t y, unsigned char c,
    uint16_t color, uint16_t bg, uint8_t size) {
    if ((x >= width) || (y >= height) || ((x + (fsw + 1) * size - 1) < 0) ||
        ((y + fsh * size - 1) < 0))
        return;
    if (c < 32 && c > 128)
        return;
    SetGRAM(x, y, x + 4, y + 7);
    uint8_t trow = 0x01;
    uint8_t chb;
    uint8_t chb1;
    uint16_t temppix[40];
    uint8_t pc = 0;
    for (uint8_t j = 0; j < 8; j++) {
        for (uint8_t i = 0; i < 5; i++) {
            chb = font1[(c * 5) + i];
            chb1 = chb >> j;
            if (chb1 & trow) {
                temppix[pc] = color;
            }
            else {
                temppix[pc] = bg;
            }
            pc++;
        }
    }
    WrGRAMs(temppix, 40);
}

void gfx4desp32::TWprintln(String istr) {
    TWstring2write(istr);
    TWwrite(13);
}

void gfx4desp32::TWprintln(char* istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(int8_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(uint8_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(int16_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(uint16_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(int32_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(uint32_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(int64_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(uint64_t istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprintln(float istr) {
    TWstring2write(String(istr));
    TWwrite(13);
}

void gfx4desp32::TWprint(String istr) { TWstring2write(istr); }

void gfx4desp32::TWprint(char* istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(int8_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(uint8_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(int16_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(uint16_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(int32_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(uint32_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(int64_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(uint64_t istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWprint(float istr) { TWstring2write(String(istr)); }

void gfx4desp32::TWstring2write(String istr) {
    char tistr;
    uint16_t len = istr.length();
    for (int n = 0; n < len; n++) {
        tistr = istr.charAt(n);
        TWwrite(tistr);
    }
}

String gfx4desp32::GetCommand() {
    String tcmdtxt = cmdtxt;
    cmdtxt = "";
    return tcmdtxt;
}

void gfx4desp32::TWtextcolor(uint16_t twc) { twcolnum = twc; }

boolean gfx4desp32::TWMoveTo(uint8_t twcrx, uint8_t twcry) {
    if (twcrx <= chracc && twcry <= chrdwn && chracc > 0 && chrdwn > 0) {
        twcurx = txtx + (9 * twcrx);
        twcury = txty + (16 * twcry);
        twxpos = twcrx;
        twypos = twcry;
        return true;
    }
    else {
        return false;
    }
}

void gfx4desp32::TWprintAt(uint8_t pax, uint8_t pay, String istr) {
    if (TWMoveTo(pax, pay))
        TWprint(istr);
}

void gfx4desp32::TWwrite(const char txtinput) {
    if (TWimgSet) {
        if (twcurson && twen)
            drawChar2TWimage(0, TWimage, 0, twcurx, twcury, txtf);
    }
    else {
        if (twcurson && twen)
            drawChar2tw(twcurx, twcury, 0, txtf, txtb, 1);
    }
    boolean skip2 = false;
    if (txtinput > 31) {
        twtext = twtext + char(txtinput);
        if (TWimgSet) {
            if (twen)
                drawChar2TWimage(txtinput - 32, TWimage, 0, twcurx, twcury, twcolnum);
        }
        else {
            if (twen)
                drawChar2tw(twcurx, twcury, txtinput - 32, twcolnum, txtb, 1);
        }
        txtbuf[(chracc * twypos) + twxpos] = txtinput;
        txfcol[(chracc * twypos) + twxpos] = twcolnum;
        twcurx = twcurx + 9;
        twxpos++;
        if ((twcurx + 8 + 1) > (txtw + txtx)) {
            twcury = twcury + 16;
            twcurx = txtx;
            twypos++;
            twxpos = 0;
        }
    }
    if (txtinput == 9) {
        uint tcnt = 0;
        uint ccpos = twcurx / 9;
        for (int n = 0; n < (chracc / 10); n++) {
            tcnt = tcnt + 14;
            if (tcnt > ccpos) {
                for (uint o = 0; o < (tcnt - ccpos); o++) {
                    twtext = twtext + char(32);
                    twcurx = twcurx + 9;
                    twxpos++;
                    txtbuf[(chracc * twypos) + twxpos] = 32; // 8 + 2
                    txfcol[(chracc * twypos) + twxpos] = twcolnum;
                }
                break;
            }
        }
    }
    if (txtinput == 13 || txtinput == 10) {
        twcury = twcury + 16;
        twypos++;
        uint8_t remtxt = chracc - twxpos + 1;
        twcurx = txtx;
        twxpos = 0;
        twcl = twcl + 1;
        if (txtinput == 13) {
            cmdtxt = twtext;
            twtext = "";
        }
        for (int n = 0; n < remtxt; n++) {
            txtbuf[(chracc * (twypos - 1)) + (chracc - n)] = char(0);
        }
    }
    if (txtinput == 8) {
        if (twypos < 1 && twxpos < 1) {
            return;
        }
        uint16_t lenct = twtext.length();
        if ((twcurx - txtx + 6) < (10) && lenct > 0 && twcury > fsh) {
            skip2 = true;
            twcury = twcury - 16;
            twypos--;
            twcurx = txtx + (((txtw / 9) * 9) - 9);
            twxpos = chracc - 1;
            if (TWimgSet) {
                if (twen)
                    drawChar2TWimage(0, TWimage, 0, twcurx, twcury, twcolnum);
            }
            else {
                if (twen)
                    drawChar2tw(twcurx, twcury, 0, twcolnum, txtb, 1);
            }
            txtbuf[(chracc * twypos) + twxpos] = txtinput;
            txfcol[(chracc * twypos) + twxpos] = twcolnum;
        }
        if (twcurx > txtx && lenct > 0 && skip2 == false) {
            twcurx = twcurx - 8 - 1;
            twxpos--;
            if (TWimgSet) {
                if (twen)
                    drawChar2TWimage(0, TWimage, 0, twcurx, twcury, twcolnum);
            }
            else {
                if (twen)
                    drawChar2tw(twcurx, twcury, 0, twcolnum, txtb, 1);
            }
            txtbuf[(chracc * twypos) + twxpos] = txtinput;
            txfcol[(chracc * twypos) + twxpos] = twcolnum;
        }
        String tempcmd = "";
        for (int n = 0; n < (lenct - 1); n++) {
            tempcmd = tempcmd + twtext.charAt(n);
        }
        twtext = tempcmd;
    }
    if ((twcury - txty) + 16 > txth) {
        uint16_t tempc;
        uint16_t tempp;
        uint16_t tempcpos;
        uint16_t temptwcol;
        uint16_t temptwcolc;

        for (int n = 0; n < chrdwn - 1; n++) {
            yield();
            for (int o = 0; o < chracc; o++) {
                tempcpos = ((n + 1) * chracc) + o;
                tempc = txtbuf[tempcpos];
                tempp = txtbuf[(n * chracc) + o];
                temptwcol = txfcol[tempcpos];
                temptwcolc = txfcol[(n * chracc) + o];
                txtbuf[(n * chracc) + o] = tempc;
                txfcol[(n * chracc) + o] = temptwcol;
                if (tempc < 32)
                    tempc = 32;
                if (tempp < 32)
                    tempp = 32;
                if (tempc != tempp || temptwcol != temptwcolc) {
                    if (TWimgSet) {
                        if (twen)
                            drawChar2TWimage(tempc - 32, TWimage, 0, txtx + (9 * o),
                                txty + (16 * n), temptwcol);
                    }
                    else {
                        if (twen)
                            drawChar2tw(txtx + (9 * o), txty + (16 * n), tempc - 32,
                                temptwcol, txtb, 1);
                    }
                }
            }
        }
        twcury = twcury - 16;
        twypos--;
        if (TWimgSet) {
            if (twen)
                UserImagesDR(TWimage, 0, twcurx - tuix[TWimage], twcury - tuiy[TWimage],
                    txtw - 1, 16);
        }
        else {
            if (twen)
                RectangleFilled(twcurx, twcury, twcurx + (txtw - 1) - 1, twcury + 16,
                    txtb);
        }
    }
    if (twcurson) {
        if (TWimgSet) {
            if (twen)
                drawChar2TWimage(63, TWimage, 0, twcurx, twcury, txtf);
        }
        else {
            if (twen)
                drawChar2tw(twcurx, twcury, 63, txtf, txtb, 1);
        }
    }
}

void gfx4desp32::TWcursorOn(bool twco) { twcurson = twco; }

void gfx4desp32::TWcls() {
    if (TWimgSet) {
        UserImages(TWimage, 0);
    }
    else {
        RectangleFilled(txtx - 3, txty - 3, (txtx - 3) + (txtw + 2) - 1,
            (txty - 3) + (txth + 2) - 1, txtb);
    }
    twcurx = txtx;
    twcury = txty;
    twxpos = 0;
    twypos = 0;
    for (int n = 0; n < sizeof(txtbuf); n++) {
        txtbuf[n] = 0;
    }
}

void gfx4desp32::TWcolor(uint16_t fcol) {
    txtf = fcol;
    TWtextcolor(fcol);
}

void gfx4desp32::TWcolor(uint16_t fcol, uint16_t bcol) {
    txtf = fcol;
    txtb = bcol;
    TWtextcolor(fcol);
}

void gfx4desp32::TextWindowImage(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t txtcolor, uint16_t TWimg,
    uint16_t frcolor) {
    TWimage = TWimg;
    TWimgSet = 1;
    if (x < tuix[TWimg])
        x = tuix[TWimg];
    if (y < tuiy[TWimg])
        y = tuiy[TWimg];
    if (x + w > tuix[TWimg] + tuiw[TWimg])
        w = tuix[TWimg] + tuiw[TWimg] - x;
    if (y + h > tuiy[TWimg] + tuih[TWimg])
        h = tuiy[TWimg] + tuih[TWimg] - y;
    TextWindow(x, y, w, h, txtcolor, 0);
}

void gfx4desp32::TextWindow(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t txtcolor, uint16_t txbcolor,
    uint16_t frcolor) {
    int calcbuf = (getWidth() >> 3) * ((getHeight() >> 4) + 1);
    if (TWInitial) {
        txfcol = (uint16_t*)malloc(calcbuf << 1);
        txtbuf = (uint8_t*)malloc(calcbuf);
        TWInitial = false;
    }
    twen = true;
    for (int n = 0; n < 600; n++) {
        txtbuf[n] = 0;
    }
    twxpos = 0;
    twypos = 0;
    if (w < 24)
        w = 24;
    if (h < 31)
        h = 31;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if ((w + x) > width)
        w = w - ((w + x) - width);
    if ((h + y) > height)
        h = h - ((h + y) - height);
    txtwin = true;
    TWtextcolor(txtcolor);
    txtf = txtcolor;
    txtb = txbcolor;
    txtx = x + 6;
    twcurx = txtx;
    txty = y + 6;
    twcury = txty;
    txtw = w - 8;
    txth = h - 8;
    twtext = "";
    twframe = true;
    twframecol = frcolor;
    chracc = txtw / 9;
    chrdwn = txth / 16;
    PanelRecessed(x, y, w, h, frcolor);
    RectangleFilled(x + 3, y + 3, (x + 3) + (w - 6) - 1, (y + 3) + (h - 6) - 1,
        txbcolor);
    TWimgSet = 0;
}

void gfx4desp32::TWenable(boolean t) { twen = t; }

void gfx4desp32::TextWindowRestore() {
    twen = true;
    uint16_t chracc = txtw / (fsw + 1);
    uint8_t chrdwn = txth / fsh;
    txtwin = true;
    twtext = "";
    uint16_t tcoltw;
    if (TWimgSet) {
        UserImages(TWimage, 0);
    }
    else {
        if (twframe) {
            PanelRecessed(txtx - 6, txty - 6, txtw + 8, txth + 8, twframecol);
            RectangleFilled(txtx - 3, txty - 3, (txtx - 3) + (txtw + 2) - 1,
                (txty - 3) + (txth + 2) - 1, txtb);
        }
        else {
            RectangleFilled(txtx - 3, txty - 3, (txtx - 3) + (txtw + 2) - 1,
                (txty - 3) + (txth + 2) - 1, txtb);
        }
    }
    uint16_t tempc;
    for (int n = 0; n < (chrdwn - 1); n++) {
        yield();
        for (int o = 0; o < chracc; o++) {
            tempc = txtbuf[(n * chracc) + o];
            txtbuf[((n * chracc) + o)] = tempc;
            if (tempc < 32) {
                tempc = 32;
            }
            tcoltw = txfcol[(n * chracc) + o];
            if (TWimgSet) {
                drawChar2TWimage(tempc - 32, TWimage, 0, txtx + ((fsw + 1) * o),
                    txty + (fsh * n), tcoltw);
            }
            else {
                drawChar2tw(txtx + ((fsw + 1) * o), txty + (fsh * n), tempc - 32,
                    tcoltw, txtb, 1);
            }
        }
    }
}

void gfx4desp32::TextWindow(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t txtcolor, uint16_t txbcolor) {
    twen = true;
    if (!(TWimgSet))
        TWimage = -1;
    int calcbuf = (getWidth() >> 3) * ((getHeight() >> 4) + 1);
    if (TWInitial) {
        txfcol = (uint16_t*)malloc(calcbuf << 1);
        txtbuf = (uint8_t*)malloc(calcbuf);
        TWInitial = false;
    }
    for (int n = 0; n < 600; n++) {
        txtbuf[n] = 0;
    }
    if (w < 22)
        w = 22;
    if (h < 29)
        h = 29;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if ((w + x) > width)
        w = w - ((w + x) - width);
    if ((h + y) > height)
        h = h - ((h + y) - height);
    twxpos = 0;
    twypos = 0;
    TWtextcolor(txtcolor);
    txtwin = true;
    txtf = txtcolor;
    txtb = txbcolor;
    txtx = x + 3;
    twcurx = txtx;
    txty = y + 3;
    twcury = txty;
    txtw = w - 6;
    txth = h - 6;
    twtext = "";
    twframe = false;
    chracc = txtw / 9;
    chrdwn = txth / 16;
    if (TWimage == -1)
        RectangleFilled(x, y, x + w - 1, y + h - 1, txbcolor);
}

void gfx4desp32::drawChar2TWimage(uint8_t ch, uint16_t uino, int frames,
    int16_t uxpos, int16_t uypos,
    uint16_t txtcol) {
    UIDRcharOn = 1;
    TWcharByte = 0;
    TWcharBit = 6;
    TWimageCount = ch << 4;
    TWimageTextCol = txtcol;
    UserImagesDR(uino, frames, uxpos - tuix[uino], uypos - tuiy[uino], 8, 16);
}

void gfx4desp32::Panel(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t colorp) {
    RectangleFilled(x + 2, y + 2, (x + 2) + (w - 3) - 1, (y + 2) + (h - 3) - 1,
        colorp);
    uint32_t tcol = bevelColor(colorp);
    uint16_t _dark = tcol >> 16;
    uint16_t _light = tcol & 0xffff;
    Hline(x, y, w, _light);
    Hline(x + 1, y + 1, w - 2, _light);
    Vline(x, y, h, _light);
    Vline(x + 1, y + 1, h - 2, _light);
    Hline(x, y + h - 1, w, _dark);
    Hline(x + 1, y + h - 2, w - 2, _dark);
    Vline(x + w - 1, y, h, _dark);
    Vline(x + w - 2, y + 1, h - 2, _dark);
}

void gfx4desp32::PanelRecessed(int16_t x, int16_t y, int16_t w, int16_t h,
    uint16_t colorpr) {
    RectangleFilled(x, y, x + w - 1, y + h - 1, colorpr);
    uint32_t tcol = bevelColor(colorpr);
    uint16_t _dark = tcol >> 16;
    uint16_t _light = tcol & 0xffff;
    Hline(x + 2, y + 2, w - 4, _dark);
    Vline(x + 2, y + 2, h - 4, _dark);
    Hline(x + 3, y + h - 3, w - 5, _light);
    Vline(x + w - 3, y + 3, h - 5, _light);
}

int16_t gfx4desp32::XYposToDegree(int curX, int curY) {
    int delta, deg, adj;
    if (curY < 0) {
        if (curX < 0) {
            adj = 1;
            deg = 90;
        }
        else {
            adj = 2;
            deg = 180;
        }
    }
    else {
        if (curX < 0) {
            deg = 0;
            adj = 2;
        }
        else {
            deg = 270;
            adj = 1;
        }
    }
    curX = abs(curX);
    curY = abs(curY);
    if (curX < curY) {
        adj &= 1;
    }
    else {
        adj &= 2;
        swap(curX, curY);
    }
    delta = at[(curX * 100) / curY];
    if (adj) {
        deg += 90 - delta;
    }
    else {
        deg += delta;
    }
    return deg;
}

uint16_t gfx4desp32::getNumberofObjects(void) { return gciobjnum; }

void gfx4desp32::ButtonDown(int hndl) {
    uint16_t x = bposx[hndl];
    uint16_t y = bposy[hndl];
    uint16_t w = bposw[hndl];
    uint16_t h = bposh[hndl];
    uint16_t colorbd = bposc[hndl];
    uint32_t tcol = bevelColor(colorbd);
    uint16_t _dark = tcol >> 16;
    if (bxStyle == 0) {
        Hline(x, y, w, colorbd);
        Hline(x + 1, y + 1, w - 2, colorbd);
        Vline(x, y, h, colorbd);
        Vline(x + 1, y + 1, h - 2, colorbd);
        Hline(x, y + h - 1, w, _dark);
        Hline(x + 1, y + h - 2, w - 2, colorbd);
        Vline(x + w - 1, y, h, _dark);
        Vline(x + w - 2, y + 1, h - 2, colorbd);
    }
    if (bxStyle > 0) {
        int nh, nw;
        nw = h / 10;
        if (nw == 0)
            nw = 1;
        if (bxStyle == 1) {
            nh = h / 6;
        }
        if (bxStyle > 1) {
            nh = h >> 1;
        }
        gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
            GRADIENT_RAISED, 0, -1, -1, -1, 0);
    }
}

void gfx4desp32::ButtonUp(int hndl) {
    if (bactive[hndl]) {
        uint16_t x = bposx[hndl];
        uint16_t y = bposy[hndl];
        uint16_t w = bposw[hndl];
        uint16_t h = bposh[hndl];
        uint16_t colorbu = bposc[hndl];
        uint32_t tcol = bevelColor(colorbu);
        uint16_t _dark = tcol >> 16;
        uint16_t _light = tcol & 0xffff;
        if (bxStyle == 0) {
            Hline(x, y, w, _light);
            Hline(x + 1, y + 1, w - 2, _light);
            Vline(x, y, h, _light);
            Vline(x + 1, y + 1, h - 2, _light);
            Hline(x, y + h - 1, w, _dark);
            Hline(x + 1, y + h - 2, w - 2, _dark);
            Vline(x + w - 1, y, h, _dark);
            Vline(x + w - 2, y + 1, h - 2, _dark);
        }
        g2 = 0;
        if (bxStyle > 0) {
            int nh, g1, nw;
            nw = h / 10;
            if (nw == 0)
                nw = 1;
            if (bxStyle == 1) {
                nh = h / 6;
                g1 = -1;
                g2 = 0;
            }
            if (bxStyle > 1) {
                nh = h >> 1;
                g1 = 28;
                g2 = 0;
            }
            if (bxStyle == 3) {
                g1 = 48;
                g2 = 28;
            }
            gradientShape(0, nw, x, y, w - 1, h - 1, nh, nh, nh, nh, 0, _dark,
                GRADIENT_RAISED, g1, -1, -1, -1, 0);
        }
    }
}

/****************************************************************************/
/*!
  @brief  Capture and save to uSD an area of the screen.
  @param  x - left X position in pixels
  @param  y - top Y position in pixels
  @param  w - width
  @param  h - height
  @param  fname - filname for saved image
  @note returns true if successful
*/
/****************************************************************************/
bool gfx4desp32::ScreenCapture(int16_t x, int16_t y, int16_t w, int16_t h,
    String fname) {
#ifndef USE_LITTLEFS_FILE_SYSTEM
    if (!sdok)
        return false;
    if (y < 0) {
        h -= 0 - y;
        y = 0;
    }
    if ((y + h) > height)
        h = height - y;

#ifdef USE_SDMMC_FILE_SYSTEM
    File tempfile;
    if (SD_MMC.exists(fname))
        return false;
    tempfile = SD_MMC.open(fname, FILE_WRITE);
#else
    FsFile tempfile;
    if (uSD.exists(fname))
        return false;
    tempfile = uSD.open(fname, FILE_WRITE);
#endif
    int n, o, wline;
    uint16_t sline[w];
    for (n = 0; n < h; n++) {
        wline = ReadLine(x, y + n, w, sline);
        if (n == 0) {
            tempfile.write(wline >> 8);
            tempfile.write(wline);
            tempfile.write(h >> 8);
            tempfile.write(h);
            tempfile.write(0x10);
            tempfile.write(n);
        }
        for (o = 0; o < wline; o++) {
            if (DisplayType == DISP_INTERFACE_RGB) {
                tempfile.write(sline[o] >> 8);
                tempfile.write(sline[o]);
            }
            else {
                tempfile.write(sline[o]);
                tempfile.write(sline[o] >> 8);
            }
        }
    }
    tempfile.close();
    return true;
#endif
    return false;
}

/****************************************************************************/
/*!
  @brief  Select font style for Font1 characters.
  @param  ctyp - 0 to 6 0 is default no style.
  @note adds effect to system Font1 with size of 3 or more.
*/
/****************************************************************************/
void gfx4desp32::FontStyle(uint8_t ctyp) { fstyle = ctyp % 6; }

/****************************************************************************/
/*!
  @brief  Get the number of the current frame buffer in use.
  @note returns current frame buffer number.
*/
/****************************************************************************/
uint8_t gfx4desp32::GetFrameBuffer() {
    return frame_buffer;
}

/****************************************************************************/
/*!
  @brief  Read the colour value of a pixel within a GCI object.
  @param  inum - GCI object index.
  @param  x - The X position that the pixel would be at if the image was drawn on-screen
  @param  y - The Y position that the pixel would be at if the image was drawn on-screen
  @note returns 16bit colour value.
*/
/****************************************************************************/
uint16_t gfx4desp32::ReadImagePixel(int inum, int x, int y) {
    if ((GCItype == GCI_SYSTEM_USD && (!userImag)) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return 0;
    int yc = y - tuiy[inum];
    int xc = x - tuix[inum];
    if (y < tuiy[inum] || x < tuix[inum]) return 0;
    if (y > tuiy[inum] + tuih[inum] - 1 || x > tuix[inum] + tuiw[inum] - 1) return 0;
    uint32_t ofst = 6;
    ofst += ((gciobjframes[inum] > 0) << 1);
    uint32_t pos = ((yc * tuiw[inum]) << 1) + (xc << 1);
    if (ofst == 8) ofst += (((tuiw[inum] * tuih[inum]) * tuiImageIndex[inum]) << 1);
    GCIreadToBuff(tuiIndex[inum] + ofst + pos, 2);
    return psRAMbuffer1[1] + (psRAMbuffer1[0] << 8);
}

/****************************************************************************/
/*!
  @brief  Copies a horizontal line from GCI object to the current frame buffer.
  @param  inum - GCI object index.
  @param  x - The X position that the line would start if the image was drawn on-screen
  @param  y - The Y position that the line would start if the image was drawn on-screen
  @param  w - The length of the line to be copied
  @note returns nothing.
*/
/****************************************************************************/
void gfx4desp32::CopyImageLine(int inum, int x, int y, int w) {
    if (y > tuiy[inum] + tuih[inum] - 1 || y < 0)
        return;
    if (x > tuix[inum] + tuiw[inum] - 1 || (x + w - 1) < 0)
        return;
    if (w < 0) {
        x += w;
        //w *= -1;
        w = abs(w);
    }
    if (x < tuix[inum]) {
        w += x;
        x = 0;
    }
    //if ((x + w - 1) >= tuix[inum] + tuiw[inum] - 1)
    //    w = tuiw[inum] - x;
    int yc = y - tuiy[inum];
    int xc = x - tuix[inum];
    if ((GCItype == GCI_SYSTEM_USD && (!userImag)) ||
        (GCItype == GCI_SYSTEM_PROGMEM && (!gcidatArray)))
        return;
    uint32_t ofst = 6;
    ofst += ((gciobjframes[inum] > 0) << 1);
    uint32_t pos = ((yc * tuiw[inum]) << 1) + (xc << 1);
    if (ofst == 8) ofst += (((tuiw[inum] * tuih[inum]) * tuiImageIndex[inum]) << 1);
    GCIreadToBuff(tuiIndex[inum] + ofst + pos, w << 1);
    usePushColors = (DisplayType == DISP_INTERFACE_RGB) && (x >= clipx1) &&
        (y >= clipy1) && (x + w <= clipx2) && (y <= clipy2) &&
        (!transalpha) && (!WriteFBonly) && (frame_buffer == 0);
    SetGRAM(x, y, x + w - 1, y);
    //  if (GCItype == GCI_SYSTEM_USD) {
    if (usePushColors && frame_buffer == 0) {
        pushColors(psRAMbuffer1, w);
    }
    else {
        WrGRAMs(psRAMbuffer1, w);
    }
}

/****************************************************************************/
/*!
  @brief  draws a horizontal line to the current frame buffer with color parameter switch.
  @param  x - The X position of the start of the line
  @param  y - The Y position of the start of the line
  @param  w - The length of the line to be drawn
  @param  color - can be a 16bit color or a GCI image or a FrameBuffer
                  HlineX(10, 10, 100, GCI_IMAGE + 3) draws a line at 10, 10 and a length of
                    100 pixels from GCI image index 3. x and y are relative to objects x and y
                    position.
                  HlineX(10, 10, 100, FRAMEBUFFER_IMAGE + 3) draws a line at 10, 10 and a length of
                    100 pixels from frame buffer 3 at the same x and y position.
  @note returns nothing.
*/
/****************************************************************************/
void gfx4desp32::HlineX(int x, int y, int w, int32_t color) {
    if (w < 1) return;
    uint8_t fB = 0;
    int16_t imageNum = -1;
    int gpos = 7 * (gradON == 2);
    if ((color & 0xffff0000) == 0xf00000) fB = color & 0x000f;
    if ((color & 0xffff0000) == 0xf0000) imageNum = color & 0xffff;
    uint16_t fgcol = color;
    if (fB) {
        CopyFrameBufferLine(x, y, w, fB);
    }
    else if (imageNum != -1) {
        CopyImageLine(imageNum, x, y, w);
    }
    else {
        if (gradON) {
            Hline(x, y, w, Grad(grad1[0 + gpos], grad1[1 + gpos], grad1[2 + gpos], grad1[3 + gpos], y - grad1[6 + gpos], fgcol));
        }
        else {
            Hline(x, y, w, fgcol);
        }
    }
}

/****************************************************************************/
/*!
  @brief  draws a vertical line to the current frame buffer with color parameter switch.
  @param  x - The X position of the start of the line
  @param  y - The Y position of the start of the line
  @param  w - The length of the line to be drawn
  @param  color - can be a 16bit color or a GCI image or a FrameBuffer
                  VlineX(10, 10, 100, GCI_IMAGE + 3) draws a line at 10, 10 and a length of
                    100 pixels from GCI image index 3. x and y are relative to objects x and y
                    position.
                  VlineX(10, 10, 100, FRAMEBUFFER_IMAGE + 3) draws a line at 10, 10 and a length of
                    100 pixels from frame buffer 3 at the same x and y position.
  @note returns nothing.
*/
/****************************************************************************/
void gfx4desp32::VlineX(int x, int y, int w, int32_t color) {
    if (w < 1) return;
    uint8_t fB = 0;
    int16_t imageNum = -1;
    int gpos = 7 * (gradON == 2);
    if ((color & 0xffff0000) == 0xf00000) fB = color & 0x000f;
    if ((color & 0xffff0000) == 0xf0000) imageNum = color & 0xffff;
    uint16_t fgcol = color;
    if (fB || imageNum != -1 || (gradON && !gradientVert)) {
        for (int n = 0; n < w; n++) {
            if (fB) {
                fgcol = ReadPixelFromFrameBuffer(x, y + n, fB);
            }
            else {
                fgcol = ReadImagePixel(imageNum, x, y + n);
            }
            if (gradON) fgcol = Grad(grad1[0 + gpos], grad1[1 + gpos], grad1[2 + gpos], grad1[3 + gpos], (y + n) - grad1[6 + gpos], color);
            PutPixel(x, y + n, fgcol);
        }
    }
    else {
        if (gradientVert) fgcol = Grad(grad1[0 + gpos], grad1[1 + gpos], grad1[2 + gpos], grad1[3 + gpos], x - grad1[6 + gpos], color);
        Vline(x, y, w, fgcol);
    }
}

/****************************************************************************/
/*!
  @brief  draws a rectangle to the current frame buffer with color parameter switch.
  @param  x0 - The X position of the top left corner of the rectangle
  @param  y0 - The Y position of the top left corner of the rectangle
  @param  x1 - The X position of the bottom right corner of the rectangle
  @param  y1 - The Y position of the bottom right corner of the rectangle
  @param  color - can be a 16bit color or a GCI image or a FrameBuffer
                  RectangleFilledX(10, 10, 100, 100, GCI_IMAGE + 3) draws a rectangle at 10, 10
                    from GCI image index 3. x and y are relative to objects x and y.
                  RectangleFilledX(10, 10, 100, 100, FRAMEBUFFER_IMAGE + 3) draws a rectangle at 10, 10
                    from framebuffer 3 at the same x and y position.
  @note returns nothing.
*/
/****************************************************************************/
void gfx4desp32::RectangleFilledX(int x0, int y0, int x1, int y1, int32_t color) {
    uint8_t fB = 0;
    int16_t imageNum = -1;
    int w = x1 - x0 + 1;
    int h = y1 - y0 + 1;
    if ((color & 0xffff0000) == 0xf00000) fB = color & 0x000f;
    if ((color & 0xffff0000) == 0xf0000) imageNum = color & 0xffff;
    uint16_t fgcol = color;
    if (fB) {
        DrawFrameBufferArea(fB, x0, y0, x1, y1);
    }
    else if (imageNum != -1) {
        ClipWindow(x0, y0, x1, y1);
        int tclipx1, tclipy1, tclipx2, tclipy2;
        bool tclippingON = clippingON;
        tclipx1 = clipx1; tclipy1 = clipy1; tclipx2 = clipx2; tclipy2 = clipy2;
        Clipping(ON);
        imageShow(imageNum);
        //UserImage(imageNum/*, imageGetWord(imageNum, IMAGE_INDEX)*/);
        if (!tclippingON) Clipping(OFF);
        clipx1 = tclipx1; clipy1 = tclipy1; clipx2 = tclipx2; clipy2 = tclipy2;
        //UserImageDR(imageNum, x0, y0, w, h, x0, y0);
    }
    else {
        RectangleFilled(x0, y0, x1, y1, color);
    }
}

/****************************************************************************/
/*!
  @brief  draws a pixel to the current frame buffer with color parameter switch.
  @param  x - The X position of the pixel
  @param  y - The Y position of the pixel
  @param  color - can be a 16bit color or a GCI image or a FrameBuffer
                  PutPixelAlpha(10, 10, GCI_IMAGE + 3, 255) draws a pixel at 10, 10
                    from GCI image index 3 and full alpha level. x and y are relative to objects x and y.
                  PutPixelAlpha(10, 10, FRAMEBUFFER_IMAGE + 3, 127) draws a pixel at 10, 100
                    from GCI image index 3 with half alpha at the same x and y position.
                  PutPixelAlpha(10, 10, RED, 127) draws a RED pixel at 10, 10
                    with half alpha at the same x and y position.
  @param  alpha - the level of alpha that the pixel is drawn.
  @note returns nothing.
*/
/****************************************************************************/
void gfx4desp32::PutPixelAlpha(int x, int y, int32_t color, uint8_t alpha) {
    uint8_t FB = 0;
    int16_t imageNum = -1;
    uint16_t bg;
    int gpos = 7 * (gradON == 2);
    if ((color & 0xffff0000) == 0xf00000) FB = color & 0x07;
    if ((color & 0xffff0000) == 0xf0000) imageNum = color & 0xffff;
    uint16_t fgcol;
    if (FB) {
        fgcol = ReadPixelFromFrameBuffer(x, y, FB);
    }
    else if (imageNum != -1) {
        fgcol = ReadImagePixel(imageNum, x, y);
    }
    else {
        fgcol = color & 0xffff;
    }
    bg = ReadPixel(x, y); //swin = true;
    if (gradON) {
        if (gradientVert) {
            fgcol = Grad(grad1[0 + gpos], grad1[1 + gpos], grad1[2 + gpos], grad1[3 + gpos], x - grad1[6 + gpos], fgcol);
        }
        else {
            fgcol = Grad(grad1[0 + gpos], grad1[1 + gpos], grad1[2 + gpos], grad1[3 + gpos], y - grad1[6 + gpos], fgcol);
        }
    }
    calcAlpha(fgcol, bg, (uint8_t)alpha);
    PutPixel(x, y, __colour);
}

/****************************************************************************/
/*!
  @brief  Set the frame buffer for drawing functions.
  @param  fbnum - frame buffer number 0 is main 1 to 3 are additional buffers
  @note once set, all drawing functions will be sent to specified frame
    buffer. If frame buffer 0 is set (default) all drawing functions will
    appear immediately on the display.
*/
/****************************************************************************/
void gfx4desp32::DrawToframebuffer(uint8_t fbnum) {
    switch (fbnum) {
    case 0:
        frame_buffer = 0;
        break;
    case 1:
        frame_buffer = 1;
        if (!framebufferInit1)
            AllocateFB(1);
        break;
    case 2:
        frame_buffer = 2;
        if (!framebufferInit2)
            AllocateFB(2);
        break;
    case 3:
        frame_buffer = 3;
        if (!framebufferInit3)
            AllocateFB(3);
        break;
    case 4:
        frame_buffer = 4;
        if (!framebufferInit4)
            AllocateFB(4);
        break;
    }
}

bool gfx4desp32::SpriteInit(uint16_t* sdata, size_t nums) {
    if (msprites < 1)
        return false;
    uint16_t scount = 0;
    int sdatpos = 0;
    int sprsize = 0;
    uint16_t cdpth = 1;
    uint16_t nextpos = 4;
    while (scount <= nums && cdpth > 0) {
        spriteData[sdatpos] = sdata[scount];
        spriteData[sdatpos + 1] = sdata[scount + 1];
        spriteData[sdatpos + 2] = sdata[scount + 2];
        cdpth = sdata[scount + 3];
        spriteData[sdatpos + 3] = nextpos;
        sprsize = (sdata[scount + 1] * sdata[scount + 2]) >> (cdpth - 1);
        if (cdpth == SPRITE_8BIT &&
            ((sdata[scount + 1] * sdata[scount + 2]) % 2) > 0)
            sprsize++;
        if (cdpth == SPRITE_4BIT &&
            ((sdata[scount + 1] * sdata[scount + 2]) % 4) > 0)
            sprsize++;
        nextpos = nextpos + sprsize + 4;
        sdatpos += 4;
        scount = nextpos - 4;
    }
    return true;
}

bool gfx4desp32::SpriteAdd(int pos, int snum, int x, int y, uint16_t* sdata) {
    if (snum > msprites)
        return false;
    int spos = spriteData[(snum << 2) + 3];
    byte coldepth = sdata[spos - 1];
    spriteList[(pos << 3)] = 0;
    spriteList[(pos << 3)] |= (coldepth << 1);
    spriteList[(pos << 3) + SPRITE_MEMPOS] = spos;
    spriteList[(pos << 3) + SPRITE_X] = x;
    spriteList[(pos << 3) + SPRITE_Y] = y;
    spriteList[(pos << 3) + SPRITE_WIDTH] = sdata[spos - 3];
    spriteList[(pos << 3) + SPRITE_HEIGHT] = sdata[spos - 2];
    spriteLast[pos << 1] = x;
    spriteLast[(pos << 1) + 1] = y;
    spriteNum[pos] = snum;
    if (pos + 1 > numSprites)
        numSprites = pos + 1;
    return true;
}

void gfx4desp32::SpriteAreaSet(uint16_t x, uint16_t y, uint16_t x1,
    uint16_t y1) {
    spriteArea[0] = x;
    spriteArea[1] = y;
    spriteArea[2] = x1;
    spriteArea[3] = y1;
    saSet = true;
    if (SpriteBKGfbNUM) DrawFrameBufferArea(SpriteBKGfbNUM, x, y, x1, y1);
}

void gfx4desp32::SpriteUseFrameBufferBackground(int fbnum) {
    SpriteBKGfbNUM = fbnum;
}

void gfx4desp32::SetSprite(int num, bool active, int x, int y, uint16_t bscolor,
    uint16_t* sdata) {
    bool delsprite = false;
    int lxy = num << 1;
    int dxy = num << 3;
    int dsx = spriteList[dxy + SPRITE_X];
    int dsy = spriteList[dxy + SPRITE_Y];
    if (spriteList[(num << 3) + 0] == 1 && active == 0) {
        delsprite = true;
    }
    if (spriteList[dxy] == 0) {
        spriteLast[lxy] = x;
        spriteLast[lxy + 1] = y;
    }
    spriteList[dxy] |= active;
    spriteList[dxy + SPRITE_X] = x;
    spriteList[dxy + SPRITE_Y] = y;
    int16_t tsx, tsy, tsx1, tsy1;
    if (!delsprite) {
        if (spriteLast[lxy] <= x) {
            tsx = spriteLast[lxy];
            tsx1 = x + spriteList[dxy + SPRITE_WIDTH];
        }
        else {
            tsx1 = spriteLast[lxy] + spriteList[dxy + SPRITE_WIDTH];
            tsx = x;
        }
        if (spriteLast[lxy + 1] <= y) {
            tsy = spriteLast[lxy + 1];
            tsy1 = y + spriteList[dxy + SPRITE_HEIGHT];
        }
        else {
            tsy1 = spriteLast[lxy + 1] + spriteList[dxy + SPRITE_HEIGHT];
            tsy = y;
        }
    }
    else {
        tsx = dsx;
        tsy = dsy;
        tsx1 = dsx + spriteList[dxy + SPRITE_WIDTH];
        tsy1 = dsy + spriteList[dxy + SPRITE_HEIGHT];
    }
    spriteLast[lxy] = x;
    spriteLast[lxy + 1] = y;
    if ((((tsx1 - tsx) >> 1) + 1 > spriteList[dxy + SPRITE_WIDTH] ||
        ((tsy1 - tsy) >> 1) + 1 > spriteList[dxy + SPRITE_HEIGHT]) &&
        spriteList[dxy + SPRITE_ACTIVE]) {
        spriteList[dxy + SPRITE_ACTIVE] |= 0;
        SpriteUpdate(dsx, dsy, dsx + spriteList[dxy + SPRITE_WIDTH],
            dsy + spriteList[dxy + SPRITE_HEIGHT], bscolor, sdata);
        spriteList[dxy + SPRITE_ACTIVE] |= 1;
        SpriteUpdate(x, y, x + spriteList[dxy + SPRITE_WIDTH],
            y + spriteList[dxy + SPRITE_HEIGHT], bscolor, sdata);
    }
    else {
        SpriteUpdate(tsx, tsy, tsx1, tsy1, bscolor, sdata);
    }
}

void gfx4desp32::SpriteUpdate(int16_t tsx, int16_t tsy, int16_t tsx1,
    int16_t tsy1, uint16_t bscolor, uint16_t* sdata) {
    saSet = true;
    if (tsx >= width || tsy >= height || tsx1 < 1 || tsy1 < 1)
        return;
    if (tsx < 0)
        tsx = 0;
    if (tsy < 0)
        tsy = 0;
    if (tsx1 >= width)
        tsx1 = width - 1;
    if (tsy1 >= height)
        tsy1 = height - 1;
    spriteArea[0] = tsx;
    spriteArea[1] = tsy;
    spriteArea[2] = tsx1;
    spriteArea[3] = tsy1;
    UpdateSprites(bscolor, sdata);
}

int gfx4desp32::GetSprite(int snum, int choice) {
    return spriteList[(snum << 3) + choice];
}

void gfx4desp32::UpdateSprites(uint16_t bscolor, uint16_t* sdata) {
    if (!saSet)
        return;
    spriteArea[3] = spriteArea[3] + 1;
    SetGRAM(spriteArea[0], spriteArea[1], spriteArea[2], spriteArea[3]);
    StartWrite();

    byte cdepth;
    uint16_t sspos = 0;
    int collide;
    int addr;
    int xo, yo;

    uint16_t wscolor = 0;
    uint16_t cscolor = 0;
    uint16_t tscolor = 0;
    uint16_t bufsp[spriteArea[2] + 20];
    uint16_t count = 0;
    int spriteAPos;
    for (int ns = 0; ns < numSprites; ns++) {
        spriteList[(ns << 3) + 6] = -1;
        spriteList[(ns << 3) + 7] = -1;
    }
    collide = -1;
    for (int y = 0; y < spriteArea[3] - spriteArea[1] + 1; y++) {
        count = 0;
        for (int x = 0; x < spriteArea[2] - spriteArea[0] + 1; x++) {
            if (SpriteBKGfbNUM) {
                wscolor = ReadPixelFromFrameBuffer(spriteArea[0] + x, spriteArea[1] + y, SpriteBKGfbNUM);
            }
            else {
                wscolor = bscolor;
            }
            collide = -1;
            for (int chk = 0; chk < numSprites; chk++) {
                xo = spriteArea[0] + x;
                yo = spriteArea[1] + y;
                addr = chk << 3;
                cdepth = spriteList[addr] >> 1;
                spriteAPos = spriteList[addr + SPRITE_MEMPOS];
                sdetaily = spriteList[addr + SPRITE_Y];
                sdetailh = spriteList[addr + SPRITE_HEIGHT];
                sdetailx = spriteList[addr + SPRITE_X];
                sdetailw = spriteList[addr + SPRITE_WIDTH];
                if ((spriteList[addr] & 0x01) && yo >= sdetaily &&
                    yo <= sdetaily + sdetailh - 1 && xo >= sdetailx &&
                    xo <= sdetailx + sdetailw - 1) {
                    tscolor = sdata[spriteAPos - 4];
                    sspos = ((yo - sdetaily) * sdetailw) + (xo - sdetailx);
                    if (cdepth == SPRITE_16BIT)
                        cscolor = sdata[spriteAPos + sspos];
                    if (cdepth == SPRITE_8BIT) {
                        uint16_t tcscol = sdata[spriteAPos + (sspos >> 1)];
                        cscolor = RGB3322565[(tcscol >> 8 * ((sspos % 2) == 0)) & 0xff];
                    }
                    if (cdepth == SPRITE_4BIT) {
                        uint16_t tcscol = sdata[spriteAPos + (sspos >> 2)];
                        cscolor = palette4bit[(tcscol >> ((3 - (sspos % 4)) * 4)) & 0x0f];
                    }
                    if (cscolor != tscolor) {
                        wscolor = cscolor;
                        if (collide == -1) {
                            collide = chk;
                        }
                        else if (collide != -1) {
                            if (spriteList[(collide << 3) + SPRITE_COLLIDE1] == -1) {
                                spriteList[(collide << 3) + SPRITE_COLLIDE1] = chk;
                                spriteList[addr + SPRITE_COLLIDE1] = collide;
                            }
                            else {
                                spriteList[(collide << 3) + SPRITE_COLLIDE2] = chk;
                                spriteList[addr + SPRITE_COLLIDE1] = collide;
                            }
                            collide = chk;
                        }
                    }
                }
            }
            bufsp[count] = wscolor;
            count++;
        }

        WrGRAMs(bufsp, count);
    }
    EndWrite();
}

void gfx4desp32::SetNumberSprites(uint16_t numspr) { numSprites = numspr; }

int gfx4desp32::GetNumberSprites() { return numSprites; }

int16_t gfx4desp32::GetSpriteImageNum(int snum) { return spriteNum[snum]; }

uint16_t gfx4desp32::SpriteGetPixel(int snum, int xo, int yo, uint16_t tcolor,
    uint16_t* sdata) {
    uint16_t cscolor = 0x0000;
    uint16_t rcolor = tcolor;
    uint16_t sspos;
    byte cdepth;
    int chks, chke;
    if (snum < 0) {
        chks = 0;
        chke = numSprites;
    }
    else {
        chks = snum;
        chke = snum + 1;
    }
    for (int chk = chks; chk < chke; chk++) {
        uint16_t addr = chk << 3;
        cdepth = spriteList[addr] >> 1;
        if (spriteList[addr] && yo >= spriteList[addr + SPRITE_Y] &&
            yo <= spriteList[addr + SPRITE_Y] + spriteList[addr + SPRITE_HEIGHT] -
            1 &&
            xo >= spriteList[addr + SPRITE_X] &&
            xo <=
            spriteList[addr + SPRITE_X] + spriteList[addr + SPRITE_WIDTH] - 1) {
            sspos = ((yo - spriteList[addr + SPRITE_Y]) *
                spriteList[addr + SPRITE_WIDTH]) +
                (xo - spriteList[addr + SPRITE_X]);
            if (cdepth == SPRITE_16BIT)
                cscolor = sdata[spriteList[addr + SPRITE_MEMPOS] + sspos];
            if (cdepth == SPRITE_8BIT) {
                uint16_t tcscol =
                    sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 1)];
                cscolor = RGB3322565[(tcscol >> 8 * ((sspos % 2) == 0)) & 0xff];
            }
            if (cdepth == SPRITE_4BIT) {
                uint16_t tcscol =
                    sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 2)];
                cscolor = palette4bit[(tcscol >> ((3 - (sspos % 4)) * 4)) & 0x0f];
            }
            if (cscolor != tcolor)
                rcolor = cscolor;
        }
    }
    return rcolor;
}

int gfx4desp32::GetSpritesAt(int xo, int yo, uint16_t tcolor, int* slist,
    uint16_t* sdata) {
    uint16_t cscolor = 0x0000;
    uint16_t sspos;
    int snum = 0;
    int r = -1;
    byte cdepth;
    for (int chk = 0; chk < numSprites; chk++) {
        slist[chk] = -1;
        uint16_t addr = chk << 3;
        cdepth = spriteList[addr] >> 1;
        if (spriteList[addr] && yo >= spriteList[addr + SPRITE_Y] &&
            yo <= spriteList[addr + SPRITE_Y] + spriteList[addr + SPRITE_HEIGHT] -
            1 &&
            xo >= spriteList[addr + SPRITE_X] &&
            xo <=
            spriteList[addr + SPRITE_X] + spriteList[addr + SPRITE_WIDTH] - 1) {
            sspos = ((yo - spriteList[addr + SPRITE_Y]) *
                spriteList[addr + SPRITE_WIDTH]) +
                (xo - spriteList[addr + SPRITE_X]);
            if (cdepth == SPRITE_16BIT)
                cscolor = sdata[spriteList[addr + SPRITE_MEMPOS] + sspos];
            if (cdepth == SPRITE_8BIT) {
                uint16_t tcscol =
                    sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 1)];
                cscolor = RGB3322565[(tcscol >> 8 * ((sspos % 2) == 0)) & 0xff];
            }
            if (cdepth == SPRITE_4BIT) {
                uint16_t tcscol =
                    sdata[spriteList[addr + SPRITE_MEMPOS] + (sspos >> 2)];
                cscolor = palette4bit[(tcscol >> ((3 - (sspos % 4)) * 4)) & 0x0f];
            }
            if (cscolor != tcolor) {
                slist[snum] = chk;
                snum++;
                r = snum;
            }
        }
    }
    return r;
}

void gfx4desp32::SpriteEnable(int snum, bool sen) {
    spriteList[snum << 3] |= sen;
}

void gfx4desp32::SpriteSetPalette(int pnumber, uint16_t plcolor) {
    palette4bit[pnumber % 16] = plcolor;
}

uint16_t gfx4desp32::SpriteGetPalette(int pnumber) {
    return palette4bit[pnumber % 16];
}

void gfx4desp32::SetMaxNumberSprites(uint16_t snos) {
    snos = snos << 1;
    uint16_t bytes = (uint16_t)snos << 2;
    if ((spriteData = (int16_t*)malloc(bytes))) {
        memset(spriteData, 0, bytes);
    }
    bytes = (uint16_t)snos << 3;
    if ((spriteList = (int16_t*)malloc(bytes))) {
        memset(spriteList, 0, bytes);
    }
    bytes = (uint16_t)snos << 1;
    if ((spriteLast = (int16_t*)malloc(bytes))) {
        memset(spriteLast, 0, bytes);
    }
    bytes = (uint16_t)snos;
    if ((spriteNum = (int16_t*)malloc(bytes))) {
        memset(spriteNum, -1, bytes);
    }
    msprites = snos >> 1;
}

void gfx4desp32::Opacity(bool opacity) {
    if (opacity == false) {
        opacitystate = false;
    }
    else {
        opacitystate = true;
    }
}

void gfx4desp32::putstr(String strg) { print(strg); }

void gfx4desp32::putstr(const char* strg) { print(strg); }

void gfx4desp32::putstrXY(int xpos, int ypos, String strg) {
    MoveTo(xpos, ypos);
    print(strg);
}

void gfx4desp32::putstrXY(int xpos, int ypos, const char* strg) {
    MoveTo(xpos, ypos);
    print(strg);
}

void gfx4desp32::putch(char chr) { write(chr); }

void gfx4desp32::putchXY(int xpos, int ypos, char chr) {
    MoveTo(xpos, ypos);
    write(chr);
}

int gfx4desp32::__gciCharWidth(uint16_t ch) {
    if (fno == 0) {
        gciFont.seek(ch * fsb + 8); // character offset 
        // (number of bytes per character * character value) +  8-byte header
        return gciFont.read() << 8 | gciFont.read();
    }
    else if (fno == -1) {
        if (fntCmprs) {
            if (fontPtr[FONT_TYPE]) {
                return fntData[(ch - fso) * fsb];
            }
            else {
                return fsw;
            }
        }
        else {
            int i = ch * fsb + 8;
            return fontPtr[i] << 8 | fontPtr[i + 1];
        }
    }
    return fsw;
}

int gfx4desp32::charWidth(uint16_t ch) {
    if (fno != 0 && fno != -1)
        return (fsw + 1) * textsize;
    return __gciCharWidth(ch);
}

int gfx4desp32::charHeight(uint16_t ch) { return fsh * textsize; }

int gfx4desp32::strWidth(String ts) {
    size_t len = ts.length();
    if (fno != 0 && fno != -1)
        return len * ((fsw + 1) * textsize);
    int width = 0;
    for (size_t i = 0; i < len; i++) {
        width += __gciCharWidth(ts.charAt(i));
    }
    return width;
}

int gfx4desp32::strWidth(char* ts) {
    size_t len = strlen(ts);
    if (fno != 0 && fno != -1)
        return len * ((fsw + 1) * textsize);
    int width = 0;
    for (size_t i = 0; i < len; i++) {
        width += __gciCharWidth(ts[i]);
    }
    return width;
}

void gfx4desp32::imageSetWord(uint16_t img, byte controlIndex, int val1,
    int val2) {
    switch (controlIndex) {
    case IMAGE_XPOS:
        tuix[img] = val1;
        break;
    case IMAGE_YPOS:
        tuiy[img] = val1;
        break;
    case IMAGE_XYPOS:
        tuix[img] = val1;
        tuiy[img] = val2;
        break;
    case IMAGE_INDEX:
        tuiImageIndex[img] = val1;
        break;
    }
}

int gfx4desp32::imageGetWord(uint16_t img, byte controlIndex) {
    int retval = -1;
    switch (controlIndex) {
    case IMAGE_XPOS:
        retval = tuix[img];
        break;
    case IMAGE_YPOS:
        retval = tuiy[img];
        break;
    case IMAGE_WIDTH:
        retval = tuiw[img];
        break;
    case IMAGE_HEIGHT:
        retval = tuih[img];
        break;
    case IMAGE_INDEX:
        retval = tuiImageIndex[img];
        break;
    }
    return retval;
}

int16_t gfx4desp32::getImageValue(uint16_t ui) { return tuiImageIndex[ui]; }

void gfx4desp32::DownloadFile(String WebAddr, String Fname) {
    Download(WebAddr, 0, "", Fname, false, empty);
}

void gfx4desp32::DownloadFile(String WebAddr, String Fname, const char* sha1) {
    Download(WebAddr, 0, "", Fname, true, sha1);
}

void gfx4desp32::DownloadFile(String Address, uint16_t port, String hfile,
    String Fname) {
    Download(Address, port, hfile, Fname, false, empty);
}

void gfx4desp32::Download(String Address, uint16_t port, String hfile,
    String Fname, bool certUsed, const char* sha1) {
#ifndef USE_LITTLEFS_FILE_SYSTEM
    bool error = false;
    int16_t errornum = 0;
    int8_t retries = 3;
    dlok = false;
    int32_t lens;
    uint8_t buffDL[512] = { 0 };
#ifdef USE_SDMMC_FILE_SYSTEM
    File Dwnload;
#else
    FsFile Dwnload;
#endif
    HTTPClient http;
    WiFiClient client;
    WiFiClientSecure clientS;
    WiFiClient* stream;
    while (retries--) {
        if (port > 0) {
            http.begin(client, Address, port, hfile);
        }
        else {
            if (!certUsed) {
                http.begin(client, Address);
            }
            else {
                clientS.setCACert(sha1);
                http.begin(clientS, Address);
            }
        }
        int httpCode = http.GET();
        if (httpCode == 404 || httpCode == 403) {
            errornum = httpCode;
            error = true;
            goto skipDownload;
        }
        if (sdok == false) {
            errornum = -1;
            error = true;
            goto skipDownload;
        }
        lens = http.getSize();
        if (lens == 0) {
            errornum = -2;
            error = true;
            goto skipDownload;
        }
#ifdef USE_SDMMC_FILE_SYSTEM
        if (SD_MMC.exists((char*)Fname.c_str())) {
            SD_MMC.remove((char*)Fname.c_str());
        }
        Dwnload = SD_MMC.open((char*)Fname.c_str(), FILE_WRITE);
#else
        if (uSD.exists((char*)Fname.c_str())) {
            uSD.remove((char*)Fname.c_str());
        }
        Dwnload = uSD.open((char*)Fname.c_str(), FILE_WRITE);
#endif
        stream = http.getStreamPtr();
        while (http.connected() && (lens > 0 || lens == -1)) {
            size_t size = stream->available();
            if (size) {
                int cstream = stream->readBytes(
                    buffDL, ((size > sizeof(buffDL)) ? sizeof(buffDL) : size));
                if (!Dwnload.write(buffDL, cstream)) {
                    delay(500);
                    error = true;
                    break;
                }
                if (lens > 0) {
                    lens -= cstream;
                }
            }
        }
        http.end();
        if (Dwnload.size() == 0) {
            errornum = -3;
            error = true;
            goto skipDownload;
        }
    skipDownload:
        if (!error) {
            break;
        }
        http.end();
        clientS.stop();
        delay(500);
        lens = 0;
    }
    if (error) {
        if (errornum > 0) {
            print("HTTP error ");
            println(errornum);
        }
        else {
            if (errornum == -1) {
                println("SD Error");
            }
            if (errornum == -2) {
                println("Size Error");
            }
            if (errornum == -3) {
                println("0 Bytes Rec Error");
            }
            Dwnload.close();
#ifdef USE_SDMMC_FILE_SYSTEM
            if (SD_MMC.exists((char*)Fname.c_str())) {
                SD_MMC.remove((char*)Fname.c_str());
            }
#else
            if (uSD.exists((char*)Fname.c_str())) {
                uSD.remove((char*)Fname.c_str());
            }
#endif
            delay(500);
            dlok = false;
        }
    }
    else {
        Dwnload.close();
        dlok = true;
    }
    http.end();
    return;
#endif
}

void gfx4desp32::PrintImageFile(String ifile) {
    if (cursor_x >= (width - 1))
        return;
    boolean tempnl = false;
    if (nl) {
        tempnl = true;
        newLine(lastfsh, textsizeht, 0);
    }
    if (cursor_y > (height - 1) && (sEnable == false))
        return;
#ifndef USE_LITTLEFS_FILE_SYSTEM
#ifdef USE_SDMMC_FILE_SYSTEM
    dataFile = SD_MMC.open(ifile);
#else
    dataFile = uSD.open(ifile);
#endif
#else
    ifile = "/" + ifile;
    dataFile = LittleFS.open((char*)ifile.c_str(), "r");
#endif
    if (!dataFile)
        return;
    if (DATAsize() < 6)
        return;
    Piwidth = (DATAread() << 8);
    Piwidth = Piwidth + DATAread();
    Piheight = (DATAread() << 8);
    Piheight = Piheight + DATAread();
    uint8_t mul = DATAread() / 8;
    DATAread(); // dummy read to move pointer
    uint32_t pos = 6;
    uint16_t ichunk = Piwidth << (mul - 1);
    uint8_t buf[width << (mul - 1)];
    if (sEnable == false) {
        if (((cursor_y + Piheight) - 1) > height - 1) {
            Piheight = Piheight - ((cursor_y + Piheight) - height);
        }
    }
    boolean off = false;
    int cuiw = Piwidth;
    if ((cursor_x + Piwidth - 1) >= width) {
        cuiw = Piwidth - ((cursor_x + Piwidth - 1) - width) - 1;
        off = true;
    }
    for (int idraw = 0; idraw < Piheight; idraw++) {
        nl = true;
        newLine(1, 1, cursor_x);
        if ((cursor_y - 1) < 0) {
            setGRAM(cursor_x, cursor_y + height - 1, cursor_x + cuiw - 1,
                cursor_y + height - 1);
        }
        else {
            setGRAM(cursor_x, cursor_y - 1, cursor_x + cuiw - 1, cursor_y - 1);
        }
        if (off) {
            DATAread(buf, cuiw << (mul - 1));
            WrGRAMs(buf, cuiw << (mul - 1));
            pos = pos + (Piwidth << (mul - 1));
            DATAseek(pos);
        }
        else {
            DATAread(buf, ichunk);
            WrGRAMs(buf, ichunk);
        }
    }
    if (tempnl) {
        nl = true;
        lastfsh = 1;
    }
}

void gfx4desp32::UserCharacterBG(uint32_t* data, uint8_t ucsize, int16_t ucx,
    int16_t ucy, uint16_t color, boolean draw,
    uint32_t bgindex) {
    UserCharacterBG(data, ucsize, ucx, ucy, color, draw, bgindex, true, 0);
}

void gfx4desp32::UserCharacterBG(int8_t ui, uint32_t* data, uint8_t ucsize,
    int16_t ucx, int16_t ucy, uint16_t color,
    boolean draw) {
    UserCharacterBG(data, ucsize, ucx, ucy, color, draw, tuiIndex[ui], false, ui);
}

void gfx4desp32::UserCharacterBG(uint32_t* data, uint8_t ucsize, int16_t ucx,
    int16_t ucy, uint16_t color, boolean draw,
    uint32_t bgindex, bool type, int8_t ui) {
    if ((!dataFile && type) || (!userImag && !type) ||
        GCItype != GCI_SYSTEM_USD) {
        return;
    }
    if (ucx < 0 || ucy < 0)
        return;
    uint16_t bwidth;
    if (type) {
        dataFile.seek(bgindex);
        bwidth = (dataFile.read() << 8) + dataFile.read();
    }
    else {
        bwidth = tuiw[ui];
    }
    uint32_t bgoff = bgindex + 6 + (((ucy * bwidth) + ucx) << 1);
    uint8_t left = 0;
    uint32_t tdw;
    uint32_t pix1;
    uint32_t pix2;
    tdw = *data++;
    uint8_t ucwidth = tdw;
    tdw = *data++;
    uint8_t ucheight = tdw;
    if ((ucx + ucwidth - 1) > (width - 1) || (ucy + ucheight - 1) > (height - 1))
        return;
    uint16_t ucloop = (ucwidth * ucheight) >> 1;
    setGRAM(ucx, ucy, ucx + ucwidth - 1, ucy + ucheight - 1);
    uint32_t test2 = 0;
    uint16_t bgbuf[ucloop << 1];
    tdw = *data++;
    for (int c = 0; c < ucloop; c++) {
        test2 = ((tdw >> (ucwidth - 1)) - left) & 0x1;
        if (test2 == 1 && draw) {
            pix1 = color;
        }
        else {
            if (type) {
                dataFile.seek(bgoff + (left << 1));
                pix1 = (dataFile.read() << 8) + dataFile.read();
            }
            else {
                userImag.seek(bgoff + (left << 1));
                pix1 = (userImag.read() << 8) + userImag.read();
            }
        }
        left++;
        test2 = ((tdw >> (ucwidth - 1)) - left) & 0x1;
        if (test2 == 1 && draw) {
            pix2 = color;
        }
        else {
            if (type) {
                dataFile.seek(bgoff + (left << 1));
                pix2 = (dataFile.read() << 8) + dataFile.read();
            }
            else {
                userImag.seek(bgoff + (left << 1));
                pix2 = (userImag.read() << 8) + userImag.read();
            }
        }
        left++;
        if (left > (ucwidth - 1)) {
            left = 0;
            bgoff = bgoff + (bwidth << 1);
            tdw = *data++;
        }
        bgbuf[c << 1] = pix1;
        bgbuf[(c << 1) + 1] = pix2;
    }
    WrGRAMs(bgbuf, ucloop << 1);
}

void gfx4desp32::UserCharacter(uint32_t* data, uint8_t ucsize, int16_t ucx,
    int16_t ucy, uint16_t color, uint16_t bgcolor) {
    uint8_t top = 0;
    uint8_t left = 0;
    uint32_t tdw;
    uint32_t pix1;
    uint32_t pix2;
    int mx;
    int my;
    tdw = *data++;
    uint8_t ucwidth = tdw;
    tdw = *data++;
    uint8_t ucheight = tdw;
    uint16_t ucloop = (ucwidth * ucheight) >> 1;
    uint32_t test2 = 0;
    tdw = *data++;
    if (ucx > -1 && ucy > -1 && (ucx + ucwidth - 1) < (width) &&
        (ucy + ucheight - 1) < (height)) {
        setGRAM(ucx, ucy, ucx + ucwidth - 1, ucy + ucheight - 1);
        for (int c = 0; c < ucloop; c++) {
            test2 = ((tdw >> (ucwidth - 1)) - left) & 0x1;
            pix1 = bgcolor;
            if (test2 == 1)
                pix1 = color;
            left++;
            test2 = ((tdw >> (ucwidth - 1)) - left) & 0x1;
            pix2 = bgcolor;
            if (test2 == 1)
                pix2 = color;
            left++;
            if (left > (ucwidth - 1)) {
                left = 0;
                tdw = *data++;
            }
            WrGRAM(pix1);
            WrGRAM(pix2);
        }
    }
    else {
        for (int c = 0; c < ucloop; c++) {
            mx = ucx + left;
            my = ucy + top;
            test2 = ((tdw >> (ucwidth - 1)) - left) & 0x1;
            if (mx > -1 && my > -1 && mx < width && my < height) {
                if (test2 == 1) {
                    PutPixel(mx, my, color);
                }
                else {
                    PutPixel(mx, my, bgcolor);
                }
            }
            left++;
            mx = ucx + left;
            test2 = ((tdw >> (ucwidth - 1)) - left) & 0x1;
            if (mx > -1 && my > -1 && mx < width && my < height) {
                if (test2 == 1) {
                    PutPixel(mx, my, color);
                }
                else {
                    PutPixel(mx, my, bgcolor);
                }
            }
            left++;
            if (left > (ucwidth - 1)) {
                left = 0;
                top++;
                tdw = *data++;
            }
        }
    }
}

bool gfx4desp32::CheckSD(void) { return sdok; }

bool gfx4desp32::CheckDL(void) { return dlok; }

void gfx4desp32::setCacheSize(uint32_t cs) { DRcache = cs; }

void gfx4desp32::setGCIsystem(uint8_t gs) {
    if (gs <= GCI_SYSTEM_SPECIAL) {
        Close4dGFX();
        GCItype = gs;
    }
}

uint8_t gfx4desp32::getGCIsystem() { return GCItype; }

void gfx4desp32::GCIreadToBuff(uint32_t Index, uint32_t len) {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        userImag.seek(Index);
        userImag.read(psRAMbuffer1, len);
        break;
    case GCI_SYSTEM_PROGMEM:
        gciArrayPos = Index;
        memcpy(psRAMbuffer1, GCIarray + gciArrayPos, len);
        gciArrayPos += len;
        break;
    }
}

void gfx4desp32::GCIreadToBuff2(uint32_t Index, uint32_t len) {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        userImag.seek(Index);
        userImag.read(psRAMbuffer2, len);
        break;
    case GCI_SYSTEM_PROGMEM:
        gciArrayPos = Index;
        memcpy(psRAMbuffer2, GCIarray + gciArrayPos, len);
        gciArrayPos += len;
        break;
    }
}

void gfx4desp32::GCIreadToBuff2(uint32_t Index, uint32_t pos, uint32_t len) {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        userImag.seek(Index);
        userImag.read(psRAMbuffer2 + pos, len);
        break;
    case GCI_SYSTEM_PROGMEM:
        gciArrayPos = Index;
        memcpy(psRAMbuffer2 + pos, GCIarray + gciArrayPos, len);
        gciArrayPos += len;
        break;
    }
}

void gfx4desp32::GCIreadToBuf(uint32_t Index, uint32_t len) {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        userImag.read(psRAMbuffer1 + Index, len);
        break;
    case GCI_SYSTEM_PROGMEM:
        memcpy(psRAMbuffer1 + Index, GCIarray + gciArrayPos, len);
        gciArrayPos += len;
        break;
    }
}

void gfx4desp32::GCIread(uint8_t* dest, uint32_t len) {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        userImag.read(dest, len);
        break;
    case GCI_SYSTEM_PROGMEM:
        memcpy(dest, GCIarray + gciArrayPos, len);
        gciArrayPos += len;
        break;
    }
}

void gfx4desp32::DATAreadToBuff(uint32_t Index, uint32_t len) {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        dataFile.seek(Index);
        dataFile.read(psRAMbuffer1, len);
        break;
    case GCI_SYSTEM_PROGMEM:
        gciArrayPos = Index;
        memcpy(psRAMbuffer1, GCIarray + gciArrayPos, len);
        gciArrayPos += len;
        break;
    }
}

void gfx4desp32::DATAread(uint8_t* dest, uint32_t len) {
    dataFile.read(dest, len);
}

int16_t gfx4desp32::GCIread() {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        return (userImag.read());
        break;
    case GCI_SYSTEM_PROGMEM:
        return GCIarray[gciArrayPos++];
        break;
    }
    return -1;
}

int16_t gfx4desp32::DATread() { return (userDat.read()); }

int16_t gfx4desp32::DATAread() { return (dataFile.read()); }

void gfx4desp32::GCIseek(uint32_t Index) {
    switch (GCItype) {
    case GCI_SYSTEM_USD:
        userImag.seek(Index);
        break;
    case GCI_SYSTEM_PROGMEM:
        gciArrayPos = Index;
        break;
    }
}

void gfx4desp32::DATAseek(uint32_t Index) { dataFile.seek(Index); }

uint32_t gfx4desp32::DATAsize() { return dataFile.size(); }

void gfx4desp32::Open4dGFX(const uint8_t* DATa, uint32_t DATlen,
    const uint8_t* GCIa, uint32_t GCIlen) {
    DATarray = DATa;
    GCIarray = GCIa;
    datArraySize = DATlen;
    gciArraySize = GCIlen;
    GCItype = GCI_SYSTEM_PROGMEM;
    Open4dGFX("gfx4dDummy");
}

void gfx4desp32::UserImageHide(int hndl) { UserImageHide(hndl, BLACK); }

void gfx4desp32::UserImageHide(int hndl, uint16_t color) {
    if (hndl > 0) {
        RectangleFilled(tuix[hndl], tuiy[hndl], tuiw[hndl], tuih[hndl], color);
    }
    else {
        for (int n = 0; n > MAX_WIDGETS; n++) {
            RectangleFilled(tuix[n], tuiy[n], tuiw[n], tuih[n], color);
        }
    }
}

void gfx4desp32::UserImageHideBG(int hndl, int objBG) {
    if (hndl > 0) {
        UserImageDR(objBG, tuix[hndl], tuiy[hndl], tuiw[hndl], tuih[hndl],
            tuix[hndl], tuiy[hndl]);
    }
    else {
        for (int n = 0; n > MAX_WIDGETS; n++) {
            UserImageDR(objBG, tuix[n], tuiy[n], tuiw[n], tuih[n], tuix[n], tuiy[n]);
        }
    }
}