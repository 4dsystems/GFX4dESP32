#include "gfx4desp32_rgb_panel.h"

#define swap(a, b)                                                             \
    {                                                                          \
        int16_t tab = a;                                                       \
        a = b;                                                                 \
        b = tab;                                                               \
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

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_types.h"
#include "esp_err.h"
#include "esp_log.h"
// #include "esp_psram.h"
#include "soc/lcd_periph.h"
#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "rom/cache.h"
#include "esp_rom_gpio.h"

gfx4desp32_rgb_panel::gfx4desp32_rgb_panel(
    esp_lcd_rgb_panel_config_t* panel_config, int bk_pin, int bk_on_level,
    int bk_off_level, int sd_gpio_sck, int sd_gpio_miso, int sd_gpio_mosi,
    int sd_gpio_cs, bool touchYinvert, uint8_t tType)
    : gfx4desp32() {

    this->panel_config = panel_config;
    this->bk_config.pin = bk_pin;
    this->bk_config.on_level = bk_on_level;
    this->bk_config.off_level = bk_off_level;
    sd_sck = sd_gpio_sck;
    sd_miso = sd_gpio_miso;
    sd_mosi = sd_gpio_mosi;
    sd_cs = sd_gpio_cs;
    // backlight = bk_pin;
    __TImode = touchYinvert;
}

gfx4desp32_rgb_panel::~gfx4desp32_rgb_panel() {}

void gfx4desp32_rgb_panel::DisplayControl(uint8_t cmd) {
    switch (cmd) {
    case DISP_CTRL_RE_INIT:
        esp_lcd_panel_reset(panel_handle);
        digitalWrite(40, LOW);
        delay(400);
        digitalWrite(40, HIGH);
        esp_lcd_panel_init(panel_handle);
        delay(300);
        digitalWrite(40, LOW);
        delay(400);
        digitalWrite(40, HIGH);
        rgb_panel = __containerof(panel_handle, esp_rgb_panel_t, base);;
        break;
    case DISP_CTRL_RESET:
        esp_lcd_panel_reset(panel_handle);
        break;
    case DISP_CTRL_NEW:
        ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(panel_config, &panel_handle));
        break;
    case DISP_CTRL_INIT:
        esp_lcd_panel_init(panel_handle);
        break;
    case DISP_CTRL_STOP:
        gdma_reset(rgb_panel->dma_chan);
        lcd_ll_stop(rgb_panel->hal.dev);
        lcd_ll_fifo_reset(rgb_panel->hal.dev);
        break;
    case DISP_CTRL_START_TX:
        __start_transmission();
        break;
    case DISP_CTRL_DEL:
        esp_lcd_panel_del(panel_handle);
        break;
    case DISP_CTRL_START:
        gdma_start(rgb_panel->dma_chan, (intptr_t)rgb_panel->dma_nodes);
        esp_rom_delay_us(1);
        lcd_ll_start(rgb_panel->hal.dev);
        break;
    case DISP_CTRL_FLUSH:
        FlushArea(0, __scrHeight - 1, -1);
        break;
    }
}


esp_lcd_panel_handle_t gfx4desp32_rgb_panel::__begin() {

    ESP_LOGI(TAG, "Turn off LCD Backlight");
    gpio_config_t bk_gpio_config = {
        .pin_bit_mask = 1ULL << bk_config.pin,
        .mode = GPIO_MODE_OUTPUT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Install RGB LCD panel driver");

    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(panel_config, &panel_handle));

    ESP_LOGI(TAG, "Initialize RGB LCD panel");

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    if (__TImode == DISP_TOUCH_NORMAL) {
        digitalWrite(40, LOW);
        delay(400);
        digitalWrite(40, HIGH);
    }
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    delay(300);
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    if (__TImode == DISP_TOUCH_NORMAL) {
        digitalWrite(40, LOW);
        delay(400);
        digitalWrite(40, HIGH);
    }
    ESP_LOGI(TAG, "Turn on LCD Backlight");
    gpio_set_level((gpio_num_t)bk_config.pin, bk_config.on_level);

    rgb_panel = __containerof(panel_handle, esp_rgb_panel_t, base);
    /*** Set some Initial variables ***/
    clipX1pos = 0;
    clipY1pos = 0;
    clipX2pos = (int)rgb_panel->timings.h_res - 1;
    clipY2pos = (int)rgb_panel->timings.v_res - 2;
    ClipWindow(0, 0, (int)rgb_panel->timings.h_res - 1,
        (int)rgb_panel->timings.v_res - 2);
    Clipping(true);
    Clipping(false);
    __scrWidth = (int)rgb_panel->timings.h_res << 1;
    __scrHeight = (int)rgb_panel->timings.v_res - 1;
    __fbSize = (rgb_panel->timings.v_res - 1) * __scrWidth;
    __width = rgb_panel->timings.h_res;
    __height = rgb_panel->timings.v_res - 1;
    /*** Scroll window set to maximum for GFX4dESP32 compatibilty ***/
    scroll_X1 = 0;
    scroll_Y1 = 0;
    scroll_X2 = __width - 1;
    scroll_Y2 = __height - 1;

    ledcSetup(backlight, 25000, 10);
    ledcAttachPin(bk_config.pin, backlight);

    Contrast(15);
    if (I2CInit == false) {
        Wire.begin(17, 18);
        Wire.setClock(400000);
        I2CInit = true;
    }
    PinMode(GFX4d_TOUCH_RESET, OUTPUT);
    PinMode(GFX4d_TOUCH_INT, INPUT);
    IOexpInit = true;
    DisplayType = DISP_INTERFACE_RGB;
    return panel_handle;
}

/****************************************************************************/
/*!
  @brief  Set display brightness
  @param  cntrst    0 - 15
  @note   ESP32 requires pin setup for Analog Write.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::Contrast(int ctrst) {
    if (ctrst > 15)
        ctrst = 15;
    if (ctrst < 0)
        ctrst = 0;
    switch (ctrst) {
    case 15:
        ledcWrite(backlight, 1023);
        break;
    case 14:
        ledcWrite(backlight, 820);
        break;
    case 13:
        ledcWrite(backlight, 608);
        break;
    case 12:
        ledcWrite(backlight, 460);
        break;
    case 11:
        ledcWrite(backlight, 352);
        break;
    case 10:
        ledcWrite(backlight, 276);
        break;
    case 9:
        ledcWrite(backlight, 224);
        break;
    case 8:
        ledcWrite(backlight, 188);
        break;
    case 7:
        ledcWrite(backlight, 140);
        break;
    case 6:
        ledcWrite(backlight, 105);
        break;
    case 5:
        ledcWrite(backlight, 78);
        break;
    case 4:
        ledcWrite(backlight, 62);
        break;
    case 3:
        ledcWrite(backlight, 47);
        break;
    case 2:
        ledcWrite(backlight, 35);
        break;
    case 1:
        ledcWrite(backlight, 28);
        break;
    case 0:
        ledcWrite(backlight, 0);
        break;
    }
}

/****************************************************************************/
/*!
  @brief  Switch on / off backlight
  @param  blight - true / false
  @note  compatible function for early IoD's. Sets backlight fully on.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::BacklightOn(bool blight) {
    if (blight) {
        ledcWrite(backlight, 1023);
    }
    else {
        ledcWrite(backlight, 0);
    }
}

/****************************************************************************/
/*!
  @brief  Set Screen Orientation - resets scroll and clip window to maximum
  @param  r Rotation 0 - 3 (LANDSCAPE, LANDSCAPE_R, PORTRAIT, PORTRAIT_R)
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::panelOrientation(uint8_t r) {
    rotation = r % 4;
    if (rotation < 2) {
        ClipWindow(0, 0, (int)rgb_panel->timings.h_res - 1,
            (int)rgb_panel->timings.v_res - 2);
        Clipping(1);
        Clipping(0);
        __width = rgb_panel->timings.h_res;
        __height = rgb_panel->timings.v_res - 1;
    }
    else {
        ClipWindow(0, 0, (int)rgb_panel->timings.v_res - 2,
            (int)rgb_panel->timings.h_res - 1);
        Clipping(1);
        Clipping(0);
        __width = rgb_panel->timings.v_res - 1;
        __height = rgb_panel->timings.h_res;
    }
    clippingON = false;
    wrGRAM = 0;
    scroll_X1 = 0;
    scroll_Y1 = 0;
    scroll_X2 = __width - 1;
    scroll_Y2 = __height - 1;
}

/**********************************************************************/
/*!
  @brief    Get height in current Orientation
  @returns  __height height in pixels
*/
/**********************************************************************/
int16_t gfx4desp32_rgb_panel::getHeight(void) { return __height; }

/**********************************************************************/
/*!
  @brief    Get width in current Orientation
  @returns  __width width in pixels
*/
/**********************************************************************/
int16_t gfx4desp32_rgb_panel::getWidth(void) { return __width; }

/**********************************************************************/
/*!
  @brief    Get Maximum Y scroll window value in current Orientation
  @returns  scroll_Y2  position in pixels
  @note     Compatipble GFX4dESP32 function used by write and newline.
*/
/**********************************************************************/
int16_t gfx4desp32_rgb_panel::getScrollareaY1() { return (int16_t)scroll_Y2; }

/****************************************************************************/
/*!
  @brief    Get current Screen Orientation
  @returns  rotation 0 - 3 (LANDSCAPE, LANDSCAPE_R, PORTRAIT, PORTRAIT_R)
*/
/****************************************************************************/
uint8_t gfx4desp32_rgb_panel::getPanelOrientation(void) { return rotation; }

/****************************************************************************/
/*!
  @brief    Set Invert mode
  @param    Inv    true / false
@note     Needs coding. Not sure how to deal with this on RGB displays
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::Invert(bool Inv) {
    /*** Not sure how to deal with this on RGB displays ***/
}

/****************************************************************************/
/*!
  @brief  Set GRAM window ready for wrGRAM/s functions.
  @param  x1 left X position in pixels
  @param  y1 top Y position in pixels
  @param  x2 right X position in pixels
  @param  y2 bottom Y position in pixels
  @note   sets global variables GRAMx1, GRAMy1, GRAMx2, GRAMy2 to window pos
          sets GRAMxpos to x1 start and GRAMypos to y1 start
          resets PixelPos to 0 and sets pixel count to size of window
          sets flag wrGRAM to true indicating GRAM window set.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::SetGRAM(int16_t x1, int16_t y1, int16_t x2,
    int16_t y2) {
    GRAMx1 = x1;
    GRAMy1 = y1;
    GRAMx2 = x2;
    GRAMy2 = y2;
    wrGRAM = true;
    GRAMxpos = x1;
    GRAMypos = y1;
    pixelPos = 0;
    pixelCount = (GRAMx2 - GRAMx1) * (GRAMy2 - GRAMy1);
}

/****************************************************************************/
/*!
  @brief  Set start write condition
  @param  none
  @note   StartWrite disables flushing after frame buffer write.
          Flush area will be updated by subsequent writes so that EndWrite
          will flush all affected
*/
/****************************************************************************/
bool gfx4desp32_rgb_panel::StartWrite() {
    if (writeFBonly)
        return false; // return if already initiated
    writeFBonly = true;
    WriteFBonly = true;
    low_Y = rgb_panel->timings.v_res - 2;
    high_Y = 0;
    high_ypos = low_Y;
    return true;
}

/****************************************************************************/
/*!
  @brief  Set end write condition
  @param  none
  @note   EndWrite will enable flush functions and flush the entire area
          that needs to be flushed
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::EndWrite() {
    writeFBonly = false;
    WriteFBonly = false;
    if (rgb_panel->flags.fb_in_psram) {
        high_Y++;
        if (high_Y > __scrHeight - 1)
            high_Y = __scrHeight - 1;
        if (low_Y < 0)
            low_Y = 0;
        if (low_Y > high_Y)
            return;
        if (low_Y > __scrHeight)
            flush_pending = false;
        uint32_t bytes_to_flush = (uint32_t)(high_Y - low_Y) * __scrWidth;
        Cache_WriteBack_Addr((uint32_t)(rgb_panel->fb + (low_Y * __scrWidth)),
            bytes_to_flush);
    }
    if (!rgb_panel->flags.stream_mode) {
        __start_transmission();
    }
    esp_rom_delay_us(1);
}

/****************************************************************************/
/*!
  @brief  Fill entire frame buffer with colour and flush
  @param  color 16 bit RGB565 colour
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::FillScreen(uint16_t color) {
    uint32_t total = __fbSize >> 1; // total size of framebuffer in pixels
    uint8_t* pto = SelectFB(frame_buffer);
    while (total--) {
        pto[0] = color;
        pto[1] = color >> 8;
        pto += 2;
    }
    FlushArea(0, __scrHeight, -1);
}

/****************************************************************************/
/*!
  @brief  Draw selected frame buffer to the display
  @param  fbnum - frame buffer number to draw.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::DrawFrameBuffer(uint8_t fbnum) {
    uint8_t* tfrom = SelectFB(fbnum);
    uint8_t* to = rgb_panel->fb;
    memcpy(to, tfrom, __fbSize);
    FlushArea(0, __scrHeight, -1);
}

/****************************************************************************/
/*!
  @brief  Write array of colour data to selected frame buffer to the display
  @param  fbnum - frame buffer number to write to.
  @param  offset - position in destination frame buffer number to write to.
  @param  data - arrary containing data.
  @param  len - length of data array.
  @note   function for direct frame buffer writing without any x or y position
          Care to be taken to not exceed the current frame buffer size.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WriteToFrameBuffer(uint32_t offset, uint8_t* data, uint32_t len) {
    uint8_t* to = SelectFB(frame_buffer);
    uint8_t* pto;
    uint32_t pc = 0;
    uint8_t colM, colL;
    pto = to + offset;
    if (!alpha) {
        memcpy(pto, data, len);
    }
    else {
        while (len -= 2) {
            colM = data[pc++];
            colL = data[pc++];
            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
            pto[0] = __colour;
            pto[1] = __colour >> 8;
            pto += 2;
        }
    }
}

/****************************************************************************/
/*!
  @brief  Write array of colour data to selected frame buffer to the display
  @param  fbnum - frame buffer number to write to.
  @param  offset - position in destination frame buffer number to write to.
  @param  data - arrary containing 16 bit data.
  @param  len - length of data array.
  @note   function for direct frame buffer writing without any x or y position
          Care to be taken to not exceed the current frame buffer size.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WriteToFrameBuffer(uint32_t offset, uint16_t* data, uint32_t len) {
    uint8_t* to = SelectFB(frame_buffer);
    uint8_t* pto;
    uint32_t pc = 0;
    uint16_t tcol;
    uint8_t colM, colL;
    pto = to + offset;
    if (!alpha) {
        memcpy(pto, data, len << 1);
    }
    else {
        while (len--) {
            tcol = data[pc++];
            colM = tcol >> 8;
            colL = tcol & 0xff;
            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
            pto[0] = __colour;
            pto[1] = __colour >> 8;
            pto += 2;
        }
    }
}

/****************************************************************************/
/*!
  @brief  Draw frame buffer area relative to GCI widget from selected to buffer
  to target buffer
  @param  fbnum - frame buffer number to draw from.
              ui - GCI widget index
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::DrawFrameBufferArea(uint8_t fbnum, int16_t ui) {
    DrawFrameBufferArea(fbnum, tuix[ui], tuiy[ui], tuix[ui] + tuiw[ui] - 1,
        tuiy[ui] + tuih[ui] - 1);
}

/****************************************************************************/
/*!
  @brief  Draw frame buffer area using screen co-ordinates  from selected to
  buffer to target buffer
  @param  fbnum - frame buffer number to draw from.
              x1 - left co-ordinate
              y1 - top co-ordinate
              x2 - right co-ordinate
                  y2 - bottom co-ordinate
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::DrawFrameBufferArea(uint8_t fbnum, int16_t x1,
    int16_t y1, int16_t x2,
    int16_t y2) {
    uint8_t* tfrom = SelectFB(fbnum);
    uint8_t* to = SelectFB(frame_buffer);
    int32_t x_start = 0;
    int32_t y_start = 0;
    int32_t x_end = 0;
    int32_t y_end = 0;
    uint8_t colM, colL;
    Clipping(false);
    // set area depending on orientation
    switch (rotation) {
    case LANDSCAPE:
        x_start = x1;
        y_start = y1;
        x_end = x2;
        y_end = y2;
        break;
    case LANDSCAPE_R:
        x_start = rgb_panel->timings.h_res - 1 - x2;
        x_end = rgb_panel->timings.h_res - 1 - x1;
        y_start = rgb_panel->timings.v_res - 2 - y2;
        y_end = rgb_panel->timings.v_res - 2 - y1;
        break;
    case PORTRAIT_R:
        x_start = rgb_panel->timings.h_res - 1 - y2;
        x_end = rgb_panel->timings.h_res - 1 - y1;
        y_start = x1;
        y_end = x2;
        break;
    case PORTRAIT:
        x_start = y1;
        x_end = y2;
        y_start = rgb_panel->timings.v_res - 2 - x2;
        y_end = rgb_panel->timings.v_res - 2 - x1;
    }
    if (x_start >= rgb_panel->timings.h_res || x_end < 0 || y_start >= (rgb_panel->timings.v_res - 2) || y_end < 0)
        return;
    if (x_start < 0)
        x_start = 0;
    if (y_start < 0)
        y_start = 0;
    if (x_end >= rgb_panel->timings.h_res)
        x_end = rgb_panel->timings.h_res - 1;
    if (y_end >= rgb_panel->timings.v_res - 1)
        y_end = rgb_panel->timings.v_res - 2;
    uint32_t s_width = x_end - x_start + 1;
    uint32_t s_height = y_end - y_start + 1;
    uint32_t pc = (y_start * __scrWidth) + (x_start << 1);
    to += pc;
    tfrom += pc;
    int twidth;
    while (s_height--) {
        if (!transalpha) {
            memcpy(to, tfrom, s_width << 1);
            to += __scrWidth;
            tfrom += __scrWidth;
        }
        else {
            twidth = s_width;
            while (twidth--) {
                colM = tfrom[1];
                colL = tfrom[0];
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), to[0] | (to[1] << 8), __alpha);
                        to[0] = __colour;
                        to[1] = __colour >> 8;
                    }
                    else {
                        to[1] = colM;
                        to[0] = colL;
                    }
                }
                to += 2;
                tfrom += 2;
            }
            to += (__scrWidth - (s_width << 1));
            tfrom += (__scrWidth - (s_width << 1));
        }
    }
    if (frame_buffer == 0)
        FlushArea(y_start, y_end, -1);
}

/****************************************************************************/
/*!
  @brief  Write buffer of pixels to selected frame buffer
  @param  *color - 8 bit colour buffer
  @param  length of buffer in pixels
  @note   Write to previously set GRAM window from byte array.
          Clipping handles out of bounds also which does have a speed penalty.
          Used for writing from uSD images in byte chunks if needed.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WrGRAMs(uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return; // return if GRAM window not set.
    uint32_t pc = 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t colM, colL;
    if (rotation == LANDSCAPE) {
        pto = tpto + (GRAMypos * __scrWidth) +
            (GRAMxpos << 1); // set frame buffer pointer to current x, y position
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) { // check if inside clip region
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto += 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos * __scrWidth) +
                    (GRAMxpos << 1); // set new frame buffer pointer
            }
            pixelPos++;
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto -= 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                    (__scrWidth - ((GRAMxpos + 1) << 1));
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto += __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto =
                    tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto -= __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos << 1) +
                    ((__scrHeight - GRAMxpos - 1) * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (pixelPos >= (pixelCount - 2) &&
        frame_buffer == 0) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        // CPU writes data to PSRAM through DCache, data in PSRAM might not get
        // updated, so write back
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Write flash array buffer of pixels to selected frame buffer
  @param  *color - 8 bit colour buffer
  @param  length of buffer in pixels
  @note   Write to previously set GRAM window from byte array.
          Clipping handles out of bounds also which does have a speed penalty.
          Used for writing from uSD images in byte chunks if needed.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WrGRAMs(const uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return; // return if GRAM window not set.
    uint32_t pc = 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t colM, colL;

    if (rotation == LANDSCAPE) {
        pto = tpto + (GRAMypos * __scrWidth) +
            (GRAMxpos << 1); // set frame buffer pointer to current x, y position
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) { // check if inside clip region
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto += 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos * __scrWidth) +
                    (GRAMxpos << 1); // set new frame buffer pointer
            }
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto -= 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                    (__scrWidth - ((GRAMxpos + 1) << 1));
            }
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto += __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto =
                    tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
            }
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        while (len--) {
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = color_data[pc++];
                colL = color_data[pc++];
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            else {
                pc += 2;
            }
            GRAMxpos++;
            pto -= __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos << 1) +
                    ((__scrHeight - GRAMxpos - 1) * __scrWidth);
            }
        }
    }
    if (GRAMypos > GRAMy2 && frame_buffer == 0) {
        // CPU writes data to PSRAM through DCache, data in PSRAM might not get
        // updated, so write back
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Flush frame buffer area recently written to
  @param  y1 start position in pixels
  @param  y2 end position in pixels
  @param  xpos x position in line
  @note   flush area will immediately flush defined area if StartWrite has not
          been initiated. If StartWrite has been initiated then the area will
          be updated only. If the area to be flushed is just a pixel then an
          area of just 2 bytes will be flushed
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::FlushArea(int y1, int y2, int xpos) {
    if (y1 < 0)
        y1 = 0;
    if (y2 > (__scrHeight - 1))
        y2 = __scrHeight - 1;
    if (writeFBonly) { // just update flush area
        if (y1 < low_Y)
            low_Y = y1;
        if (y2 > high_Y)
            high_Y = y2;
        flush_pending = true;
        return;
    }
    if (y1 > y2)
        swap(y1, y2);
    if (y1 > __scrHeight) {
        flush_pending = false;
        return;
    }
    if (xpos != -1 &&
        !flush_pending) { // if pixel and not in previous StartWrite condition
        Cache_WriteBack_Addr(
            (uint32_t)(rgb_panel->fb + (y1 * __scrWidth) + (xpos << 1)),
            2); // flush pixel
    }
    else {
        if (rgb_panel->flags.fb_in_psram) {
            uint32_t bytes_to_flush = (uint32_t)(y2 - y1 + 2) * __scrWidth;
            Cache_WriteBack_Addr((uint32_t)(rgb_panel->fb + (y1 * __scrWidth)),
                bytes_to_flush); // flush area
        }
    }
    if (!rgb_panel->flags.stream_mode) {
        __start_transmission();
    }
    esp_rom_delay_us(1);
    flush_pending = false;
}

/****************************************************************************/
/*!
  @brief  Flush frame buffer area recently written to
      @param  x1 left start position in pixels
      @param  x2 right end position in pixels
  @param  y1 top start position in pixels
  @param  y2 bottom position in pixels
  @param  xpos x position in line
  @note   flush area will immediately flush defined area if StartWrite has not
          been initiated. If StartWrite has been initiated then the area will
          be updated only. If the area to be flushed is just a pixel then an
          area of just 2 bytes will be flushed
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::FlushArea(int x1, int x2, int y1, int y2, int xpos) {
    if (y1 < 0)
        y1 = 0;
    if (y2 > (__scrHeight - 1))
        y2 = __scrHeight - 1;
    if (x1 < 0)
        x1 = 0;
    if (x2 > ((__scrWidth >> 1) - 1))
        x2 = (__scrWidth >> 1) - 1;
    if (writeFBonly) { // just update flush area
        if (y1 < low_Y)
            low_Y = y1;
        if (y2 > high_Y)
            high_Y = y2;
        if (x1 < low_X)
            low_X = x1;
        if (x2 > high_X)
            high_X = x2;
        flush_pending = true;
        return;
    }
    if (y1 > y2)
        swap(y1, y2);
    if (y1 > (__height - 1) || x1 > (__width - 1)) {
        flush_pending = false;
        return;
    }
    int h = y2 - y1 + 1;
    int w = x2 - x1 + 1;
    uint32_t pc = (y1 * __scrWidth) + (x1 << 1);
    uint8_t* pto = rgb_panel->fb;
    pto += pc;
    while (h--) {
        if (rgb_panel->flags.fb_in_psram) {
            uint32_t bytes_to_flush = w << 1;
            Cache_WriteBack_Addr((uint32_t)(pto), bytes_to_flush); // flush area
        }
        pto += __scrWidth;
    }
    if (!rgb_panel->flags.stream_mode) {
        __start_transmission();
    }
    esp_rom_delay_us(1);
    flush_pending = false;
}

/****************************************************************************/
/*!
  @brief  Write buffer of pixels to selected frame buffer
  @param  *color_data - 16 bit colour buffer
  @param  length of buffer in pixels
  @note   Write to previously set GRAM window from byte array.
          Clipping handles out of bounds also which does have a speed penalty.
          Used for writing from RAM locations in 16bit chunks if needed.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WrGRAMs(uint16_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint16_t tcol;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t colM, colL;

    if (rotation == LANDSCAPE) {
        pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
        while (len--) {
            tcol = color_data[pc++];
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = tcol >> 8;
                colL = tcol & 0xff;
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            GRAMxpos++;
            pto += 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
            }
            pixelPos++;
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        while (len--) {
            tcol = color_data[pc++];
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = tcol >> 8;
                colL = tcol & 0xff;
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            GRAMxpos++;
            pto -= 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                    (__scrWidth - ((GRAMxpos + 1) << 1));
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        while (len--) {
            tcol = color_data[pc++];
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = tcol >> 8;
                colL = tcol & 0xff;
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            GRAMxpos++;
            pto += __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto =
                    tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        while (len--) {
            tcol = color_data[pc++];
            if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                GRAMypos <= clipY2) {
                colM = tcol >> 8;
                colL = tcol & 0xff;
                if (!transalpha) {
                    pto[1] = colM;
                    pto[0] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                            pto[0] = __colour;
                            pto[1] = __colour >> 8;
                        }
                        else {
                            pto[1] = colM;
                            pto[0] = colL;
                        }
                    }
                }
            }
            GRAMxpos++;
            pto -= __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos << 1) +
                    ((__scrHeight - GRAMxpos - 1) * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (pixelPos >= pixelCount &&
        frame_buffer == 0) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Write buffer of pixels to selected frame buffer (compatible IoD
  Function)
  @param  *color_data - 32 bit colour buffer (2 x 16)
  @param  length of buffer in pixels
  @note   Write to previously set GRAM window from byte array.
          Clipping handles out of bounds also which does have a speed penalty.
          Used for writing from RAM locations in 16bit chunks if needed.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WrGRAMs(uint32_t* color_data, uint16_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint32_t tcol;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t div;
    uint8_t colM, colL;
    int innerloop;

    if (rotation == LANDSCAPE) {
        pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
        while (len--) {
            innerloop = 2;
            div = 32;
            tcol = color_data[pc++];
            while (innerloop--) {
                if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                    GRAMypos <= clipY2) {
                    colM = tcol >> (div -= 8);
                    colL = tcol >> (div -= 8);
                    if (!transalpha) {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                    else {
                        if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                            if (alpha) {
                                calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                                pto[0] = __colour;
                                pto[1] = __colour >> 8;
                            }
                            else {
                                pto[1] = colM;
                                pto[0] = colL;
                            }
                        }
                    }
                }
                GRAMxpos++;
                pto += 2;
                if (GRAMxpos > GRAMx2) {
                    GRAMxpos = GRAMx1;
                    GRAMypos++;
                    pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
                }
                pixelPos++;
            }
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        while (len--) {
            innerloop = 2;
            div = 32;
            tcol = color_data[pc++];
            while (innerloop--) {
                if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                    GRAMypos <= clipY2) {
                    colM = tcol >> (div -= 8);
                    colL = tcol >> (div -= 8);
                    if (!transalpha) {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                    else {
                        if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                            if (alpha) {
                                calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                                pto[0] = __colour;
                                pto[1] = __colour >> 8;
                            }
                            else {
                                pto[1] = colM;
                                pto[0] = colL;
                            }
                        }
                    }
                }
                GRAMxpos++;
                pto -= 2;
                if (GRAMxpos > GRAMx2) {
                    GRAMxpos = GRAMx1;
                    GRAMypos++;
                    pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                        (__scrWidth - ((GRAMxpos + 1) << 1));
                }
                pixelPos++;
            }
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        while (len--) {
            innerloop = 2;
            div = 32;
            tcol = color_data[pc++];
            while (innerloop--) {
                if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                    GRAMypos <= clipY2) {
                    colM = tcol >> (div -= 8);
                    colL = tcol >> (div -= 8);
                    if (!transalpha) {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                    else {
                        if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                            if (alpha) {
                                calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                                pto[0] = __colour;
                                pto[1] = __colour >> 8;
                            }
                            else {
                                pto[1] = colM;
                                pto[0] = colL;
                            }
                        }
                    }
                }
                GRAMxpos++;
                pto += __scrWidth;
                if (GRAMxpos > GRAMx2) {
                    GRAMxpos = GRAMx1;
                    GRAMypos++;
                    pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) +
                        (GRAMxpos * __scrWidth);
                }
                pixelPos++;
            }
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        while (len--) {
            innerloop = 2;
            div = 32;
            tcol = color_data[pc++];
            while (innerloop--) {
                if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
                    GRAMypos <= clipY2) {
                    colM = tcol >> (div -= 8);
                    colL = tcol >> (div -= 8);
                    if (!transalpha) {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                    else {
                        if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                            if (alpha) {
                                calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                                pto[0] = __colour;
                                pto[1] = __colour >> 8;
                            }
                            else {
                                pto[1] = colM;
                                pto[0] = colL;
                            }
                        }
                    }
                }
                GRAMxpos++;
                pto -= __scrWidth;
                if (GRAMxpos > GRAMx2) {
                    GRAMxpos = GRAMx1;
                    GRAMypos++;
                    pto = tpto + (GRAMypos << 1) +
                        ((__scrHeight - GRAMxpos - 1) * __scrWidth);
                }
                pixelPos++;
            }
        }
    }
    if (pixelPos >= pixelCount &&
        frame_buffer == 0) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Write buffer of pixels to selected frame buffer
  @param  *color_data - 16 bit colour buffer
  @param  length of buffer in pixels
  @note   Write to previously set GRAM window from byte array.
          Fastest way to write GRAM. Not to be used for GCI or gfx4desp32
  functions. Used for LVGL sketches as no clipping or out of bounds supported.
          LANDSCAPE mode uses memcpy for frame buffer write.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::pushColors(uint16_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint16_t tcol;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    if (rotation == LANDSCAPE) {
        uint16_t* pfrom = color_data; // set pointer from 16bit array
        int16_t pcheight = GRAMy2 - GRAMy1 + 1;
        int16_t pcwidth = GRAMx2 - GRAMx1 + 1;
        pto = tpto + (GRAMypos * __scrWidth) +
            (GRAMxpos << 1); // set pointer to frame buffer
        while (pcheight--) {
            memcpy(pto, pfrom, pcwidth << 1); // copy 'from' to 'to'
            GRAMypos++;
            pto = tpto + (GRAMypos * __scrWidth) +
                (GRAMxpos << 1); // set new frame buffer pointer
            pfrom += pcwidth;
            pixelPos += (pcwidth);
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        while (len--) {
            tcol = color_data[pc++];
            pto[0] = tcol & 0xff;
            pto[1] = tcol >> 8;
            GRAMxpos++;
            pto -= 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                    (__scrWidth - ((GRAMxpos + 1) << 1));
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        while (len--) {
            tcol = color_data[pc++];
            pto[0] = tcol & 0xff;
            pto[1] = tcol >> 8;
            GRAMxpos++;
            pto += __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto =
                    tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        while (len--) {
            tcol = color_data[pc++];
            pto[0] = tcol & 0xff;
            pto[1] = tcol >> 8;
            GRAMxpos++;
            pto -= __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos << 1) +
                    ((__scrHeight - GRAMxpos - 1) * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (pixelPos >= pixelCount &&
        frame_buffer == 0) { // if GRAM area fully written to, flush area
        wrGRAM = false;
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Write buffer of pixels to selected frame buffer
  @param  *color_data - 8 bit colour buffer
  @param  length of buffer in pixels
  @note   Write to previously set GRAM window from byte array.
          Fastest way to write GRAM. Not to be used for GCI or gfx4desp32
  functions. Used for LVGL sketches as no clipping or out of bounds supported.
          LANDSCAPE mode uses memcpy for frame buffer write.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::pushColors(uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    if (rotation == LANDSCAPE) {
        pto = tpto + (GRAMypos * __scrWidth) +
            (GRAMxpos << 1); // set pointer to frame buffer
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto += 2;
            if (GRAMxpos > GRAMx2) {

                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos * __scrWidth) +
                    (GRAMxpos << 1); // set new frame buffer pointer
            }
            pixelPos++;
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto -= 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                    (__scrWidth - ((GRAMxpos + 1) << 1));
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto += __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto =
                    tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto -= __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos << 1) +
                    ((__scrHeight - GRAMxpos - 1) * __scrWidth);
            }
            pixelPos++;
        }
    }
    if (pixelPos >= pixelCount &&
        frame_buffer == 0) { // if GRAM area fully written to, flush area
        wrGRAM = false;
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

void gfx4desp32_rgb_panel::pushColors(const uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    if (rotation == LANDSCAPE) {
        pto = tpto + (GRAMypos * __scrWidth) +
            (GRAMxpos << 1); // set pointer to frame buffer
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto += 2;
            if (GRAMxpos > GRAMx2) {

                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos * __scrWidth) +
                    (GRAMxpos << 1); // set new frame buffer pointer
            }
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto -= 2;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                    (__scrWidth - ((GRAMxpos + 1) << 1));
            }
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto += __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto =
                    tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
            }
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        while (len--) {
            pto[1] = color_data[pc++];
            pto[0] = color_data[pc++];
            GRAMxpos++;
            pto -= __scrWidth;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
                pto = tpto + (GRAMypos << 1) +
                    ((__scrHeight - GRAMxpos - 1) * __scrWidth);
            }
        }
    }
    if (GRAMypos > GRAMy2 && frame_buffer == 0) {
        wrGRAM = false;
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Write single pixel to selected frame buffer
  @param  color - 16 bit colour RGB565
  @note   Write to previously set GRAM window with RGB565 colour.
          Clipping handles out of bounds also which does have a speed penalty.
          Used for writing from RAM locations in single pixels.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WrGRAM(uint16_t color) {
    if (!(wrGRAM))
        return;
    if (transparency) {
        if (color == _transparentColor) {
            GRAMxpos++;
            if (GRAMxpos > GRAMx2) {
                GRAMxpos = GRAMx1;
                GRAMypos++;
            }
            pixelPos++;
            return;
        }
    }

    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    if (rotation == LANDSCAPE) {
        pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
        if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
            GRAMypos <= clipY2) {
            pto[0] = color & 0xff;
            pto[1] = color >> 8;
        }
        GRAMxpos++;
        pto += 2;
        if (GRAMxpos > GRAMx2) {
            GRAMxpos = GRAMx1;
            GRAMypos++;
            pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
        }
        pixelPos++;
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
            (__scrWidth - ((GRAMxpos + 1) << 1));
        if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
            GRAMypos <= clipY2) {
            pto[0] = color & 0xff;
            pto[1] = color >> 8;
        }
        GRAMxpos++;
        pto -= 2;
        if (GRAMxpos > GRAMx2) {
            GRAMxpos = GRAMx1;
            GRAMypos++;
            pto = tpto + ((__scrHeight - GRAMypos - 1) * __scrWidth) +
                (__scrWidth - ((GRAMxpos + 1) << 1));
        }
        pixelPos++;
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
            GRAMypos <= clipY2) {
            pto[0] = color & 0xff;
            pto[1] = color >> 8;
        }
        GRAMxpos++;
        pto += __scrWidth;
        if (GRAMxpos > GRAMx2) {
            GRAMxpos = GRAMx1;
            GRAMypos++;
            pto = tpto + (__scrWidth - (GRAMypos << 1) - 2) + (GRAMxpos * __scrWidth);
        }
        pixelPos++;
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
            GRAMypos <= clipY2) {
            pto[0] = color & 0xff;
            pto[1] = color >> 8;
        }
        GRAMxpos++;
        pto -= __scrWidth;
        if (GRAMxpos > GRAMx2) {
            GRAMxpos = GRAMx1;
            GRAMypos++;
            pto =
                tpto + (GRAMypos << 1) + ((__scrHeight - GRAMxpos - 1) * __scrWidth);
        }
        pixelPos++;
    }
    if (pixelPos >= pixelCount &&
        frame_buffer == 0) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        if (rotation == LANDSCAPE)
            FlushArea(GRAMy1, GRAMy2, -1);
        if (rotation == LANDSCAPE_R)
            FlushArea(__scrHeight - GRAMy2 - 1, __scrHeight - GRAMy1 - 1, -1);
        if (rotation == PORTRAIT_R)
            FlushArea(GRAMx1, GRAMx2, -1);
        if (rotation == PORTRAIT)
            FlushArea(__scrHeight - GRAMx2 - 1, __scrHeight - GRAMx1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Read a single pixel.
  @param  x left X position in pixels
  @param  y top Y position in pixels
  @note   returns RGB565 colour
*/
/****************************************************************************/
uint16_t gfx4desp32_rgb_panel::ReadPixel(uint16_t xrp, uint16_t yrp) {
    if (yrp > clipY2 || yrp < clipY1)
        return 0;
    if (xrp > clipX2 || yrp < clipX1)
        return 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto = NULL;
    if (rotation == LANDSCAPE)
        pto = tpto + (yrp * __scrWidth) + (xrp << 1);
    if (rotation == LANDSCAPE_R)
        pto = tpto + ((__scrHeight - yrp - 1) * __scrWidth) +
        (__scrWidth - ((xrp + 1) << 1));
    if (rotation == PORTRAIT_R) {
        int32_t temp = (__scrWidth - ((yrp + 1) << 1)) + (__scrWidth * xrp);
        if (temp < 0 || temp > __fbSize)
            return 0;
        pto = tpto + temp;
    }
    if (rotation == PORTRAIT)
        pto = tpto + (yrp << 1) + ((__scrHeight - xrp - 1) * __scrWidth);
    return pto[0] + (pto[1] << 8);
}

uint16_t gfx4desp32_rgb_panel::ReadPixelFromFrameBuffer(uint16_t xrp, uint16_t yrp, uint8_t fB) {
    if (yrp > clipY2 || yrp < clipY1)
        return 0;
    if (xrp > clipX2 || yrp < clipX1)
        return 0;
    uint8_t* tpto = SelectFB(fB);
    uint8_t* pto;
    if (rotation == LANDSCAPE)
        pto = tpto + (yrp * __scrWidth) + (xrp << 1);
    if (rotation == LANDSCAPE_R)
        pto = tpto + ((__scrHeight - yrp - 1) * __scrWidth) +
        (__scrWidth - ((xrp + 1) << 1));
    if (rotation == PORTRAIT_R) {
        int32_t temp = (__scrWidth - ((yrp + 1) << 1)) + (__scrWidth * xrp);
        if (temp < 0 || temp > __fbSize)
            return 0;
        pto = tpto + temp;
    }
    if (rotation == PORTRAIT)
        pto = tpto + (yrp << 1) + ((__scrHeight - xrp - 1) * __scrWidth);
    return pto[0] + (pto[1] << 8);
}

/****************************************************************************/
/*!
  @brief  Read a line of pixels.
  @param  x left X position in pixels
  @param  y top Y position in pixels
  @param  w width of line
  @param  data - 16 bit user array
  @note   returns len
*/
/****************************************************************************/
uint16_t gfx4desp32_rgb_panel::ReadLine(int16_t x, int16_t y, int16_t w,
    uint16_t* data) {
    if (y > __height - 1 || y < 0)
        return 0;
    if (x > __width - 1 || (x + w - 1) < 0)
        return 0;
    if (x < 0) {
        w -= 0 - x;
        x = 0;
    }
    if ((x + w) > __width)
        w = __width - x;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    int pc = 0;
    int readw = w;
    if (rotation == LANDSCAPE) {
        pto = tpto + (y * __scrWidth) + (x << 1);
        while (w--) {
            data[pc++] = pto[0] + (pto[1] << 8);
            pto += 2;
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - y - 1) * __scrWidth) +
            (__scrWidth - ((x + 1) << 1));
        while (w--) {
            data[pc++] = pto[0] + (pto[1] << 8);
            pto -= 2;
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - ((y + 1) << 1)) + (x * __scrWidth);
        while (w--) {
            data[pc++] = pto[0] + (pto[1] << 8);
            pto += __scrWidth;
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (y << 1) + ((__scrHeight - x - 1) * __scrWidth);
        while (w--) {
            data[pc++] = pto[0] + (pto[1] << 8);
            pto -= __scrWidth;
        }
    }
    return readw;
}

/****************************************************************************/
/*!
  @brief  Copy a line of pixels from selected frame buffer to current frame buffer.
  @param  x left X position in pixels
  @param  y top Y position in pixels
  @param  w width of line
  @param  fb source frame buffer number
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::CopyFrameBufferLine(int16_t x, int16_t y, int16_t w,
    int Fb) {
    if (y > clipY2 || y < clipY1)
        return;
    if (x > clipX2 || (x + w - 1) < clipX1)
        return;
    if (w < 0) {
        x += w;
        w = abs(w);
    }
    if (x < clipX1) {
        w -= clipX1 - x;
        x = clipX1;
    }
    if ((x + w - 1) >= clipX2)
        w = clipX2 - x;
    uint8_t cb = GetFrameBuffer();
    DrawToframebuffer(Fb);
    ReadLine(x, y, w, lbuff);
    DrawToframebuffer(cb);
    WriteLine(x, y, w, lbuff);
}


/****************************************************************************/
/*!
  @brief  Write a line of pixels.
  @param  x left X position in pixels
  @param  y top Y position in pixels
  @param  w width of line
  @param  data - 16 bit user array
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::WriteLine(int16_t x, int16_t y, int16_t w,
    uint16_t* data) {
    if (y > clipY2 || y < clipY1)
        return;
    if (x > clipX2 || (x + w - 1) < clipX1)
        return;
    if (x < clipX1) {
        w -= clipX1 - x;
        x = clipX1;
    }
    if ((x + w - 1) >= clipX2)
        w = clipX2 - x;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t colM, colL;
    uint16_t tcol;
    int pc = 0;
    int flushw = w;
    if (rotation == LANDSCAPE) {
        pto = tpto + (y * __scrWidth) + (x << 1);
        while (w--) {
            tcol = data[pc++];
            colM = tcol >> 8;
            colL = tcol & 0xff;
            if (!transalpha) {
                pto[1] = colM;
                pto[0] = colL;
            }
            else {
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                        pto[0] = __colour;
                        pto[1] = __colour >> 8;
                    }
                    else {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                }
            }

            pto += 2;
        }
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - y - 1) * __scrWidth) +
            (__scrWidth - ((x + 1) << 1));
        while (w--) {
            tcol = data[pc++];
            colM = tcol >> 8;
            colL = tcol & 0xff;
            if (!transalpha) {
                pto[1] = colM;
                pto[0] = colL;
            }
            else {
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                        pto[0] = __colour;
                        pto[1] = __colour >> 8;
                    }
                    else {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                }
            }
            pto -= 2;
        }
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - ((y + 1) << 1)) + (x * __scrWidth);
        while (w--) {
            tcol = data[pc++];
            colM = tcol >> 8;
            colL = tcol & 0xff;
            if (!transalpha) {
                pto[1] = colM;
                pto[0] = colL;
            }
            else {
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                        pto[0] = __colour;
                        pto[1] = __colour >> 8;
                    }
                    else {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                }
            }
            pto += __scrWidth;
        }
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (y << 1) + ((__scrHeight - x - 1) * __scrWidth);
        while (w--) {
            tcol = data[pc++];
            colM = tcol >> 8;
            colL = tcol & 0xff;
            if (!transalpha) {
                pto[1] = colM;
                pto[0] = colL;
            }
            else {
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), pto[0] | (pto[1] << 8), __alpha);
                        pto[0] = __colour;
                        pto[1] = __colour >> 8;
                    }
                    else {
                        pto[1] = colM;
                        pto[0] = colL;
                    }
                }
            }
            pto -= __scrWidth;
        }
    }
}

/****************************************************************************/
/*!
  @brief  Set clipping window ready for wrGRAM/s functions.
  @param  x1 left X position in pixels
  @param  y1 top Y position in pixels
  @param  x2 right X position in pixels
  @param  y2 bottom Y position in pixels
  @note   sets global variables clipX1pos, clipY1pos, clipX2pos, clipY2pos to
  window pos
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::ClipWindow(int x1, int y1, int x2, int y2) {
    clipX1pos = x1;
    clipY1pos = y1;
    clipX2pos = x2;
    clipY2pos = y2; // need to add check for out of bounds
    if (clipX1pos > __width - 1) clipX1pos = __width - 1;
    if (clipX1pos < 0) clipX1pos = 0;
    if (clipX2pos > __width - 1) clipX2pos = __width - 1;
    if (clipX2pos < 0) clipX2pos = 0;
    if (clipY1pos > __height - 1) clipY1pos = __height - 1;
    if (clipY1pos < 0) clipY1pos = 0;
    if (clipY2pos > __height - 1) clipY2pos = __height - 1;
    if (clipY2pos < 0) clipY2pos = 0;
}

/****************************************************************************/
/*!
  @brief  Enable / disable Clipping region
  @param  clipping - true / false
  @note   clipping is enabled by changing from defalult to user position and
          disabled by reverting back to default.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::Clipping(bool clipping) {
    if (clipping) {
        clipX1 = clipX1pos;
        clipY1 = clipY1pos;
        clipX2 = clipX2pos;
        clipY2 = clipY2pos;
    }
    else {
        if (rotation < 2) { // set to screen dimensions for disabled
            clipX1 = 0;
            clipY1 = 0;
            clipX2 = (int)rgb_panel->timings.h_res - 1;
            clipY2 = (int)rgb_panel->timings.v_res - 2;
        }
        else {
            clipX1 = 0;
            clipY1 = 0;
            clipX2 = (int)rgb_panel->timings.v_res - 1;
            clipY2 = (int)rgb_panel->timings.h_res - 2;
        }
    }
    clippingON = clipping;
    clipx1 = clipX1;
    clipy1 = clipY1;
    clipx2 = clipX2;
    clipy2 = clipY2;
}

/****************************************************************************/
/*!
  @brief  Set scroll window ready for scrolling.
  @param  x1 left X position in pixels
  @param  y1 top Y position in pixels
  @param  x2 right X position in pixels
  @param  y2 bottom Y position in pixels
  @note   sets global variables scroll_X1, scroll_Y1, scroll_X2, scroll_Y2 to
  window pos
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::setScrollArea(int x1, int y1, int x2, int y2) {
    scroll_X1 = x1;
    scroll_Y1 = y1;
    scroll_X2 = x2;
    scroll_Y2 = y2;
    if (scroll_X1 < 0)
        scroll_X1 = 0;
    if (scroll_X2 > __width - 1)
        scroll_X2 = __width - 1;
    if (scroll_Y1 < 0)
        scroll_Y1 = 0;
    if (scroll_Y2 > __height - 1)
        scroll_Y2 = __height - 1;
}

/****************************************************************************/
/*!
  @brief  Set scroll window ready for scrolling.
  @param  y1 top Y position in pixels
  @param  y2 bottom Y position in pixels
  @note   sets global variables scroll_X1, scroll_Y1, scroll_X2, scroll_Y2 to
  window pos compatible GFX4dESP32 scroll area for SPI displays and backward
  compatibilty with RGB displays
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::setScrollArea(int y1, int y2) {
    scroll_X1 = 0;
    scroll_Y1 = y1;
    scroll_X2 = __width - 1;
    scroll_Y2 = y2;
    if (scroll_X1 < 0)
        scroll_X1 = 0;
    if (scroll_X2 > __width - 1)
        scroll_X2 = __width - 1;
    if (scroll_Y1 < 0)
        scroll_Y1 = 0;
    if (scroll_Y2 > __height - 1)
        scroll_Y2 = __height - 1;
}

/****************************************************************************/
/*!
  @brief  Switch on / off automatic scroll
  @param  scrEn - true / false
  @note   enable automatic scrolling in conjunction with gfx4desp32 write and
  newline functions
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::_ScrollEnable(bool scrEn) { scroll_Enable = scrEn; }

/****************************************************************************/
/*!
  @brief  Set direction of scroll
  @param  scrEn - scroll_Direction 0 - 3
  @note   default direction is upwards
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::setScrollDirection(uint8_t scrDir) {
    scroll_Direction = scrDir % 4;
}

/****************************************************************************/
/*!
  @brief  Set blanking line colour after scroll has moved
  @param  scolor - RGB565 colour
  @note   this maybe could be the current text background colour
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::setScrollBlankingColor(int32_t scolor) {
    scroll_blanking = scolor;
}

/****************************************************************************/
/*!
  @brief  Set scroll behaviour
  @param  sspeed - enable and delay
  @note   0 - will scroll height defined by character height in one step.
          a value higher than 0 will scroll pixel line by pixel line delayed
          by the value in ms
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::SmoothScrollSpeed(uint8_t sspeed) {
    scroll_speed = sspeed;
}

/****************************************************************************/
/*!
  @brief  Perform scroll
  @param  steps - number of pixel lines to scroll
  @note   Scroll is carried out by moving the contents of the framebuffer to
          a new location defined by steps.
          if smoothScrollSpeed has been set then it will be scrolled pixel line
          by pixel line delayed by the ms value in smoothScrollSpeed.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::Scroll(int steps) {
    uint32_t x_start = 0;
    uint32_t y_start = 0;
    uint32_t x_end = 0;
    uint32_t y_end = 0;
    uint8_t scrdir = 0;
    Clipping(false);
    // set area depending on orientation
    if (rotation == LANDSCAPE) {
        x_start = scroll_X1;
        y_start = scroll_Y1;
        x_end = scroll_X2;
        y_end = scroll_Y2;
    }
    if (rotation == LANDSCAPE_R) {
        x_start = rgb_panel->timings.h_res - 1 - scroll_X2;
        x_end = rgb_panel->timings.h_res - 1 - scroll_X1;
        y_start = rgb_panel->timings.v_res - 2 - scroll_Y2;
        y_end = rgb_panel->timings.v_res - 2 - scroll_Y1;
    }
    if (rotation == PORTRAIT_R) {
        x_start = rgb_panel->timings.h_res - 1 - scroll_Y2;
        x_end = rgb_panel->timings.h_res - 1 - scroll_Y1;
        y_start = scroll_X1;
        y_end = scroll_X2;
    }
    if (rotation == PORTRAIT) {
        x_start = scroll_Y1;
        x_end = scroll_Y2;
        y_start = rgb_panel->timings.v_res - 2 - scroll_X2;
        y_end = rgb_panel->timings.v_res - 2 - scroll_X1;
    }
    uint32_t s_width = x_end - x_start + 1;
    uint32_t s_height = y_end - y_start + 1;
    scrdir =
        scroll_Directions[(rotation << 2) +
        scroll_Direction]; // calculate direction using
    // orientation and direction array
    uint32_t n, o;
    uint8_t* tfbbuf = SelectFB(frame_buffer);
    uint8_t* from; // create pointer for from area
    uint8_t* to;   // create pointer for to area
    int16_t s_steps = steps;
    uint32_t inc = 0;
    uint32_t s_inc = 0;
    int32_t pc;
    if (scroll_speed == 0 && steps > 1) {
        s_steps = 1;
        inc = steps;
    }
    else {
        s_steps = steps;
        inc = 1;
    }
    if (scrdir == 0) {
        while (s_steps--) {
            from = tfbbuf + ((y_start + inc - 1) * __scrWidth) + (x_start << 1);
            to = tfbbuf + (y_start * __scrWidth) + (x_start << 1);
            n = s_height;
            while (n--) {
                if (n > inc - 1) {
                    memcpy(to, from, s_width << 1);
                    from += __scrWidth;
                    to += __scrWidth;
                }
                else {
                    o = s_width;
                    while (o--) {
                        if (scroll_blanking != -1) {
                            to[0] = scroll_blanking;
                            to[1] = scroll_blanking >> 8;
                        }
                        to += 2;
                    }
                    to += (__scrWidth - (s_width << 1));
                }
            }
            FlushArea(y_start, y_end, -1);
            delay(scroll_speed);
        }
    }
    if (scrdir == 1) {
        int32_t tempfrom;
        while (s_steps--) {
            tempfrom = ((y_start + s_height - 1 - inc) * __scrWidth) + (x_start << 1);
            from = tfbbuf + tempfrom;
            to = tfbbuf + ((y_start + s_height - 1) * __scrWidth) + (x_start << 1);
            n = s_height;
            while (n--) {
                if (n > inc - 1) {
                    if (tempfrom >= 0)
                        memmove(to, from, s_width << 1);
                    from -= __scrWidth;
                    tempfrom -= __scrWidth;
                    to -= __scrWidth;
                }
                else {
                    o = s_width;
                    while (o--) {
                        if (scroll_blanking != -1) {
                            to[0] = scroll_blanking;
                            to[1] = scroll_blanking >> 8;
                        }
                        to += 2;
                    }
                    to -= (__scrWidth + (s_width << 1));
                }
            }
            FlushArea(y_start, y_end, -1);
            delay(scroll_speed);
        }
    }
    if (scrdir == 2) {
        while (s_steps--) {
            from = tfbbuf + (y_start * __scrWidth) + (x_start << 1);
            to = tfbbuf + (y_start * __scrWidth) + ((x_start + inc) << 1);
            n = s_height;
            while (n--) {
                memmove(to, from, (s_width - inc) << 1);
                pc = 0;
                s_inc = inc;
                while (s_inc--) {
                    if (scroll_blanking != -1) {
                        from[pc++] = scroll_blanking;
                        from[pc++] = scroll_blanking >> 8;
                    }
                }
                from += __scrWidth;
                to += __scrWidth;
            }
            FlushArea(y_start, y_end, -1);
            delay(scroll_speed);
        }
    }
    if (scrdir == 3) {
        while (s_steps--) {
            from = tfbbuf + (y_start * __scrWidth) + ((x_start + inc) << 1);
            to = tfbbuf + (y_start * __scrWidth) + (x_start << 1);
            n = s_height;
            while (n--) {
                memmove(to, from, (s_width - inc) << 1);
                pc = ((s_width - 0 - inc) << 1);
                s_inc = inc;
                while (s_inc--) {
                    if (scroll_blanking != -1) {
                        to[pc++] = scroll_blanking;
                        to[pc++] = scroll_blanking >> 8;
                    }
                }
                from += __scrWidth;
                to += __scrWidth;
            }
            FlushArea(y_start, y_end, -1);
            delay(scroll_speed);
        }
    }
    scrolled = true;
    Clipping(true);
}

/****************************************************************************/
/*!
  @brief  Draw simple Filled Rectangle.
  @param  x1 left X position in pixels
  @param  y1 top Y position in pixels
  @param  x2 right X position in pixels
  @param  y2 bottom Y position in pixels
  @param  color - RGB565 color
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RectangleFilled(int x1, int y1, int x2, int y2,
    uint16_t color) {
    if (transparency) {
        if (color == _transparentColor)
            return;
    }
    if (x1 > x2)
        swap(x1, x2);
    if (y1 > y2)
        swap(y1, y2);
    if (x1 >= __width || x2 < 0 || y1 >= __height || y2 < 0)
        return;
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    //uint32_t pc = 0;
    int32_t xpos = x1;
    int32_t ypos = y1;
    pPos = (ypos * __scrWidth);
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint32_t pixels = ((x2 - x1) + 1) * ((y2 - y1) + 1);
    if (rotation == LANDSCAPE) {
        pto = tpto + pPos + (xpos << 1);
        while (pixels--) {
            if (xpos >= clipX1 && xpos <= clipX2 && ypos >= clipY1 &&
                ypos <= clipY2) {
                if (!alpha) {
                    pto[0] = color;
                    pto[1] = color >> 8;
                }
                else {
                    calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
                    pto[0] = __colour;
                    pto[1] = __colour >> 8;
                }
            }
            xpos++;
            pto += 2;
            if (xpos > x2) {
                xpos = x1;
                ypos++;
                pPos += __scrWidth;
                pto = tpto + pPos + (xpos << 1);
            }
        }
        if (frame_buffer == 0)
            FlushArea(y1, y2, -1);
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - ypos - 1) * __scrWidth) +
            (__scrWidth - ((xpos + 1) << 1));
        while (pixels--) {
            if (xpos >= clipX1 && xpos <= clipX2 && ypos >= clipY1 &&
                ypos <= clipY2) {
                if (!alpha) {
                    pto[0] = color;
                    pto[1] = color >> 8;
                }
                else {
                    calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
                    pto[0] = __colour;
                    pto[1] = __colour >> 8;
                }
            }
            xpos++;
            pto -= 2;
            if (xpos > x2) {
                xpos = x1;
                ypos++;
                pPos += __scrWidth;
                pto = tpto + ((__scrHeight - ypos - 1) * __scrWidth) +
                    (__scrWidth - ((xpos + 1) << 1));
            }
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - y2 - 1, __scrHeight - y1 - 1, -1);
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - ((ypos + 1) << 1)) +
            (xpos * __scrWidth);
        while (pixels--) {
            if (xpos >= clipX1 && xpos <= clipX2 && ypos >= clipY1 &&
                ypos <= clipY2) {
                if (!alpha) {
                    pto[0] = color;
                    pto[1] = color >> 8;
                }
                else {
                    calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
                    pto[0] = __colour;
                    pto[1] = __colour >> 8;
                }
            }
            xpos++;
            pto += __scrWidth;
            if (xpos > x2) {
                xpos = x1;
                ypos++;
                pto = tpto + (__scrWidth - (ypos << 1) - 2) +
                    (xpos * __scrWidth);
            }
        }
        if (frame_buffer == 0)
            FlushArea(x1, x2, -1);
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (ypos << 1) +
            ((__scrHeight - xpos - 1) * __scrWidth);
        while (pixels--) {
            if (xpos >= clipX1 && xpos <= clipX2 && ypos >= clipY1 &&
                ypos <= clipY2) {
                if (!alpha) {
                    pto[0] = color;
                    pto[1] = color >> 8;
                }
                else {
                    calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
                    pto[0] = __colour;
                    pto[1] = __colour >> 8;
                }
            }
            xpos++;
            pto -= __scrWidth;
            if (xpos > x2) {
                xpos = x1;
                ypos++;
                pto = tpto + (ypos << 1) +
                    ((__scrHeight - xpos - 1) * __scrWidth);
            }
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - x2 - 1, __scrHeight - x1 - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Select a frame buffer for all drawing actions.
  @param  sel 0 to 3
  @note   frame buffers are pre defined as empty and only sized to screen
  dimensions when the DrawToFramebuffer function is called.
*/
/****************************************************************************/
uint8_t* gfx4desp32_rgb_panel::SelectFB(uint8_t sel) {
    switch (sel) {
    case 0:
        return rgb_panel->fb;
        break;
    case 1:
        return psRAMbuffer3;
        break;
    case 2:
        return psRAMbuffer4;
        break;
    case 3:
        return psRAMbuffer5;
        break;
    case 4:
        return psRAMbuffer6;
        break;
    }
    return rgb_panel->fb;
}

void gfx4desp32_rgb_panel::AllocateDRcache(uint32_t cacheSize) {
    size_t psram_trans_align = rgb_panel->psram_trans_align;
    psRAMbuffer2 = (uint8_t*)heap_caps_aligned_calloc(psram_trans_align, 1, cacheSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    cache_Enabled = true;
}

void gfx4desp32_rgb_panel::AllocateFB(uint8_t sel) {
    size_t psram_trans_align = rgb_panel->psram_trans_align;
    if (sel == 0) {
        psRAMbuffer1 = (uint8_t*)heap_caps_aligned_calloc(psram_trans_align, 1, 1024000, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    }
    if (sel == 1) {
        psRAMbuffer3 = (uint8_t*)heap_caps_aligned_calloc(psram_trans_align, 1, __fbSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        framebufferInit1 = true;
    }
    if (sel == 2) {
        psRAMbuffer4 = (uint8_t*)heap_caps_aligned_calloc(psram_trans_align, 1, __fbSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        framebufferInit2 = true;
    }
    if (sel == 3) {
        psRAMbuffer5 = (uint8_t*)heap_caps_aligned_calloc(psram_trans_align, 1, __fbSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        framebufferInit3 = true;
    }
    if (sel == 4) {
        psRAMbuffer6 = (uint8_t*)heap_caps_aligned_calloc(psram_trans_align, 1, __fbSize, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        framebufferInit4 = true;
    }
}

void gfx4desp32_rgb_panel::CopyFrameBuffer(uint8_t fbto, uint8_t fbfrom1) {
    uint8_t* to = SelectFB(fbto);
    uint8_t* from1 = SelectFB(fbfrom1);
    memcpy(to, from1, __fbSize);
}

/****************************************************************************/
/*!
  @brief  Merge 2 frame buffers and send to specified frame buffer.
  @param  fbto - the sent to frame buffer usually 0
  @param  fbfrom1 - base frame buffer for the 2nd frame buffer to be merged to.
  @param  fbfrom2 - 2nd buffer to merge to the first;
  @param  transColor  - RGB565 colour equivelant to the transparent colour on
  the 2nd frame buffer
  @note   Using this function without first writing to a frame buffer will cause
          issue
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1,
    uint8_t fbfrom2,
    uint16_t transColor) {
    uint8_t* to = SelectFB(fbto);
    uint8_t* from1 = SelectFB(fbfrom1);
    uint8_t* from2 = SelectFB(fbfrom2);
    uint16_t tcol1, tcol2;
    uint32_t len = __fbSize >> 1;
    while (len--) {
        tcol1 = from1[0] + (from1[1] << 8);
        tcol2 = from2[0] + (from2[1] << 8);
        if (tcol2 != transColor)
            tcol1 = tcol2;
        to[0] = tcol1 & 0xff;
        to[1] = tcol1 >> 8;
        to += 2;
        from1 += 2;
        from2 += 2;
    }
    if (fbto == 0)
        FlushArea(0, __scrHeight, -1);
}

/****************************************************************************/
/*!
  @brief  Merge 3 frame buffers and send to specified frame buffer.
  @param  fbto - the sent to frame buffer usually 0
  @param  fbfrom1 - base frame buffer for the 2nd frame buffer to be merged to.
  @param  fbfrom2 - 2nd buffer to merge to the first;
  @param  fbfrom3 - 3rd buffer to merge to the first;
  @param  transColor  - RGB565 colour equivelant to the transparent colour on
  the 2nd frame buffer
  @param  transColor  - RGB565 colour equivelant to the transparent colour on
  the 3rd frame buffer
  @note   Using this function without first writing to a frame buffer will cause
    issue
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1,
    uint8_t fbfrom2, uint8_t fbfrom3,
    uint16_t transColor,
    uint16_t transColor1) {
    uint8_t* to = SelectFB(fbto);
    uint8_t* from1 = SelectFB(fbfrom1);
    uint8_t* from2 = SelectFB(fbfrom2);
    uint8_t* from3 = SelectFB(fbfrom3);
    uint16_t tcol1, tcol2, tcol3;
    uint32_t len = __fbSize >> 1;
    while (len--) {
        tcol1 = from1[0] + (from1[1] << 8);
        tcol2 = from2[0] + (from2[1] << 8);
        tcol3 = from2[0] + (from2[1] << 8);
        if (tcol2 != transColor)
            tcol1 = tcol2;
        if (tcol3 != transColor1)
            tcol1 = tcol3;
        to[0] = tcol1 & 0xff;
        to[1] = tcol1 >> 8;
        to += 2;
        from1 += 2;
        from2 += 2;
        from3 += 2;
    }
    if (fbto == 0)
        FlushArea(0, __scrHeight, -1);
}

/****************************************************************************/
/*!
  @brief  Draw a single pixel.
  @param  x left X position in pixels
  @param  y top Y position in pixels
  @param  color - RGB565 color
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::PutPixel(int16_t x, int16_t y, uint16_t color) {
    if (y > clipY2 || y < clipY1)
        return;
    if (x > clipX2 || x < clipX1)
        return;
    if (transparency) {
        if (color == _transparentColor)
            return;
    }
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    if (rotation == LANDSCAPE) {
        pto = tpto + (y * __scrWidth) + (x << 1);
        if (!alpha) {
            pto[0] = color;
            pto[1] = color >> 8;
        }
        else {
            calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
            pto[0] = __colour;
            pto[1] = __colour >> 8;
        }
        if (frame_buffer == 0)
            FlushArea(y, y, x);
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - y - 1) * __scrWidth) +
            (__scrWidth - ((x + 1) << 1));
        if (!alpha) {
            pto[0] = color;
            pto[1] = color >> 8;
        }
        else {
            calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
            pto[0] = __colour;
            pto[1] = __colour >> 8;
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - y - 1, __scrHeight - y - 1, (__scrWidth - x + 1));
    }
    if (rotation == PORTRAIT_R) {
        int32_t temp = (__scrWidth - ((y + 1) << 1)) + (__scrWidth * x);
        if (temp < 0 || temp > __fbSize)
            return;
        pto = tpto + temp;
        if (!alpha) {
            pto[0] = color;
            pto[1] = color >> 8;
        }
        else {
            calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
            pto[0] = __colour;
            pto[1] = __colour >> 8;
        }
        if (frame_buffer == 0)
            FlushArea(x, x, __scrWidth - y + 1);
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (y << 1) + ((__scrHeight - x - 1) * __scrWidth);
        if (!alpha) {
            pto[0] = color;
            pto[1] = color >> 8;
        }
        else {
            calcAlpha(color, pto[0] | (pto[1] << 8), __alpha);
            pto[0] = __colour;
            pto[1] = __colour >> 8;
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - x - 1, __scrHeight - x - 1, y);
    }
}

/****************************************************************************/
/*!
  @brief  Draw a fast horizontal line.
  @param  x left X position in pixels
  @param  y top Y position in pixels
  @param  w of line in pixels +ve or -ve
  @param  color - RGB565 color
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::Hline(int16_t x, int16_t y, int16_t w,
    uint16_t hcolor) {
    if (y > clipY2 || y < clipY1)
        return;
    if (x > clipX2 || (x + w - 1) < clipX1)
        return;
    if (transparency) {
        if (hcolor == _transparentColor)
            return;
    }
    if (w < 0) {
        x += w;
        //w *= -1;
        w = abs(w);
    }
    if (x < clipX1) {
        w -= clipX1 - x;
        x = clipX1;
    }
    if ((x + w) > clipX2 - 1)
        w = clipX2 - x + 1;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    int flushw = w;
    if (rotation == LANDSCAPE) {
        pto = tpto + (y * __scrWidth) + (x << 1);
        while (w--) {
            if (!alpha) {
                pto[0] = hcolor;
                pto[1] = hcolor >> 8;
            }
            else {
                calcAlpha(hcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto += 2;
        }
        if (frame_buffer == 0)
            FlushArea(y, y, -1);
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - y - 1) * __scrWidth) +
            (__scrWidth - ((x + 1) << 1));
        while (w--) {
            if (!alpha) {
                pto[0] = hcolor;
                pto[1] = hcolor >> 8;
            }
            else {
                calcAlpha(hcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto -= 2;
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - y - 1, __scrHeight - y - 1, -1);
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - ((y + 1) << 1)) +
            (x * __scrWidth);
        while (w--) {
            if (!alpha) {
                pto[0] = hcolor;
                pto[1] = hcolor >> 8;
            }
            else {
                calcAlpha(hcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto += __scrWidth;
        }
        if (frame_buffer == 0)
            FlushArea(x, x + flushw, -1);
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (y << 1) +
            ((__scrHeight - x - 1) * __scrWidth);
        while (w--) {
            if (!alpha) {
                pto[0] = hcolor;
                pto[1] = hcolor >> 8;
            }
            else {
                calcAlpha(hcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto -= __scrWidth;
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - x - 1 - flushw, __scrHeight - x - 1, -1);
    }
}

/****************************************************************************/
/*!
  @brief  Draw a fast vertical line.
  @param  x left X position in pixels
  @param  y top Y position in pixels
  @param  w of line in pixels +ve or -ve
  @param  color - RGB565 color
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::Vline(int16_t x, int16_t y, int16_t w,
    uint16_t vcolor) {
    if (x > clipX2 || x < clipX1)
        return;
    if (y > clipY2 || (y + w - 1) < clipY1)
        return;
    if (transparency) {
        if (vcolor == _transparentColor)
            return;
    }
    if (w < 0) {
        y += w;
        w *= -1;
    }
    if (y < clipY1) {
        w -= clipY1 - y;
        y = clipY1;
    }
    if ((y + w) > clipY2 - 1)
        w = clipY2 - y + 1;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    int flushw = w;
    if (rotation == LANDSCAPE) {
        pto = tpto + (y * __scrWidth) + (x << 1);
        while (w--) {
            if (!alpha) {
                pto[0] = vcolor;
                pto[1] = vcolor >> 8;
            }
            else {
                calcAlpha(vcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto += __scrWidth;
        }
        if (frame_buffer == 0)
            FlushArea(y, y + flushw, -1);
    }
    if (rotation == LANDSCAPE_R) {
        pto = tpto + ((__scrHeight - y - 1) * __scrWidth) +
            (__scrWidth - ((x + 1) << 1));
        while (w--) {
            if (!alpha) {
                pto[0] = vcolor;
                pto[1] = vcolor >> 8;
            }
            else {
                calcAlpha(vcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto -= __scrWidth;
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - y - 1 - flushw, __scrHeight - y - 1, -1);
    }
    if (rotation == PORTRAIT_R) {
        pto = tpto + (__scrWidth - ((y + 1) << 1)) +
            (x * __scrWidth);
        while (w--) {
            if (!alpha) {
                pto[0] = vcolor;
                pto[1] = vcolor >> 8;
            }
            else {
                calcAlpha(vcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto -= 2;
        }
        if (frame_buffer == 0)
            FlushArea(x, x, -1);
    }
    if (rotation == PORTRAIT) {
        pto = tpto + (y << 1) +
            ((__scrHeight - x - 1) * __scrWidth);
        while (w--) {
            if (!alpha) {
                pto[0] = vcolor;
                pto[1] = vcolor >> 8;
            }
            else {
                calcAlpha(vcolor, pto[0] | (pto[1] << 8), __alpha);
                pto[0] = __colour;
                pto[1] = __colour >> 8;
            }
            pto += 2;
        }
        if (frame_buffer == 0)
            FlushArea(__scrHeight - x - 1, __scrHeight - x - 1, -1);
    }
}

// void gfx4desp32_rgb_panel::drawBitmap(int x1, int y1, int x2, int y2,
//     uint16_t* c_data) {
//     esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2, y2, &c_data);
// }

/****************************************************************************/
/*!
  @brief  ESP32-S3 function
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::__start_transmission() {
    // reset FIFO of DMA and LCD, incase there remains old frame data
    gdma_reset(rgb_panel->dma_chan);
    lcd_ll_stop(rgb_panel->hal.dev);
    lcd_ll_fifo_reset(rgb_panel->hal.dev);
    gdma_start(rgb_panel->dma_chan, (intptr_t)rgb_panel->dma_nodes);
    // delay 1us is sufficient for DMA to pass data to LCD FIFO
    // in fact, this is only needed when LCD pixel clock is set too high
    esp_rom_delay_us(1);
    // start LCD engine
    lcd_ll_start(rgb_panel->hal.dev);
}

/****************************************************************************/
/*!
  @brief  Turn on Transparent mode
  @param  trans - transparency on / off
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::Transparency(bool trans) {
    transparency = trans;
    transalpha = alpha | transparency;

}

/****************************************************************************/
/*!
  @brief  Set transparent colour
  @param  color - RGB565 colour that won't be drawn in all functions when
          transparency enabled.
  @note   does not operate in pushColors function.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::TransparentColor(uint16_t color) {
    _transparentColor = color;
    _transMSB = color >> 8;
    _transLSB = color;
}

/****************************************************************************/
/*!
  @brief  Turn on Alpha Blend mode
  @param  alphablend - Alpha Blend on / off
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::AlphaBlend(bool alphablend) {
    alpha = alphablend;
    transalpha = alpha | transparency;
}

/****************************************************************************/
/*!
  @brief  Set Alpha Blend Level
  @param  alphaLevel - 0 to 255
  @note   does not operate in pushColors function.
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::AlphaBlendLevel(uint32_t alphaLevel) {
    __alpha = alphaLevel;
}

/****************************************************************************/
/*!
  @brief  IO Expander set Pin Mode
  @param  mode - INPUT or OUTPUT
  @note   sets the direction of GPIO on the IO expander. GPIO 6 & GPIO 7 are
          restricted after begin function is called
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::PinMode(byte pin, uint8_t mode) {
    if (IOexpInit && pin > 5)
        return;
    byte output_reg = 0;
    Wire.beginTransmission(GFX4d_IO_EXPANDER_ADDR);
    Wire.write((uint8_t)GFX4d_CONFIG_REG);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)GFX4d_IO_EXPANDER_ADDR, (uint8_t)1);
    output_reg = Wire.read();
    Wire.endTransmission();
    Wire.beginTransmission(GFX4d_IO_EXPANDER_ADDR);
    Wire.write((byte)GFX4d_CONFIG_REG);
    if (mode == INPUT) {
        output_reg |= (1 << pin);
    }
    else if (mode == OUTPUT) {
        output_reg &= ~(1 << pin);
    }
    Wire.write((uint8_t)output_reg);
    Wire.endTransmission();
}

void gfx4desp32_rgb_panel::DigitalWrite(byte pin, bool state) {
    byte output_reg = 0;
    Wire.beginTransmission(GFX4d_IO_EXPANDER_ADDR);
    Wire.write((uint8_t)GFX4d_OUTPUT_PORT_REG);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)GFX4d_IO_EXPANDER_ADDR, (uint8_t)1);
    output_reg = Wire.read();
    Wire.endTransmission();
    Wire.beginTransmission(GFX4d_IO_EXPANDER_ADDR);
    Wire.write((byte)GFX4d_OUTPUT_PORT_REG);
    if (state == HIGH) {
        output_reg |= (1 << pin);
    }
    else if (state == LOW) {
        output_reg &= ~(1 << pin);
    }
    Wire.write((uint8_t)output_reg);
    Wire.endTransmission();
}

int gfx4desp32_rgb_panel::DigitalRead(byte pin) {
    byte input_reg = 0;
    Wire.beginTransmission(GFX4d_IO_EXPANDER_ADDR);
    Wire.write((uint8_t)GFX4d_INPUT_PORT_REG);
    Wire.endTransmission();
    Wire.requestFrom((uint8_t)GFX4d_IO_EXPANDER_ADDR, (uint8_t)1);
    input_reg = Wire.read();
    Wire.endTransmission();
    if ((input_reg & (1 << pin)) > 0) {
        return 1;
    }
    else {
        return 0;
    }
}

/****************************************************************************/
/*!
  @brief  initialize PCF8563 RTC
  @note   Clears TESTC and CLKOUT
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCinit() {
    RTCwrite_AND(Control_status_1, 247); // clear TESTC bit
    RTCwrite_AND(CLKOUT_control, 127);   // clear CLKOUT enable bit
}
/****************************************************************************/
/*!
  @brief  Start the clock
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCstartClock() {
    RTCwrite_AND(Control_status_1, ~(1 << 5));
}
/****************************************************************************/
/*!
  @brief  Stop the clock
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCstopClock() {
    RTCwrite_OR(Control_status_1, 1 << 5);
}

/****************************************************************************/
/*!
  @brief  Set Year
  @param  Year - 0 to 99
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCsetYear(uint16_t year) {
    uint8_t y1, y2;
    uint16_t year100;
    year100 = year / 100;
    if (year100 == 20) {
        y1 = RTCread(Century_months) & 0x7f;
    }
    else {
        y1 = (RTCread(Century_months) & 0x7f) + 0x80;
    }
    y2 = year - (year100 * 100);
    const uint8_t data = ((get_second_number(y2)) << 4) | (get_first_number(y2));
    RTCwrite(Century_months, y1);
    RTCwrite(Years, data);
}
/****************************************************************************/
/*!
  @brief  Set Month
  @param  Month - 1 to 12
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCsetMonth(uint8_t month) {
    uint8_t century = RTCread(Century_months) & 0x80;
    const uint8_t data =
        ((get_second_number(month)) << 4) | (get_first_number(month));
    RTCwrite(Century_months, (century | (data & 0x7f)));
}

/****************************************************************************/
/*!
  @brief  Set Day
  @param  Day - 1 to 31
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCsetDay(uint8_t day) {
    const uint8_t data =
        ((get_second_number(day)) << 4) | (get_first_number(day));
    RTCwrite(Days, data);
}

/****************************************************************************/
/*!
  @brief  Set Hour
  @param  Hour - 0 to 24
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCsetHour(uint8_t hour) {
    const uint8_t data =
        ((get_second_number(hour)) << 4) | (get_first_number(hour));
    RTCwrite(Hours, data);
}

/****************************************************************************/
/*!
  @brief  Set Minute
  @param  Minute - 0 to 59
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCsetMinute(uint8_t minute) {
    const uint8_t data =
        ((get_second_number(minute)) << 4) | (get_first_number(minute));
    RTCwrite(Minutes, data);
}

/****************************************************************************/
/*!
  @brief  Set Seconds
  @param  Seconds - 0 to 99
*/
/****************************************************************************/
void gfx4desp32_rgb_panel::RTCsetSecond(uint8_t second) {
    const uint8_t data =
        ((get_second_number(second)) << 4) | (get_first_number(second));
    RTCwrite(VL_seconds, data);
}

/****************************************************************************/
/*!
  @brief  Get Current Time
  @note   Time (current code encoded into the Time structure)
*/
/****************************************************************************/
Time gfx4desp32_rgb_panel::RTCgetTime() {
    uint8_t century = (RTCread(Century_months) & 0x80) >> 7;
    uint16_t year100;
    if (century) {
        year100 = 1900;
    }
    else {
        year100 = 2000;
    }
    Time output;
    // read data registers contents
    const uint16_t YEAR = RTCread(Years);
    const uint8_t MONTH = RTCread(Century_months);
    const uint8_t DAY = RTCread(Days);
    const uint8_t WEEKDAY = RTCread(Weekdays);
    const uint8_t HOUR = RTCread(Hours);
    const uint8_t MINUTE = RTCread(Minutes);
    const uint8_t SECONDS = RTCread(VL_seconds);
    // convert readed data to numbers using bcd_to_number function).
    output.year =
        year100 + bcd_to_number((YEAR & 0b11110000) >> 4, YEAR & 0b00001111);
    output.month = bcd_to_number((MONTH & 0b00010000) >> 4, MONTH & 0b00001111);
    output.day = bcd_to_number((DAY & 0b00110000) >> 4, DAY & 0b00001111);
    output.weekday = bcd_to_number(0, WEEKDAY & 0b00000111);
    output.hour = bcd_to_number((HOUR & 0b00110000) >> 4, HOUR & 0b00001111);
    output.minute =
        bcd_to_number((MINUTE & 0b01110000) >> 4, MINUTE & 0b00001111);
    output.second =
        bcd_to_number((SECONDS & 0b01110000) >> 4, SECONDS & 0b00001111);
    return output;
}

/****************************************************************************/
/*!
  @brief  Check clock integrity
  @note   returns clock status
*/
/****************************************************************************/
bool gfx4desp32_rgb_panel::RTCcheckClockIntegrity() {
    const uint8_t data = RTCread(VL_seconds); // read the data
    if (data & (1 << 7)) {
        return 0; // if clock integrity is not guaranteed return 0
    }
    else {
        return 1; // otherwise return 1
    }
}

// Read one byte of data
// Parameters:
//  * uint8_t address  - register read_address
// Returns: readed byte of data (uint8_t)
uint8_t gfx4desp32_rgb_panel::RTCread(uint8_t address) {
    Wire.beginTransmission(PCF8563_address); // begin transmission
    Wire.write(address); // inform chip what register we want to read
    Wire.endTransmission();
    Wire.requestFrom(PCF8563_address, 1); // request one byte from the chip
    uint8_t data = Wire.read();           // read the data
    return data;
}

// Convert BCD format to number
// Parameters:
//  * uint8_t first -> first digit
//  * uint8_t second -> second digit
// Returns: the result of the conversion (unsigned char)
unsigned char gfx4desp32_rgb_panel::bcd_to_number(uint8_t first,
    uint8_t second) {
    unsigned char output;
    output = first * 10;
    output = output + second;
    return output;
}

// Get first digit of the number
// Parameters:
//  * unsigned short ->
// Returns: digit (uint8_t)
uint8_t gfx4desp32_rgb_panel::get_first_number(unsigned short number) {
    uint8_t output = number % 10;
    return output;
}

// Get second digit of the number
// Parameters:
//  * unsigned short ->
// Returns: digit (uint8_t)
uint8_t gfx4desp32_rgb_panel::get_second_number(unsigned short number) {
    uint8_t output = number / 10;
    return output;
}

// Write one byte of data
// Parameters:
//  * uint8_t address  - register read_address
//  * uint8_t data     - byte of data that we want to write to the register
// Returns: none
void gfx4desp32_rgb_panel::RTCwrite(uint8_t address, uint8_t data) {
    Wire.beginTransmission(PCF8563_address);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission();
}

// Change state of the register using OR operation
// Parameters:
//  * uint8_t address    - register address
//  * uint8_t data       - one byte of data that we want to put in the register
// Returns: none
void gfx4desp32_rgb_panel::RTCwrite_OR(uint8_t address, uint8_t data) {
    uint8_t c = RTCread(address);
    c = c | data;
    RTCwrite(address, c);
}

// Change state of the register using AND operation
// Parameters:
//  * uint8_t address    - register address
//  * uint8_t data       - one byte of data that we want to put in the register
// Returns: none
void gfx4desp32_rgb_panel::RTCwrite_AND(uint8_t address, uint8_t data) {
    uint8_t c = RTCread(address);
    c = c & data;
    RTCwrite(address, c);
}

const char* gfx4desp32_rgb_panel::RTCformatDateTime(uint8_t sytle) {
    Time t = RTCgetTime();
    switch (sytle) {
    case RTC_TIMEFORMAT_HM:
        snprintf(format, sizeof(format), "%02d:%02d", t.hour, t.minute);
        break;
    case RTC_TIMEFORMAT_HMS:
        snprintf(format, sizeof(format), "%02d:%02d:%02d", t.hour, t.minute,
            t.second);
        break;
    case RTC_TIMEFORMAT_YYYY_MM_DD:
        snprintf(format, sizeof(format), "%04d-%02d-%02d", t.year, t.month, t.day);
        break;
    case RTC_TIMEFORMAT_MM_DD_YYYY:
        snprintf(format, sizeof(format), "%02d-%02d-%04d", t.month, t.day, t.year);
        break;
    case RTC_TIMEFORMAT_DD_MM_YYYY:
        snprintf(format, sizeof(format), "%02d-%02d-%04d", t.day, t.month, t.year);
        break;
    case RTC_TIMEFORMAT_YYYY_MM_DD_H_M_S:
        snprintf(format, sizeof(format), "%d-%d-%d/%d:%d:%d", t.year, t.month,
            t.day, t.hour, t.minute, t.second);
        break;
    case RTC_TIMEFORMAT_DD_MM_YYYY_H_M_S:
        snprintf(format, sizeof(format), "%02d-%02d-%04d/%02d:%02d:%02d", t.day,
            t.month, t.year, t.hour, t.minute, t.second);
        break;
    default:
        snprintf(format, sizeof(format), "%02d:%02d", t.hour, t.minute);
        break;
    }
    return format;
}
