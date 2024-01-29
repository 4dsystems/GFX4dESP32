#include "gfx4desp32_spi_panel.h"

#define swap(a, b)                                                            \
    {                                                                         \
        int16_t tab = a;                                                      \
        a = b;                                                                \
        b = tab;                                                              \
    }

#define calcAlpha(a, b, c)                                                    \
    {                                                                         \
        __alphatemp = c >> 3;                                                 \
        uint32_t fgu = a;                                                     \
        uint32_t bgu = b;                                                     \
        uint32_t fg = (fgu | (fgu << 16)) & 0x07e0f81f;                       \
        uint32_t bg = (bgu | (bgu << 16)) & 0x07e0f81f;                       \
        bg += (fg - bg) * __alphatemp >> 5; bg &= 0x07e0f81f;                 \
        __colour = (uint16_t)(bg | bg >> 16);                                 \
    }                                                                         \


#include "esp_lcd_panel_io.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_types.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "hal/gpio_hal.h"
#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"
#include "Wire.h"




gfx4desp32_spi_panel::gfx4desp32_spi_panel(
    int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI, int panel_Pin_MISO,
    int panel_Pin_CLK, int panel_Pin_RST, int bk_pin,
    int bk_on_level, int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO,
    int sd_gpio_MOSI, int sd_gpio_CS, int hres, int vres, bool touchXinvert)
    : gfx4desp32() {
    panelPin_CS = panel_Pin_CS;
    panelPin_DC = panel_Pin_DC;
    panelPin_MOSI = panel_Pin_MOSI;
    panelPin_MISO = panel_Pin_MISO;
    panelPin_CLK = panel_Pin_CLK;
    panel_HRES = hres;
    panel_VRES = vres;
    panelPin_RST = panel_Pin_RST;

    this->bk_config.pin = bk_pin;
    this->bk_config.on_level = bk_on_level;
    this->bk_config.off_level = bk_off_level;
    sd_sck = sd_gpio_SCK;
    sd_miso = sd_gpio_MISO;
    sd_mosi = sd_gpio_MOSI;
    sd_cs = sd_gpio_CS;
    backlight = bk_pin;
    v_res = vres;
    h_res = hres;
    __TImode = touchXinvert;
}

gfx4desp32_spi_panel::~gfx4desp32_spi_panel() {}

void gfx4desp32_spi_panel::DisplayControl(uint8_t cmd, uint32_t val) {

}

void gfx4desp32_spi_panel::DisplayControl(uint8_t cmd) {
    switch (cmd) {
    case 1:
        esp_lcd_panel_reset(panel_handle);
        break;
    case 8:
        FlushArea(0, __scrHeight, -1);
        break;
    }
}

esp_lcd_panel_handle_t gfx4desp32_spi_panel::__begin() {

    ESP_LOGI(TAG, "Turn off LCD Backlight");
    gpio_config_t bk_gpio_config = {
        .pin_bit_mask = 1ULL << bk_config.pin,
        .mode = GPIO_MODE_OUTPUT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI(TAG, "Install SPI LCD panel driver");

    displayBus = DISPLAY_BUS_SPI;
    spi_bus_config_t bus_config = {};
    bus_config.sclk_io_num = panelPin_CLK;  // CLK
    bus_config.mosi_io_num = panelPin_MOSI; // MOSI
    bus_config.miso_io_num = panelPin_MISO; // MISO
    bus_config.quadwp_io_num = -1;          // Not used
    bus_config.quadhd_io_num = -1;          // Not used

    if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
        bus_config.max_transfer_sz = h_res * 54 * 2;
    }
    else {
        bus_config.max_transfer_sz = h_res * 22 * 3;
    }
    spi_bus_initialize(GEN4_35CT_SPI_HOST, &bus_config, SPI_DMA_CH_AUTO);

    esp_lcd_panel_io_spi_config_t io_config = {};
    io_config.dc_gpio_num = panelPin_DC;
    io_config.cs_gpio_num = panelPin_CS;
    if (changePCLK) {
        io_config.pclk_hz = PCLKval;
        changePCLK = false;
    }
    else {
        io_config.pclk_hz = 40 * 1000 * 1000; // maximum spi dislay clock speed
    }
    io_config.lcd_cmd_bits = 8;
    io_config.lcd_param_bits = 8;
    io_config.spi_mode = 0;
    io_config.trans_queue_depth = 7;
    esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)GEN4_35CT_SPI_HOST,
        &io_config, &io_handle);

    delay(100);

    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.reset_gpio_num = panelPin_RST;
    panel_config.color_space = LCD_RGB_ENDIAN_BGR;
    panel_config.bits_per_pixel = 16;
    esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle);

    esp_lcd_panel_reset(panel_handle);

    delay(120);

    if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
        int regcount;
        // Autodetect IlI9341 v ST7789
        for (regcount = 0; regcount < 4; regcount++) {
            dData[0] = 0x10 + regcount;
            esp_lcd_panel_io_tx_param(io_handle, 0xd9, dData, 1);
            esp_lcd_panel_io_tx_param(io_handle, 0xd3, NULL, 0);
            esp_lcd_panel_io_rx_param(io_handle, -1, dispID + regcount, 1);
        }

        if (dispID[2] == 147 && dispID[3] == 65) {
            dispST7789 = false;
        }
        else {
            dispST7789 = true;
        }

        if (!dispST7789)
            ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    }
    delay(120);

    int ic = 0;
    uint8_t* InitCommands = SelectINIT(dispST7789);
    uint8_t len = InitCommands[0];
    uint8_t command;
    while (len != 0xff) {
        command = InitCommands[ic + 1];
        if (len == 0xfe) {
            delay(command * 10);
            ic += 2;
        }
        else {
            for (int n = 0; n < len; n++) {
                InitData[n] = InitCommands[ic + 2 + n];
            }
            if (len != 0) {
                tx_param(command, InitData, len);
            }
            else {
                tx_param(command, NULL, 0);
            }
            ic += (len + 2);
        }
        len = InitCommands[ic];
    }
    delay(120);
    if (DisplayModel == GFX4d_DISPLAY_ILI9488) {
        esp_lcd_panel_io_tx_param(io_handle, ILI9488_CMD_SLEEP_OUT, NULL, 0);
        delay(120);
        esp_lcd_panel_io_tx_param(io_handle, ILI9488_CMD_DISPLAY_ON, NULL, 0);
        delay(200);
        esp_lcd_panel_io_tx_param(io_handle, ILI9488_CMD_DISP_INVERSION_ON, NULL,
            0);
    }
    else {
        if (IPS_Display) {
            esp_lcd_panel_invert_color(panel_handle, true);
        }

        esp_lcd_panel_disp_on_off(panel_handle, true);
        delay(120);
        esp_lcd_panel_mirror(panel_handle, true, false);
    }
    ESP_LOGI(TAG, "Turn on LCD Backlight");
    //gpio_set_level((gpio_num_t)bk_config.pin, bk_config.on_level);

    /*** Set some Initial variables ***/
    clipX1pos = 0;
    clipY1pos = 0;
    clipX2pos = (int)h_res - 1;
    clipY2pos = (int)v_res - 1;
    ClipWindow(0, 0, (int)h_res - 1, (int)v_res - 1);
    Clipping(true);
    Clipping(false);

    __scrWidth = (int)h_res << 1;
    __scrHeight = (int)v_res;
    __fbSize = v_res * __scrWidth;
    __width = h_res;
    __height = v_res;

    /*** Scroll window set to maximum for GFX4dESP32 compatibilty ***/
    scroll_X1 = 0;
    scroll_Y1 = 0;
    scroll_X2 = __width - 1;
    scroll_Y2 = __height - 1;
    ledcAttach(backlight, 25000, 10);

    //Contrast(15);

    /*** Initiaize main frame buffer ***/
    fb = (uint8_t*)ps_malloc(__fbSize);
    /*** if ILI9488 Initialize 18bit buffer ***/
    if (DisplayModel == GFX4d_DISPLAY_ILI9488) {
        fb35 = (uint8_t*)ps_malloc(460800);
        lvglBufferInit = true;
    }
    DisplayType = DISP_INTERFACE_SPI;
    return panel_handle;
}

void gfx4desp32_spi_panel::tx_param(int32_t lcd_cmd, const void* param, size_t param_size)
{
    esp_lcd_panel_io_tx_param(io_handle, lcd_cmd, param, param_size);
}

void gfx4desp32_spi_panel::tx_color(int32_t lcd_cmd, const void* param, size_t param_size)
{
    esp_lcd_panel_io_tx_color(io_handle, lcd_cmd, param, param_size);
}

/****************************************************************************/
/*!
  @brief  Set display brightness
  @param  cntrst    0 - 15
  @note   ESP32 requires pin setup for Analog Write.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::Contrast(int ctrst) {
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
  @note   compatible function for early IoD's. Sets backlight fully on.
          Needs coding.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::BacklightOn(bool blight) {
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
  @param  r    Rotation 0 - 3 (LANDSCAPE, LANDSCAPE_R, PORTRAIT, PORTRAIT_R)
*/
/****************************************************************************/
void gfx4desp32_spi_panel::panelOrientation(uint8_t r) {
    rotation = r % 4;
    // rotation = 0;
    if (rotation < 2) {
        if (rotation == LANDSCAPE_R) {
            esp_lcd_panel_mirror(panel_handle, true, true);
            esp_lcd_panel_swap_xy(panel_handle, true);
        }
        if (rotation == LANDSCAPE) {
            esp_lcd_panel_mirror(panel_handle, false, false);
            esp_lcd_panel_swap_xy(panel_handle, true);
        }
        ClipWindow(0, 0, (int)h_res - 1, (int)v_res - 1);
        Clipping(1);
        Clipping(0);
        __width = h_res;
        __height = v_res;
    }
    else {
        if (rotation == PORTRAIT) {
            esp_lcd_panel_mirror(panel_handle, true, false);
            esp_lcd_panel_swap_xy(panel_handle, false);
        }
        if (rotation == PORTRAIT_R) {
            esp_lcd_panel_mirror(panel_handle, false, true);
            esp_lcd_panel_swap_xy(panel_handle, false);
        }
        ClipWindow(0, 0, (int)v_res - 1, (int)h_res - 1);
        Clipping(1);
        Clipping(0);
        __width = v_res;
        __height = h_res;
    }
    clippingON = false;
    wrGRAM = 0;
    scroll_X1 = 0;
    scroll_Y1 = 0;
    scroll_X2 = __width - 1;
    scroll_Y2 = __height - 1;
    __scrWidth = __width << 1;
    __scrHeight = __height;
}

/**********************************************************************/
/*!
  @brief      Get height in current Orientation
  @returns    __height    height in pixels
*/
/**********************************************************************/
int16_t gfx4desp32_spi_panel::getHeight(void) { return __height; }

/**********************************************************************/
/*!
  @brief      Get width in current Orientation
  @returns    __width height in pixels
*/
/**********************************************************************/
int16_t gfx4desp32_spi_panel::getWidth(void) { return __width; }

/**********************************************************************/
/*!
  @brief      Get Maximum Y scroll window value in current Orientation
  @returns    scroll_Y2   position in pixels
  @note       Compatipble GFX4dESP32 function used by write and newline.
*/
/**********************************************************************/
int16_t gfx4desp32_spi_panel::getScrollareaY1() { return (int16_t)scroll_Y2; }

/****************************************************************************/
/*!
  @brief      Get current Screen Orientation
  @returns    rotation 0 - 3 (LANDSCAPE, LANDSCAPE_R, PORTRAIT, PORTRAIT_R)
*/
/****************************************************************************/
uint8_t gfx4desp32_spi_panel::getPanelOrientation(void) { return rotation; }

/****************************************************************************/
/*!
  @brief  Set Invert mode
  @param  Inv    true / false
  @note   Needs coding. Not sure how to deal with this on RGB displays
*/
/****************************************************************************/
void gfx4desp32_spi_panel::Invert(bool Inv) {
    if (DisplayModel == GFX4d_DISPLAY_ILI9488) {
        if (Inv) {
            esp_lcd_panel_io_tx_param(io_handle, ILI9488_CMD_DISP_INVERSION_OFF, NULL, 0);
        }
        else {
            esp_lcd_panel_io_tx_param(io_handle, ILI9488_CMD_DISP_INVERSION_ON, NULL, 0);
        }
    }
    else {
        if (IPS_Display) {
            if (Inv) {
                esp_lcd_panel_invert_color(panel_handle, false);
            }
            else {
                esp_lcd_panel_invert_color(panel_handle, true);
            }
        }
        else {
            if (Inv) {
                esp_lcd_panel_invert_color(panel_handle, true);
            }
            else {
                esp_lcd_panel_invert_color(panel_handle, false);
            }
        }
    }
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
void gfx4desp32_spi_panel::SetGRAM(int16_t x1, int16_t y1, int16_t x2,
    int16_t y2) {
    GRAMx1 = x1;
    GRAMy1 = y1;
    GRAMx2 = x2;
    GRAMy2 = y2;
    wrGRAM = true;
    GRAMxpos = x1;
    GRAMypos = y1;
    pixelPos = 0;
    pixelCount = (x2 - x1) * (y2 - y1);
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
bool gfx4desp32_spi_panel::StartWrite() {
    if (writeFBonly)
        return false; // return if already initiated
    writeFBonly = true;
    WriteFBonly = true;
    low_Y = __scrHeight - 1;
    high_Y = 0;
    low_X = (__scrWidth >> 1) - 1;
    high_X = 0;
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
void gfx4desp32_spi_panel::EndWrite() {
    writeFBonly = false;
    WriteFBonly = false;
    high_Y++;
    if (high_Y > __scrHeight - 1)
        high_Y = __scrHeight - 1;
    if (low_Y < 0)
        low_Y = 0;
    if (low_Y > high_Y)
        return;
    if (low_X < 0)
        low_X = 0;
    if (high_X > ((__scrWidth >> 1) - 1))
        high_X = (__scrWidth >> 1) - 1;
    if (low_X > high_X)
        return;
    int h = high_Y - low_Y + 1;
    int w = high_X - low_X + 1;

    uint8_t* pto = fb;

    pto = fb + (low_Y * __scrWidth) + (low_X << 1);
    if (low_Y > (__height - 1) || low_X > (__width - 1))
        return;
    if (!lvglBufferInit) {
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            psRAMlvglBuffer = (uint16_t*)ps_malloc(__fbSize);
        }
        else {
            fb35 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
        }
        lvglBufferInit = true;
    }
    uint32_t pc = 0;
    if (DisplayModel == GFX4d_DISPLAY_ILI9488) {
        int n;
        uint16_t tcol;
        uint32_t pixSize = w * h;
        while (h--) {
            for (n = 0; n < (w << 1); n += 2) {
                tcol = (pto[n] << 8) + pto[n + 1];
                fb35[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            pto += __scrWidth;
        }
        uint8_t command[10];
        command[0] = (low_X >> 8) & 0xff;
        command[1] = low_X & 0xff;
        command[2] = (high_X >> 8) & 0xff;
        command[3] = high_X & 0xff;
        esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
        command[0] = (low_Y >> 8) & 0xff;
        command[1] = low_Y & 0xff;
        command[2] = (high_Y >> 8) & 0xff;
        command[3] = high_Y & 0xff;
        esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
        esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
            pixSize * 3);
    }
    else {
        while (h--) {
            memcpy(psRAMlvglBuffer + (pc * (w)), pto, w << 1);
            pc++;
            pto += __scrWidth;
        }
        esp_lcd_panel_draw_bitmap(panel_handle, low_X, low_Y, high_X + 1,
            high_Y + 1, psRAMlvglBuffer);
    }
}

/****************************************************************************/
/*!
  @brief  Fill entire frame buffer with colour and flush
  @param  color 16 bit RGB565 colour
*/
/****************************************************************************/
void gfx4desp32_spi_panel::FillScreen(uint16_t color) {
    uint32_t total = __fbSize >> 1; // total size of framebuffer in pixels
    uint8_t* pto = SelectFB(frame_buffer);
    while (total--) {
        pto[0] = color >> 8;
        pto[1] = color;
        pto += 2;
    }
    if (frame_buffer == 0)
        FlushArea(0, __scrHeight, -1);
}

/****************************************************************************/
/*!
  @brief  Draw frame buffer area relative to GCI widget from selected to buffer
  to target buffer
  @param  fbnum - frame buffer number to draw from.
              ui - GCI widget index
*/
/****************************************************************************/
void gfx4desp32_spi_panel::DrawFrameBufferArea(uint8_t fbnum, int16_t ui) {
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
void gfx4desp32_spi_panel::DrawFrameBufferArea(uint8_t fbnum, int16_t x1,
    int16_t y1, int16_t x2,
    int16_t y2) {
    uint8_t* tfrom = SelectFB(fbnum);
    uint8_t* to = SelectFB(frame_buffer);
    int32_t x_start;
    int32_t y_start;
    int32_t x_end;
    int32_t y_end;
    uint8_t colM, colL;
    x_start = x1;
    y_start = y1;
    x_end = x2;
    y_end = y2;
    if (x_start >= __width || x_end < 0 || y_start >= __height || y_end < 0)
        return;
    if (x_end >= __width)
        x_end = __width - 1;
    if (y_end >= __height)
        y_end = __height - 1;
    uint32_t s_width = x_end - x_start + 1;
    uint32_t s_height = y_end - y_start + 1;
    uint32_t pc = (y_start * __scrWidth) + (x_start << 1);
    int twidth;
    to += pc;
    tfrom += pc;
    while (s_height--) {
        if (!transalpha) {
            memcpy(to, tfrom, s_width << 1);
            to += __scrWidth;
            tfrom += __scrWidth;
        }
        else {
            twidth = s_width;
            while (twidth--) {
                colM = tfrom[0];
                colL = tfrom[1];
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), to[1] | (to[0] << 8), __alpha);
                        to[1] = __colour;
                        to[0] = __colour >> 8;
                    }
                    else {
                        to[0] = colM;
                        to[1] = colL;
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
        FlushArea(x_start, x_end, y_start, y_end, -1);
}

/****************************************************************************/
/*!
  @brief  Draw selected frame buffer to the display
  @param  fbnum - frame buffer number to draw.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::DrawFrameBuffer(uint8_t fbnum) {
    uint8_t* tfrom = SelectFB(fbnum);
    uint8_t* to = fb;
    memcpy(to, tfrom, __fbSize);
    FlushArea(0, __scrHeight, -1);
}

/****************************************************************************/
/*!
  @brief  Write array of colour data to selected frame buffer to the display
  @param  fbnum - frame buffer number to write to.
  @param  offset - position in destination frame buffer number to write to.
  @param  data - arrary containing 8 bit data.
  @param  len - length of data array.
  @note   function for direct frame buffer writing without any x or y position
          Care to be taken to not exceed the current frame buffer size.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::WriteToFrameBuffer(uint32_t offset, uint8_t* data, uint32_t len) {
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
            calcAlpha(colL + (colM << 8), pto[1] | (pto[0] << 8), __alpha);
            pto[1] = __colour;
            pto[0] = __colour >> 8;
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
void gfx4desp32_spi_panel::WriteToFrameBuffer(uint32_t offset, uint16_t* data, uint32_t len) {
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
            calcAlpha(colL + (colM << 8), pto[1] | (pto[0] << 8), __alpha);
            pto[1] = __colour;
            pto[0] = __colour >> 8;
            pto += 2;
        }
    }
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
void gfx4desp32_spi_panel::WrGRAMs(uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return; // return if GRAM window not set.
    uint32_t pc = 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t colM, colL;
    pto = tpto + (GRAMypos * __scrWidth) +
        (GRAMxpos << 1); // set frame buffer pointer to current x, y position
    while (len--) {
        if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
            GRAMypos <= clipY2) { // check if inside clip region
            colM = color_data[pc++];
            colL = color_data[pc++];
            if (!transalpha) {
                pto[0] = colM;
                pto[1] = colL;
            }
            else {
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), pto[1] | (pto[0] << 8), __alpha);
                        pto[1] = __colour;
                        pto[0] = __colour >> 8;
                    }
                    else {
                        pto[0] = colM;
                        pto[1] = colL;
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
    if (pixelPos >= pixelCount &&
        frame_buffer == 0) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        // CPU writes data to PSRAM through DCache, data in PSRAM might not get
        // updated, so write back
        FlushArea(GRAMx1, GRAMx2, GRAMy1, GRAMy2, -1);
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
void gfx4desp32_spi_panel::WrGRAMs(const uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return; // return if GRAM window not set.
    uint32_t pc = 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t colM, colL;
    pto = tpto + (GRAMypos * __scrWidth) +
        (GRAMxpos << 1); // set frame buffer pointer to current x, y position
    while (len--) {
        if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
            GRAMypos <= clipY2) { // check if inside clip region
            colM = color_data[pc++];
            colL = color_data[pc++];
            if (!transalpha) {
                pto[0] = colM;
                pto[1] = colL;
            }
            else {
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), pto[1] | (pto[0] << 8), __alpha);
                        pto[1] = __colour;
                        pto[0] = __colour >> 8;
                    }
                    else {
                        pto[0] = colM;
                        pto[1] = colL;
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
    if (GRAMypos > GRAMy2 &&
        frame_buffer == 0) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        // CPU writes data to PSRAM through DCache, data in PSRAM might not get
        // updated, so write back
        FlushArea(GRAMx1, GRAMx2, GRAMy1, GRAMy2, -1);
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
void gfx4desp32_spi_panel::FlushArea(int y1, int y2, int xpos) {
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
    int h = y2 - y1 + 1;
    int x0 = 0;
    int x1 = __width - 1;
    uint8_t command[10];
    command[0] = (x0 >> 8) & 0xff;
    command[1] = x0 & 0xff;
    command[2] = (x1 >> 8) & 0xff;
    command[3] = x1 & 0xff;
    esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
    command[0] = (y1 >> 8) & 0xff;
    command[1] = y1 & 0xff;
    command[2] = (y2 >> 8) & 0xff;
    command[3] = y2 & 0xff;
    esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
    if (DisplayModel == GFX4d_DISPLAY_ILI9488) {
        uint32_t pc = 0;
        uint8_t* pto = fb + ((uint32_t)((uint32_t)y1 * __scrWidth));
        uint32_t n;
        uint16_t tcol;
        uint32_t pixSize = __width * (y2 - y1 + 1);
        for (n = 0; n < (pixSize << 1); n += 2) {
            tcol = (pto[0] << 8) + pto[1];
            fb35[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
            fb35[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
            fb35[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            pto += 2;
        }
        if (pixSize > 76800) {
            esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                230400);
            esp_lcd_panel_io_tx_color(io_handle, -1, fb35 + 230400,
                (pixSize * 3) - 230400);
        }
        else {
            esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                pixSize * 3);
        }

    }
    else {
        esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE,
            fb + (y1 * __scrWidth), __scrWidth);
        h--;
        y1++;
        while (h--) {
            esp_lcd_panel_io_tx_color(io_handle, -1, fb + (y1 * __scrWidth),
                __scrWidth);
            y1++;
        }
    }
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
void gfx4desp32_spi_panel::FlushArea(int x1, int x2, int y1, int y2, int xpos) {
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
    uint8_t* pto = fb;

    pto = fb + (y1 * __scrWidth) + (x1 << 1);

    if (!lvglBufferInit) {
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            psRAMlvglBuffer = (uint16_t*)ps_malloc(__fbSize);
        }
        else {
            fb35 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
        }
        lvglBufferInit = true;
    }
    uint32_t pc = 0;
    if (DisplayModel == GFX4d_DISPLAY_ILI9488) {

        int n;
        uint16_t tcol;
        uint32_t pixSize = w * h;
        while (h--) {
            for (n = 0; n < (w << 1); n += 2) {
                tcol = (pto[n] << 8) + pto[n + 1];
                fb35[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            pto += __scrWidth;
        }
        uint8_t command[10];
        command[0] = (x1 >> 8) & 0xff;
        command[1] = x1 & 0xff;
        command[2] = (x2 >> 8) & 0xff;
        command[3] = x2 & 0xff;
        esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
        command[0] = (y1 >> 8) & 0xff;
        command[1] = y1 & 0xff;
        command[2] = (y2 >> 8) & 0xff;
        command[3] = y2 & 0xff;
        esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
        if (pixSize > 76800) {
            esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                230400);
            esp_lcd_panel_io_tx_color(io_handle, -1, fb35 + 230400,
                (pixSize * 3) - 230400);
        }
        else {
            esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                pixSize * 3);
        }
    }
    else {
        while (h--) {
            memcpy(psRAMlvglBuffer + (pc * (w)), pto, w << 1);
            pc++;
            pto += __scrWidth;
        }
        esp_lcd_panel_draw_bitmap(panel_handle, x1, y1, x2 + 1, y2 + 1,
            psRAMlvglBuffer);
    }
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
void gfx4desp32_spi_panel::WrGRAMs(uint16_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint16_t tcol;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t colM, colL;
    pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
    while (len--) {
        tcol = color_data[pc++];
        if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
            GRAMypos <= clipY2) {
            colM = tcol >> 8;
            colL = tcol & 0xff;
            if (!transalpha) {
                pto[0] = colM;
                pto[1] = colL;
            }
            else {
                if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                    if (alpha) {
                        calcAlpha(colL + (colM << 8), pto[1] | (pto[0] << 8), __alpha);
                        pto[1] = __colour;
                        pto[0] = __colour >> 8;
                    }
                    else {
                        pto[0] = colM;
                        pto[1] = colL;
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

    if (pixelPos >= pixelCount) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        if (frame_buffer == 0)
            FlushArea(GRAMx1, GRAMx2, GRAMy1, GRAMy2, -1);
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
void gfx4desp32_spi_panel::WrGRAMs(uint32_t* color_data, uint16_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint32_t tcol;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint8_t div;
    int innerloop;
    uint8_t colM, colL;
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
                    pto[0] = colM;
                    pto[1] = colL;
                }
                else {
                    if (!(transparency && (colM == _transMSB && colL == _transLSB))) {
                        if (alpha) {
                            calcAlpha(colL + (colM << 8), pto[1] | (pto[0] << 8), __alpha);
                            pto[1] = __colour;
                            pto[0] = __colour >> 8;
                        }
                        else {
                            pto[0] = colM;
                            pto[1] = colL;
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

    if (pixelPos >= pixelCount) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        if (frame_buffer == 0)
            FlushArea(GRAMx1, GRAMx2, GRAMy1, GRAMy2, -1);
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
void gfx4desp32_spi_panel::pushColors(uint16_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;

    uint32_t pc = 0;
    uint16_t tcol;

    if (bufferToggle == 0) {
        if (!lvglBufferInit) {
            if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
                psRAMlvglBuffer = (uint16_t*)ps_malloc(__fbSize);
            }
            else {
                fb35 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
            }
            lvglBufferInit = true;
        }
        int w, h;
        w = GRAMx2 - GRAMx1 + 1;
        h = GRAMy2 - GRAMy1 + 1;
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            memcpy(psRAMlvglBuffer, color_data, (w * h) << 1);
            esp_lcd_panel_draw_bitmap(panel_handle, GRAMx1, GRAMy1, GRAMx2 + 1,
                GRAMy2 + 1, psRAMlvglBuffer);
        }
        else {
            uint32_t pixSize = w * h;
            for (uint32_t n = 0; n < pixSize; n++) {
                tcol = color_data[n];
                fb35[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            uint8_t command[10];
            command[0] = (GRAMx1 >> 8) & 0xff;
            command[1] = GRAMx1 & 0xff;
            command[2] = (GRAMx2 >> 8) & 0xff;
            command[3] = GRAMx2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
            command[0] = (GRAMy1 >> 8) & 0xff;
            command[1] = GRAMy1 & 0xff;
            command[2] = (GRAMy2 >> 8) & 0xff;
            command[3] = GRAMy2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
            esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                pixSize * 3);
        }
        bufferToggle = 1;
    }
    else {
        if (!lvglBufferInit2) {
            if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
                psRAMlvglBuffer2 = (uint16_t*)ps_malloc(__fbSize);
            }
            else {
                fb352 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
            }
            lvglBufferInit2 = true;
        }
        int w, h;
        w = GRAMx2 - GRAMx1 + 1;
        h = GRAMy2 - GRAMy1 + 1;
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            memcpy(psRAMlvglBuffer2, color_data, (w * h) << 1);
            esp_lcd_panel_draw_bitmap(panel_handle, GRAMx1, GRAMy1, GRAMx2 + 1,
                GRAMy2 + 1, psRAMlvglBuffer2);
        }
        else {
            uint32_t pixSize = w * h;
            for (uint32_t n = 0; n < pixSize; n++) {
                tcol = color_data[n];
                fb352[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb352[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb352[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            uint8_t command[10];
            command[0] = (GRAMx1 >> 8) & 0xff;
            command[1] = GRAMx1 & 0xff;
            command[2] = (GRAMx2 >> 8) & 0xff;
            command[3] = GRAMx2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
            command[0] = (GRAMy1 >> 8) & 0xff;
            command[1] = GRAMy1 & 0xff;
            command[2] = (GRAMy2 >> 8) & 0xff;
            command[3] = GRAMy2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
            esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb352,
                pixSize * 3);
        }
        bufferToggle = 0;
    }
    wrGRAM = false;
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
void gfx4desp32_spi_panel::pushColors(uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint16_t tcol;

    if (bufferToggle == 0) {
        if (!lvglBufferInit) {
            if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
                psRAMlvglBuffer = (uint16_t*)ps_malloc(__fbSize);
            }
            else {
                fb35 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
            }
            lvglBufferInit = true;
        }
        int w, h;
        w = GRAMx2 - GRAMx1 + 1;
        h = GRAMy2 - GRAMy1 + 1;
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            memcpy(psRAMlvglBuffer, color_data, (w * h) << 1);
            esp_lcd_panel_draw_bitmap(panel_handle, GRAMx1, GRAMy1, GRAMx2 + 1,
                GRAMy2 + 1, psRAMlvglBuffer);
        }
        else {
            uint32_t pixSize = w * h;
            for (uint32_t n = 0; n < pixSize << 1; n += 2) {
                tcol = (color_data[n] << 8) + color_data[n + 1];
                fb35[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            uint8_t command[10];
            command[0] = (GRAMx1 >> 8) & 0xff;
            command[1] = GRAMx1 & 0xff;
            command[2] = (GRAMx2 >> 8) & 0xff;
            command[3] = GRAMx2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
            command[0] = (GRAMy1 >> 8) & 0xff;
            command[1] = GRAMy1 & 0xff;
            command[2] = (GRAMy2 >> 8) & 0xff;
            command[3] = GRAMy2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
            if (pixSize > 76800) {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                    230400);
                esp_lcd_panel_io_tx_color(io_handle, -1, fb35 + 230400,
                    (pixSize * 3) - 230400);
            }
            else {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                    pixSize * 3);
            }
        }
        bufferToggle = 1;
    }
    else {
        if (!lvglBufferInit2) {
            if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
                psRAMlvglBuffer2 = (uint16_t*)ps_malloc(__fbSize);
            }
            else {
                fb352 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
            }
            lvglBufferInit2 = true;
        }
        int w, h;
        w = GRAMx2 - GRAMx1 + 1;
        h = GRAMy2 - GRAMy1 + 1;
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            memcpy(psRAMlvglBuffer2, color_data, (w * h) << 1);
            esp_lcd_panel_draw_bitmap(panel_handle, GRAMx1, GRAMy1, GRAMx2 + 1,
                GRAMy2 + 1, psRAMlvglBuffer2);
        }
        else {
            uint32_t pixSize = w * h;
            for (uint32_t n = 0; n < pixSize << 1; n += 2) {
                tcol = (color_data[n] << 8) + color_data[n + 1];
                fb352[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb352[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb352[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            uint8_t command[10];
            command[0] = (GRAMx1 >> 8) & 0xff;
            command[1] = GRAMx1 & 0xff;
            command[2] = (GRAMx2 >> 8) & 0xff;
            command[3] = GRAMx2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
            command[0] = (GRAMy1 >> 8) & 0xff;
            command[1] = GRAMy1 & 0xff;
            command[2] = (GRAMy2 >> 8) & 0xff;
            command[3] = GRAMy2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
            if (pixSize > 76800) {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb352,
                    230400);
                esp_lcd_panel_io_tx_color(io_handle, -1, fb352 + 230400,
                    (pixSize * 3) - 230400);
            }
            else {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb352,
                    pixSize * 3);
            }
        }
        bufferToggle = 0;
    }
    wrGRAM = false;
}

/****************************************************************************/
/*!
  @brief  Write flash array of pixels to selected frame buffer
  @param  *color_data - 8 bit colour buffer
  @param  length of buffer in pixels
  @note   Write to previously set GRAM window from byte array.
          Fastest way to write GRAM. Not to be used for GCI or gfx4desp32
  functions. Used for LVGL sketches as no clipping or out of bounds supported.
          LANDSCAPE mode uses memcpy for frame buffer write.
          Flush occurs after GRAM window has been fully written to.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::pushColors(const uint8_t* color_data, uint32_t len) {
    if (!(wrGRAM))
        return;
    uint32_t pc = 0;
    uint16_t tcol;

    if (bufferToggle == 0) {
        if (!lvglBufferInit) {
            if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
                psRAMlvglBuffer = (uint16_t*)ps_malloc(__fbSize);
            }
            else {
                fb35 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
            }
            lvglBufferInit = true;
        }
        int w, h;
        w = GRAMx2 - GRAMx1 + 1;
        h = GRAMy2 - GRAMy1 + 1;
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            memcpy(psRAMlvglBuffer, color_data, (w * h) << 1);
            esp_lcd_panel_draw_bitmap(panel_handle, GRAMx1, GRAMy1, GRAMx2 + 1,
                GRAMy2 + 1, psRAMlvglBuffer);
        }
        else {
            uint32_t pixSize = w * h;
            for (uint32_t n = 0; n < pixSize << 1; n += 2) {
                tcol = (color_data[n] << 8) + color_data[n + 1];
                fb35[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb35[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            uint8_t command[10];
            command[0] = (GRAMx1 >> 8) & 0xff;
            command[1] = GRAMx1 & 0xff;
            command[2] = (GRAMx2 >> 8) & 0xff;
            command[3] = GRAMx2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
            command[0] = (GRAMy1 >> 8) & 0xff;
            command[1] = GRAMy1 & 0xff;
            command[2] = (GRAMy2 >> 8) & 0xff;
            command[3] = GRAMy2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
            if (pixSize > 76800) {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                    230400);
                esp_lcd_panel_io_tx_color(io_handle, -1, fb35 + 230400,
                    (pixSize * 3) - 230400);
            }
            else {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb35,
                    pixSize * 3);
            }
        }
        bufferToggle = 1;
    }
    else {
        if (!lvglBufferInit2) {
            if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
                psRAMlvglBuffer2 = (uint16_t*)ps_malloc(__fbSize);
            }
            else {
                fb352 = (uint8_t*)ps_malloc(__fbSize + (__fbSize >> 1));
            }
            lvglBufferInit2 = true;
        }
        int w, h;
        w = GRAMx2 - GRAMx1 + 1;
        h = GRAMy2 - GRAMy1 + 1;
        if (DisplayModel != GFX4d_DISPLAY_ILI9488) {
            memcpy(psRAMlvglBuffer2, color_data, (w * h) << 1);
            esp_lcd_panel_draw_bitmap(panel_handle, GRAMx1, GRAMy1, GRAMx2 + 1,
                GRAMy2 + 1, psRAMlvglBuffer2);
        }
        else {
            uint32_t pixSize = w * h;
            for (uint32_t n = 0; n < pixSize << 1; n += 2) {
                tcol = (color_data[n] << 8) + color_data[n + 1];
                fb352[pc++] = (uint8_t)(tcol >> 8) & 0xfc;
                fb352[pc++] = (uint8_t)(tcol >> 3) & 0xfc;
                fb352[pc++] = (uint8_t)(tcol << 3) & 0xfc;
            }
            uint8_t command[10];
            command[0] = (GRAMx1 >> 8) & 0xff;
            command[1] = GRAMx1 & 0xff;
            command[2] = (GRAMx2 >> 8) & 0xff;
            command[3] = GRAMx2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2a, command, 4);
            command[0] = (GRAMy1 >> 8) & 0xff;
            command[1] = GRAMy1 & 0xff;
            command[2] = (GRAMy2 >> 8) & 0xff;
            command[3] = GRAMy2 & 0xff;
            esp_lcd_panel_io_tx_param(io_handle, 0x2b, command, 4);
            if (pixSize > 76800) {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb352,
                    230400);
                esp_lcd_panel_io_tx_color(io_handle, -1, fb352 + 230400,
                    (pixSize * 3) - 230400);
            }
            else {
                esp_lcd_panel_io_tx_color(io_handle, ILI9488_CMD_MEMORY_WRITE, fb352,
                    pixSize * 3);
            }
        }
        bufferToggle = 0;
    }
    wrGRAM = false;
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
void gfx4desp32_spi_panel::WrGRAM(uint16_t color) {
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
    uint8_t colM, colL;
    pto = tpto + (GRAMypos * __scrWidth) + (GRAMxpos << 1);
    if (GRAMxpos >= clipX1 && GRAMxpos <= clipX2 && GRAMypos >= clipY1 &&
        GRAMypos <= clipY2) {
        colM = color >> 8;
        colL = color & 0xff;
        if (alpha) {
            calcAlpha(colL + (colM << 8), pto[1] | (pto[0] << 8), __alpha);
            pto[1] = __colour;
            pto[0] = __colour >> 8;
        }
        else {
            pto[0] = colM;
            pto[1] = colL;
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
    if (pixelPos >= pixelCount) { // if GRAM area is written to flush the area.
        wrGRAM = false;
        if (frame_buffer == 0)
            FlushArea(GRAMx1, GRAMx2, GRAMy1, GRAMy2, -1);
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
uint16_t gfx4desp32_spi_panel::ReadPixel(uint16_t xrp, uint16_t yrp) {
    if (yrp > clipY2 || yrp < clipY1)
        return 0;
    if (xrp > clipX2 || yrp < clipX1)
        return 0;
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    pto = tpto + (yrp * __scrWidth) + (xrp << 1);
    return pto[1] + (pto[0] << 8);
}

uint16_t gfx4desp32_spi_panel::ReadPixelFromFrameBuffer(uint16_t xrp, uint16_t yrp, uint8_t fb) {
    if (yrp > clipY2 || yrp < clipY1)
        return 0;
    if (xrp > clipX2 || yrp < clipX1)
        return 0;
    uint8_t* tpto = SelectFB(fb);
    uint8_t* pto;
    pto = tpto + (yrp * __scrWidth) + (xrp << 1);
    return pto[1] + (pto[0] << 8);
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
uint16_t gfx4desp32_spi_panel::ReadLine(int16_t x, int16_t y, int16_t w,
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
    pto = tpto + (y * __scrWidth) + (x << 1);
    while (w--) {
        data[pc++] = pto[0] + (pto[1] << 8);
        pto += 2;
    }
    return readw;
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
void gfx4desp32_spi_panel::CopyFrameBufferLine(int16_t x, int16_t y, int16_t w,
    int fb) {
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
    uint8_t* tpfrom = SelectFB(fb);
    uint8_t* pfrom;
    int flushw = w;
    SetGRAM(x, y, x + w - 1, y);
    pfrom = tpfrom + (y * __scrWidth) + (x << 1);
    WrGRAMs(pfrom, w);
    if (frame_buffer == 0)
        FlushArea(x, x + w, y, y, -1);
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
void gfx4desp32_spi_panel::WriteLine(int16_t x, int16_t y, int16_t w,
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
    int pc = 0;
    pto = tpto + (y * __scrWidth) + (x << 1);
    while (w--) {
        pto[0] = data[pc];
        pto[1] = data[pc] >> 8;
        pto += 2;
        pc++;
    }
    if (frame_buffer == 0)
        FlushArea(x, x + w, y, y, -1);
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
void gfx4desp32_spi_panel::ClipWindow(int x1, int y1, int x2, int y2) {
    clipX1pos = x1;
    clipY1pos = y1;
    clipX2pos = x2;
    clipY2pos = y2; // need to add check for out of bounds
}

/****************************************************************************/
/*!
  @brief  Enable / disable Clipping region
  @param  clipping - true / false
  @note   clipping is enabled by changing from defalult to user position and
          disabled by reverting back to default.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::Clipping(bool clipping) {
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
            clipX2 = (int)h_res - 1;
            clipY2 = (int)v_res - 1;
        }
        else {
            clipX1 = 0;
            clipY1 = 0;
            clipX2 = (int)v_res - 1;
            clipY2 = (int)h_res - 1;
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
void gfx4desp32_spi_panel::setScrollArea(int x1, int y1, int x2, int y2) {
    scroll_X1 = x1;
    scroll_Y1 = y1;
    scroll_X2 = x2;
    scroll_Y2 = y2;
    if (scroll_X1 < 0)
        scroll_X1 = 0;
    if (scroll_X2 >= __width)
        scroll_X2 = __width - 1;
    if (scroll_Y1 < 0)
        scroll_Y1 = 0;
    if (scroll_Y2 >= __height)
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
void gfx4desp32_spi_panel::setScrollArea(int y1, int y2) {
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
void gfx4desp32_spi_panel::_ScrollEnable(bool scrEn) { scroll_Enable = scrEn; }

/****************************************************************************/
/*!
  @brief  Set direction of scroll
  @param  scrEn - scroll_Direction 0 - 3
  @note   default direction is upwards
*/
/****************************************************************************/
void gfx4desp32_spi_panel::setScrollDirection(uint8_t scrDir) {
    scroll_Direction = scrDir % 4;
}

/****************************************************************************/
/*!
  @brief  Set blanking line colour after scroll has moved
  @param  scolor - RGB565 colour
  @note   this maybe could be the current text background colour
*/
/****************************************************************************/
void gfx4desp32_spi_panel::setScrollBlankingColor(int32_t scolor) {
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
void gfx4desp32_spi_panel::SmoothScrollSpeed(uint8_t sspeed) {
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
void gfx4desp32_spi_panel::Scroll(int steps) {
    uint32_t x_start;
    uint32_t y_start;
    uint32_t x_end;
    uint32_t y_end;
    uint8_t scrdir;
    Clipping(false);

    x_start = scroll_X1;
    y_start = scroll_Y1;
    x_end = scroll_X2;
    y_end = scroll_Y2;
    if (x_start >= __width || x_end < 0 || y_start >= __height || y_end < 0) return;
    uint32_t s_width = x_end - x_start + 1;
    uint32_t s_height = y_end - y_start + 1;
    scrdir =
        scroll_Directions[(0 << 2) +
        scroll_Direction]; // calculate direction using
    // orientation and direction array
    int n, o;
    uint8_t* tfbbuf = SelectFB(frame_buffer);
    uint8_t* from; // create pointer for from area
    uint8_t* to;   // create pointer for to area
    int16_t s_steps = steps;
    uint32_t inc;
    uint32_t s_inc;
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
            from = tfbbuf + ((y_start + inc) * __scrWidth) + (x_start << 1);
            to = tfbbuf + (y_start * __scrWidth) + (x_start << 1);
            n = s_height;
            while (n--) {
                if (n > inc - 1) {
                    memmove(to, from, s_width << 1);
                    from += __scrWidth;
                    to += __scrWidth;
                }
                else {
                    o = s_width;
                    while (o--) {
                        if (scroll_blanking != -1) {
                            to[1] = scroll_blanking;
                            to[0] = scroll_blanking >> 8;
                        }
                        to += 2;
                    }
                    to += (__scrWidth - (s_width << 1));
                }
            }
            FlushArea(scroll_X1, scroll_X2, scroll_Y1, scroll_Y2, -1);
            //FlushArea(y_start, y_end, -1);
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
                            to[1] = scroll_blanking;
                            to[0] = scroll_blanking >> 8;
                        }
                        to += 2;
                    }
                    to -= (__scrWidth + (s_width << 1));
                }
            }
            FlushArea(scroll_X1, scroll_X2, scroll_Y1, scroll_Y2, -1);
            //FlushArea(y_start, y_end, -1);
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
                        from[pc++] = scroll_blanking >> 8;
                        from[pc++] = scroll_blanking;
                    }
                }
                from += __scrWidth;
                to += __scrWidth;
            }
            FlushArea(scroll_X1, scroll_X2, scroll_Y1, scroll_Y2, -1);
            //FlushArea(y_start, y_end, -1);
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
                        to[pc++] = scroll_blanking >> 8;
                        to[pc++] = scroll_blanking;
                    }
                }
                from += __scrWidth;
                to += __scrWidth;
            }
            FlushArea(scroll_X1, scroll_X2, scroll_Y1, scroll_Y2, -1);
            //FlushArea(y_start, y_end, -1);
            delay(scroll_speed);
        }
    }

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
void gfx4desp32_spi_panel::RectangleFilled(int x1, int y1, int x2, int y2,
    uint16_t color) {
    if (transparency) {
        if (color == _transparentColor)
            return;
    }
    if (x1 > x2)
        swap(x1, x2);
    if (y1 > y2)
        swap(y1, y2);
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;

    int32_t xpos = x1;
    int32_t ypos = y1;
    pPos = (ypos * __scrWidth);
    uint8_t* tpto = SelectFB(frame_buffer);
    uint8_t* pto;
    uint32_t pixels = ((x2 - x1) + 1) * ((y2 - y1) + 1);
    pto = tpto + pPos + (xpos << 1);
    while (pixels--) {
        if (xpos >= clipX1 && xpos <= clipX2 && ypos >= clipY1 && ypos <= clipY2) {
            if (!alpha) {
                pto[1] = color;
                pto[0] = color >> 8;
            }
            else {
                calcAlpha(color, pto[1] | (pto[0] << 8), __alpha);
                pto[1] = __colour;
                pto[0] = __colour >> 8;
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
        FlushArea(x1, x2, y1, y2, -1);
}

/****************************************************************************/
/*!
  @brief  Select a frame buffer for all drawing actions.
  @param  sel 0 to 3
  @note   frame buffers are pre defined as empty and only sized to screen
  dimensions when the DrawToFramebuffer function is called.
*/
/****************************************************************************/
uint8_t* gfx4desp32_spi_panel::SelectFB(uint8_t sel) {
    switch (sel) {
    case 0:
        return fb;
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
    return fb;
}

void gfx4desp32_spi_panel::AllocateDRcache(uint32_t cacheSize) {
    psRAMbuffer2 = (uint8_t*)ps_malloc(cacheSize);
    cache_Enabled = true;
}

void gfx4desp32_spi_panel::AllocateFB(uint8_t sel) {
    if (sel == 0) {
        psRAMbuffer1 = (uint8_t*)ps_malloc(1024000);
    }
    if (sel == 1) {
        psRAMbuffer3 = (uint8_t*)ps_malloc(__fbSize);
        framebufferInit1 = true;
    }
    if (sel == 2) {
        psRAMbuffer4 = (uint8_t*)ps_malloc(__fbSize);
        framebufferInit2 = true;
    }
    if (sel == 3) {
        psRAMbuffer5 = (uint8_t*)ps_malloc(__fbSize);
        framebufferInit3 = true;
    }
    if (sel == 4) {
        psRAMbuffer6 = (uint8_t*)ps_malloc(__fbSize);
        framebufferInit4 = true;
    }
}

void gfx4desp32_spi_panel::CopyFrameBuffer(uint8_t fbto, uint8_t fbfrom1) {
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
void gfx4desp32_spi_panel::MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1,
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
void gfx4desp32_spi_panel::MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1,
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
void gfx4desp32_spi_panel::PutPixel(int16_t x, int16_t y, uint16_t color) {
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
    pto = tpto + (y * __scrWidth) + (x << 1);
    if (!alpha) {
        pto[1] = color;
        pto[0] = color >> 8;
    }
    else {
        calcAlpha(color, pto[1] | (pto[0] << 8), __alpha);
        pto[1] = __colour;
        pto[0] = __colour >> 8;
    }

    if (frame_buffer == 0)
        FlushArea(x, x, y, y, -1);
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
void gfx4desp32_spi_panel::Hline(int16_t x, int16_t y, int16_t w,
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
        w *= -1;
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
    pto = tpto + (y * __scrWidth) + (x << 1);
    while (w--) {
        if (!alpha) {
            pto[1] = hcolor;
            pto[0] = hcolor >> 8;
        }
        else {
            calcAlpha(hcolor, pto[1] | (pto[0] << 8), __alpha);
            pto[1] = __colour;
            pto[0] = __colour >> 8;
        }

        pto += 2;
    }
    if (frame_buffer == 0)
        FlushArea(x, x + flushw - 1, y, y, -1);
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
void gfx4desp32_spi_panel::Vline(int16_t x, int16_t y, int16_t w,
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
    pto = tpto + (y * __scrWidth) + (x << 1);
    while (w--) {
        if (!alpha) {
            pto[1] = vcolor;
            pto[0] = vcolor >> 8;
        }
        else {
            calcAlpha(vcolor, pto[1] | (pto[0] << 8), __alpha);
            pto[1] = __colour;
            pto[0] = __colour >> 8;
        }

        pto += __scrWidth;
    }
    if (frame_buffer == 0)
        FlushArea(x, x, y, y + flushw - 1, -1);
}

/****************************************************************************/
/*!
  @brief  Turn on Transparent mode
  @param  trans - transparency on / off
*/
/****************************************************************************/
void gfx4desp32_spi_panel::Transparency(bool trans) {
    transparency = trans;
    transparency = trans;
}

/****************************************************************************/
/*!
  @brief  Set transparent colour
  @param  color - RGB565 colour that won't be drawn in all functions when
          transparency enabled.
  @note   does not operate in pushColors function.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::TransparentColor(uint16_t color) {
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
void gfx4desp32_spi_panel::AlphaBlend(bool alphablend) {
    alpha = alphablend;
    alpha = alphablend;
    transalpha = alpha | transparency;
    transalpha = transalpha;
}

/****************************************************************************/
/*!
  @brief  Set Alpha Blend Level
  @param  alphaLevel - 0 to 255
  @note   does not operate in pushColors function.
*/
/****************************************************************************/
void gfx4desp32_spi_panel::AlphaBlendLevel(uint32_t alphaLevel) {
    __alpha = alphaLevel;
}

void gfx4desp32_spi_panel::drawBitmap(int x1, int y1, int x2, int y2,
    uint16_t* c_data) {
    draw_bitmap(x1, y1, x2, y2, c_data);
}

void gfx4desp32_spi_panel::draw_bitmap(int x1, int y1, int x2, int y2,
    uint16_t* c_data) {
    uint8_t command[10];
    command[0] = (x1 >> 8) & 0xFF;
    command[1] = x1 & 0xFF;
    command[2] = ((x2 - 1) >> 8) & 0xFF;
    command[3] = (x2 - 1) & 0xFF;
    tx_param(ILI9488_CMD_COLUMN_ADDRESS_SET, command, 4);
    command[0] = (y1 >> 8) & 0xFF;
    command[1] = y1 & 0xFF;
    command[2] = ((y2 - 1) >> 8) & 0xFF;
    command[3] = (y2 - 1) & 0xFF;
    tx_param(ILI9488_CMD_PAGE_ADDRESS_SET, command, 4);
    // transfer frame buffer
    size_t len = (x2 - x1) * (y2 - y1) * 2;
    tx_color(ILI9488_CMD_MEMORY_WRITE, c_data, len);
}

void gfx4desp32_spi_panel::PinMode(byte pin, uint8_t mode) { pinMode(pin, mode); }

void gfx4desp32_spi_panel::DigitalWrite(byte pin, bool state) {
    digitalWrite(pin, state);
}

int gfx4desp32_spi_panel::DigitalRead(byte pin) { return digitalRead(pin); }
