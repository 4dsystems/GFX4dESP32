#include "gfx4desp32_qspi_panel_t.h"

gfx4desp32_qspi_panel_t::gfx4desp32_qspi_panel_t(
    int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI, int panel_Pin_MISO,
    int panel_Pin_CLK, int panel_pin_QSPI, int panel_Pin_RST, int bk_pin, int bk_on_level,
    int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO, int sd_gpio_MOSI,
    int sd_gpio_CS, int hres, int vres, bool touchXinvert)
    : gfx4desp32_spi_panel(
        panel_Pin_CS, panel_Pin_DC, panel_Pin_MOSI, panel_Pin_MISO,
        panel_Pin_CLK, panel_pin_QSPI, panel_Pin_RST, bk_pin, bk_on_level, bk_off_level,
        sd_gpio_SCK, sd_gpio_MISO, sd_gpio_MOSI, sd_gpio_CS, hres, vres, touchXinvert),
    gfx4desp32_touch() {
    touchXswap = touchXinvert;
}
gfx4desp32_qspi_panel_t::~gfx4desp32_qspi_panel_t() {}

/****************************************************************************/
/*!
  @brief  Enable / disable touch functions
  @param  mode - TOUCH_ENABLE / TOUCH_DISABLE
  @note   experimental
*/
/****************************************************************************/
void gfx4desp32_qspi_panel_t::touch_Set(uint8_t mode) {
	pinMode(41, OUTPUT);
    pinMode(42, INPUT);
    digitalWrite(41, LOW);
    delay(100);
    digitalWrite(41, HIGH);
    if (mode == TOUCH_ENABLE) {
        if (I2CInit == false) {
            if (Wire.begin(17, 18, 400000)) {
                I2CInit = true;
            }
            else {
            }
        }
        _TouchEnable = true;
    }
    else {
        _TouchEnable = false;
    }
}

/****************************************************************************/
/*!
  @brief  Update touch controller
  @note   if a touch event has occurred pen, xpos, ypos and images touched
          will be updated.
*/
/****************************************************************************/
bool gfx4desp32_qspi_panel_t::touch_Update() {
    if (!_TouchEnable || digitalRead(42))
        return false;
    bool update = false;
    //int n = -1;
    Wire.beginTransmission(0x38);
    Wire.write((byte)2);
    Wire.endTransmission();
    uint8_t bytesReceived = Wire.requestFrom(0x38, 6);
    uint8_t gCTPData[bytesReceived];
    Wire.readBytes(gCTPData, bytesReceived);
    int i = 0; int tps = 0; //int tid = 0;
    tps = gCTPData[0];
    if (tps != 0xff) { // some FT chips return garbage before first valid read,
        // try and detect that
        tPen = tps;
        if (tPen == 1) {
            switch (rotation) {
            case LANDSCAPE:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                touchYpos = ((gCTPData[1 + i * 6] & 0x0f) << 8) + gCTPData[2 + i * 6];
                if (touchXswap) {
                    touchXpos =
                        __width -
                        (((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6]) - 1;
                }
                else {
                    touchXpos = ((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6];
                }
                break;
            case LANDSCAPE_R:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                touchYpos =
                    __height -
                    (((gCTPData[1 + i * 6] & 0x0f) << 8) + gCTPData[2 + i * 6]) - 1;
                if (touchXswap) {
                    touchXpos = ((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6];
                }
                else {
                    touchXpos =
                        __width -
                        (((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6]) - 1;
                }
                break;
            case PORTRAIT_R:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                touchXpos = ((gCTPData[1 + i * 6] & 0x0f) << 8) + gCTPData[2 + i * 6];
                if (touchXswap) {
                    touchYpos = ((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6];
                }
                else {
                    touchYpos =
                        __height -
                        (((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6]) - 1;
                }
                break;
            case PORTRAIT:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                touchXpos =
                    __width -
                    (((gCTPData[1 + i * 6] & 0x0f) << 8) + gCTPData[2 + i * 6]) - 1;
                if (touchXswap) {
                    touchYpos =
                        __height -
                        (((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6]) - 1;
                }
                else {
                    touchYpos = ((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6];
                }
                break;
            }
        }
        if (tPen == 1 && (lasttouchXpos != touchXpos || lasttouchYpos != touchYpos))
            update = true;
        if (tPen != lasttPen) {
            update = true;
            if (lasttPen == 1 && tPen == 0)
                tPen = 2;
            lasttPen = tPen;
            if (tPen == 1) {
                gciobjtouched = -1;
                if (gciobjnum > 0) {
                    if (opgfx) {
                        for (int n = 0; n < gciobjnum; n++) {
                            if ((gciobjtouchenable[n] & 0x01) == 1) {
                                if (touchXpos >= tuix[n] &&
                                    touchXpos <= (tuix[n] + tuiw[n] - 1) &&
                                    touchYpos >= tuiy[n] &&
                                    touchYpos <= (tuiy[n] + tuih[n] - 1)) {
                                    gciobjtouched = n;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return update;
}
