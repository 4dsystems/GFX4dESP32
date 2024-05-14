#include "gfx4desp32_rgb_panel_t.h"

#include <Preferences.h>

Preferences preferences;

gfx4desp32_rgb_panel_t::gfx4desp32_rgb_panel_t(
    esp_lcd_rgb_panel_config_t* panel_config, int bk_pin, int bk_on_level,
    int bk_off_level, int sd_gpio_sck, int sd_gpio_miso, int sd_gpio_mosi,
    int sd_gpio_cs, bool touchYinvert)
    : gfx4desp32_rgb_panel(panel_config, bk_pin, bk_on_level, bk_off_level,
        sd_gpio_sck, sd_gpio_miso, sd_gpio_mosi, sd_gpio_cs,
        touchYinvert),
    gfx4desp32_touch() {
    touchYswap = touchYinvert;
}

gfx4desp32_rgb_panel_t::~gfx4desp32_rgb_panel_t() {}

/****************************************************************************/
/*!
  @brief  Enable / disable touch functions
  @param  mode - TOUCH_ENABLE / TOUCH_DISABLE
  @note   experimental
*/
/****************************************************************************/
void gfx4desp32_rgb_panel_t::touch_Set(uint8_t mode) {
    delay(100);
    if (mode == TOUCH_ENABLE) {
        _TouchEnable = true;
        if (touchFirstEnable) {
            touchFirstEnable = false;
        }
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
bool gfx4desp32_rgb_panel_t::touch_Update() {
    bool intStatus = DigitalRead(GFX4d_TOUCH_INT);
    // *** if touch is not enabled and no touch int or touch status is not no touch
    // then just return ***
    // *** need to create touch release state before no touch ***
    if (!_TouchEnable || (intStatus && tPen == 0))
        return false;
    bool update = false;
    bool validTouch;

    int i = 0;
    int tps;
    uint8_t gTPData[10];
    uint8_t bytesReceived;

    // *** process capacitive touch ***
    Wire.beginTransmission(0x38);
    Wire.write((byte)2);
    Wire.endTransmission();
    bytesReceived = Wire.requestFrom(0x38, 6);
    Wire.readBytes(gTPData, bytesReceived);
    Wire.endTransmission();

    tps = gTPData[0];
    if (tps != 0xff) { // some FT chips return garbage before first valid read,
        // try and detect that
        tPen = tps;
        if (tPen == 1) {
            switch (rotation) {
            case LANDSCAPE:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                if (touchYswap) {
                    touchYpos =
                        __height -
                        (((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6]) - 1;
                }
                else {
                    touchYpos = ((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6];
                }
                touchXpos = ((gTPData[3 + i * 6] & 0x0f) << 8) + gTPData[4 + i * 6];
                break;
            case LANDSCAPE_R:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                if (touchYswap) {
                    touchYpos = ((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6];
                }
                else {
                    touchYpos =
                        __height -
                        (((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6]) - 1;
                }
                touchXpos = __width -
                    (((gTPData[3 + i * 6] & 0x0f) << 8) + gTPData[4 + i * 6]) -
                    1;
                break;
            case PORTRAIT_R:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                if (touchYswap) {
                    touchXpos = __width - (((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6]) - 1;
                }
                else {
                    touchXpos = ((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6];
                }
                touchYpos = __height -
                    (((gTPData[3 + i * 6] & 0x0f) << 8) + gTPData[4 + i * 6]) -
                    1;
                break;
            case PORTRAIT:
                lasttouchYpos = touchYpos;
                lasttouchXpos = touchXpos;
                if (touchYswap) {
                    touchXpos = ((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6];
                }
                else {
                    touchXpos = __width -
                        (((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6]) -
                        1;
                }
                //touchXpos = ((gTPData[1 + i * 6] & 0x0f) << 8) + gTPData[2 + i * 6];
                touchYpos = ((gTPData[3 + i * 6] & 0x0f) << 8) + gTPData[4 + i * 6];
                break;
            }
        }
        validTouch = true;
    }

    if (validTouch) {
        if (tPen == 1 && (lasttouchXpos != touchXpos || lasttouchYpos != touchYpos))
            update = true;
        if (tPen != lasttPen) {
            update = true;
            if (lasttPen == 1 && tPen == 0)
                tPen = 2; // *** create touch release state ***
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