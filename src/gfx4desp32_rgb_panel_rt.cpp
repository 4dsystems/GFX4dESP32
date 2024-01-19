#include "gfx4desp32_rgb_panel_rt.h"

#define READS           10
#define READSPLUS       READS + 10
#define PRESS_TRIP      60000

Preferences preferences;

gfx4desp32_rgb_panel_rt::gfx4desp32_rgb_panel_rt(
    esp_lcd_rgb_panel_config_t* panel_config, int bk_pin, int bk_on_level,
    int bk_off_level, int sd_gpio_sck, int sd_gpio_miso, int sd_gpio_mosi,
    int sd_gpio_cs, bool touchYinvert, uint8_t tType)
    : gfx4desp32_rgb_panel_t(panel_config, bk_pin, bk_on_level, bk_off_level,
        sd_gpio_sck, sd_gpio_miso, sd_gpio_mosi,
        sd_gpio_cs, touchYinvert, tType) {}

gfx4desp32_rgb_panel_rt::~gfx4desp32_rgb_panel_rt() {}

/****************************************************************************/
/*!
  @brief  Update touch controller
  @note   if a touch event has occurred pen, xpos, ypos and images touched
          will be updated.
*/
/****************************************************************************/
bool gfx4desp32_rgb_panel_rt::touch_Update() {
    bool intStatus = DigitalRead(GFX4d_TOUCH_INT);
    // *** if touch is not enable and no touch int or touch status is not no touch
    // then just return ***
    // *** need to create touch release state before no touch ***
    if (!_TouchEnable || (intStatus && tPen == 0))
        return false;
    bool update = false;
    bool validTouch;
    int x, y;
    int i = 0;
    uint8_t gTPData[10];
    uint8_t bytesReceived;

    int m = 0;

    // *** process resistive touch ***
    int txpos = 0; int typos = 0; int z1 = 0; int z2 = 0;
    // txpos1 = 0; // seems to be unused, only being set to 0, nothing else
    if (tPen != 2) {
        int xl[READS], yl[READS];

        long p = 0;
        int mnx = 0; int mny = 0; int mnxi = 0; int mnyi = 0; int mxx = 0; int mxy = 0; int mxxi = 0; int mxyi = 0;

        m = 0;

        // read 10 values
        for (i = 0; i < READSPLUS; i++) {
            // read x
            Wire.beginTransmission(0x48);
            Wire.write((byte)0xc4);
            Wire.endTransmission();
            bytesReceived = Wire.requestFrom(0x48, 2);
            Wire.readBytes(gTPData + 2, bytesReceived);
            x = (gTPData[2] << 4) | (gTPData[3] >> 4);
            Wire.endTransmission();
            // *** Get touch pressure z1 sample ***
            Wire.beginTransmission(0x48);
            Wire.write((byte)0xe4);
            Wire.endTransmission();
            bytesReceived = Wire.requestFrom(0x48, 2);
            Wire.readBytes(gTPData, bytesReceived);
            z1 = (gTPData[0] << 4) | (gTPData[1] >> 4);
            Wire.endTransmission();
            // *** Get touch pressure z2 sample ***
            Wire.beginTransmission(0x48);
            Wire.write((byte)0xf4);
            Wire.endTransmission();
            bytesReceived = Wire.requestFrom(0x48, 2);
            Wire.readBytes(gTPData + 8, bytesReceived);
            z2 = (gTPData[8] << 4) | (gTPData[9] >> 4);
            Wire.endTransmission();
            // *** Get touch y sample ***
            Wire.beginTransmission(0x48);
            Wire.write((byte)0xd4);
            Wire.endTransmission();
            bytesReceived = Wire.requestFrom(0x48, 2);
            Wire.readBytes(gTPData + 6, bytesReceived);
            y = (gTPData[6] << 4) | (gTPData[7] >> 4);
            Wire.endTransmission();
            // calculate
            if ((z1 < 20) || (z2 > 4075))
                p = PRESS_TRIP + 1;
            else
                p = (long)x * ((long)z2 / (long)z1 - 1l);
            if (p <= PRESS_TRIP) // keep correct pressure values
            {
                xl[m] = x;
                yl[m] = y;
                m++;
            }
            if (m == READS)
                break;
        }
        if (m == READS) // only interested if we got 10 good reads
        {
            mnx = mny = 10000; // real max is 4095
            mxx = mxy = 0;
            txpos = typos = 0;
            for (i = 0; i < READS; i++) // work out min and max
            {
                if (xl[i] < mnx) {
                    mnx = xl[i];
                    mnxi = i;
                }
                if (xl[i] > mxx) {
                    mxx = xl[i];
                    mxxi = i;
                }
                if (yl[i] < mny) {
                    mny = yl[i];
                    mnyi = i;
                }
                if (yl[i] > mxy) {
                    mxy = yl[i];
                    mxyi = i;
                }
            }
            if (mnxi == mxxi)
                mxxi++; // only true if all the same, (will both be 0), need to adjust
            // as we need to ignore two values
            if (mnyi == mxyi)
                mxyi++; // only true if all the same, (will both be 0), need to adjust
            // as we need to ignore two values
            for (i = 0; i < READS; i++) // discard min and max
            {
                if ((i != mnxi) && (i != mxxi))
                    txpos += xl[i];
                if ((i != mnyi) && (i != mxyi))
                    typos += yl[i];
            }
            txpos /= 8;
            typos /= 8;
        }
    }
    if (m == READS) {
        tPen = 1;

        lasttouchYpos = touchYpos;
        lasttouchXpos = touchXpos;
        touchXraw = txpos;
        touchYraw = typos;
        switch (rotation) {
        case LANDSCAPE:
            touchXpos = map(txpos, calx1, calx2, 0, __width);
            touchYpos = map(typos, caly1, caly2, __height, 0);
            break;
        case LANDSCAPE_R:
            touchXpos = map(txpos, calx1, calx2, __width, 0);
            touchYpos = map(typos, caly1, caly2, 0, __height);
            break;
        case PORTRAIT_R:
            touchYpos = map(txpos, calx1, calx2, __height, 0);
            touchXpos = map(typos, caly1, caly2, __width, 0);
            break;
        case PORTRAIT:
            touchYpos = map(txpos, calx1, calx2, 0, __height);
            touchXpos = map(typos, caly1, caly2, 0, __width);
            break;
        }

        validTouch = true;
    }

    else
        tPen = 0;

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

void gfx4desp32_rgb_panel_rt::touchCalibration() {

    int cx1 = calx1;
    int cx2 = calx2;
    int cy1 = caly1;
    int cy2 = caly2;
    int dx1, dx2, dy1, dy2;
    int ncx1, ncx2, ncy1, ncy2;
    int calsteps = 5;
    long tnow;

    int xpos, ypos;
    int calvaluexpos, calvalueypos;
    bool flash = false;
    int p = 0;
    bool showbuttons = false;
    int exitposx = 0; int exitposy = 0; int redoposy = 0;

    panelOrientation(LANDSCAPE);

    while (calsteps) {
        if (calsteps == 5) { // Init Screen
            calx1 = cx1;
            calx2 = cx2;
            caly1 = cy1;
            caly2 = cy2;
            Cls();
            touch_Set(TOUCH_ENABLE);
            int i;
            i = __width / 20;
            xpos = 20;
            ypos = __height / i;
            Circle(xpos + 16, ypos + 16, 16, WHITE);
            Hline(xpos, ypos + 16, 32, WHITE);
            Vline(xpos + 16, ypos, 32, WHITE);
            CircleFilled(xpos + 16, ypos + 16, 4, WHITE);
            TextColor(LIME, BLACK);
            Font(2);
            TextSize(1);
            MoveTo(xpos + 37, ypos + 10);
            print("<<< Touch Centre");
            calvaluexpos = (__width >> 1) - 80;
            calvalueypos = __height / 5;
            MoveTo(calvaluexpos, calvalueypos);
            print("Current Values");
            MoveTo(calvaluexpos, calvalueypos + 16);
            print("TOUCH_XMINCAL=");
            print(calx1, HEX);
            MoveTo(calvaluexpos, calvalueypos + 32);
            print("TOUCH_YMINCAL=");
            print(caly1, HEX);
            MoveTo(calvaluexpos, calvalueypos + 48);
            print("TOUCH_XMAXCAL=");
            print(calx2, HEX);
            MoveTo(calvaluexpos, calvalueypos + 64);
            print("TOUCH_YMAXCAL=");
            print(caly2, HEX);
            calsteps--;
            tnow = millis();
            p = 0;
        }
        if (calsteps == 4) { // cal1 pressed
            if (millis() - tnow > 500) {
                if (p == 0) {
                    if (flash) {
                        MoveTo(xpos + 37, ypos + 10);
                        print("                ");
                        flash = false;
                    }
                    else {
                        MoveTo(xpos + 37, ypos + 10);
                        print("<<< Touch Centre");
                        flash = true;
                    }
                }
                tnow = millis();
            }
            if (touch_Update()) {
                p = touch_GetPen();
                if (p == TOUCH_PRESSED) {
                    if (rotation == LANDSCAPE) {
                        ncx1 = touchXraw;
                        ncy2 = touchYraw;
                    }
                    MoveTo(xpos + 37, ypos + 10);
                    print("Sampling ...    ");
                }
                if (p == TOUCH_RELEASED) {
                    MoveTo(xpos + 37, ypos + 10);
                    print("OK              ");
                    Circle((__width - 1) - xpos - 16, (__height - 1) - ypos - 16, 16,
                        WHITE);
                    Hline((__width - 1) - xpos, (__height - 1) - ypos - 16, -32, WHITE);
                    Vline((__width - 1) - xpos - 16, (__height - 1) - ypos, -32, WHITE);
                    CircleFilled((__width - 1) - xpos - 16, (__height - 1) - ypos - 16, 4,
                        WHITE);
                    MoveTo((__width - 1) - xpos - 177, (__height - 1) - ypos - 23);
                    print("Touch Centre >>>");
                    calsteps--;
                    delay(500);
                    tnow = millis();
                    p = 0;
                }
            }
        }
        if (calsteps == 3) { // cal2 pressed
            if (millis() - tnow > 500) {
                if (p == 0) {
                    if (flash) {
                        MoveTo((__width - 1) - xpos - 177, (__height - 1) - ypos - 23);
                        print("                ");
                        flash = false;
                    }
                    else {
                        MoveTo((__width - 1) - xpos - 177, (__height - 1) - ypos - 23);
                        print("Touch Centre >>>");
                        flash = true;
                    }
                }
                tnow = millis();
            }
            if (touch_Update()) {
                p = touch_GetPen();
                if (p == TOUCH_PRESSED) {
                    if (rotation == LANDSCAPE) {
                        ncx2 = touchXraw;
                        ncy1 = touchYraw;
                    }
                    MoveTo((__width - 1) - xpos - 177, (__height - 1) - ypos - 23);
                    print("    Sampling ...");
                }
                if (p == TOUCH_RELEASED) {
                    MoveTo((__width - 1) - xpos - 177, (__height - 1) - ypos - 23);
                    print("              OK");
                    int rx = __width - ((xpos + 16) << 1);
                    int ry = __height - ((ypos + 16) << 1);
                    float diffx =
                        (((float)ncx2 - (float)ncx1) / (float)rx) * ((float)xpos + 16);
                    float diffy =
                        (((float)ncy2 - (float)ncy1) / (float)ry) * ((float)ypos + 16);
                    ncx1 -= (int)diffx;
                    ncx2 += (int)diffx;
                    ncy1 -= (int)diffy;
                    ncy2 += (int)diffy;
                    MoveTo(calvaluexpos, (calvalueypos * 3));
                    print("New Values");
                    MoveTo(calvaluexpos, (calvalueypos * 3) + 16);
                    print("TOUCH_XMINCAL=");
                    print(ncx1, HEX);
                    MoveTo(calvaluexpos, (calvalueypos * 3) + 32);
                    print("TOUCH_YMINCAL=");
                    print(ncy1, HEX);
                    MoveTo(calvaluexpos, (calvalueypos * 3) + 48);
                    print("TOUCH_XMAXCAL=");
                    print(ncx2, HEX);
                    MoveTo(calvaluexpos, (calvalueypos * 3) + 64);
                    print("TOUCH_YMAXCAL=");
                    print(ncy2, HEX);
                    if (ncx1 < ncx2 && ncy1 < ncy2) {
                        MoveTo(calvaluexpos, (calvalueypos * 3) + 80);
                        TextColor(YELLOW, BLACK);
                        print("ACCEPTED");
                        calx1 = ncx1;
                        calx2 = ncx2;
                        caly1 = ncy1;
                        caly2 = ncy2;
                    }
                    TextColor(CYAN, BLACK);
                    MoveTo(xpos + 70, ypos + 10);
                    print("NOW DRAW TO TEST....");
                    calsteps--;
                    delay(500);
                    tnow = millis();
                    showbuttons = true;
                    p = 0;
                    dx1 = -1;
                    dx2 = -1;
                    dy1 = -1;
                    dy2 = -1;
                }
            }
        }
        if (calsteps == 2) { // test touch
            touch_Update();
            if (touch_GetPen() == TOUCH_PRESSED) {
                dx2 = touch_GetX();
                dy2 = touch_GetY();

                if (dx1 == -1) {
                    dx1 = dx2;
                    dy1 = dy2;
                }
                else {
                    Line(dx1, dy1, dx2, dy2, LIGHTGREEN);
                    dx1 = dx2;
                    dy1 = dy2;
                    if (showbuttons) {
                        showbuttons = false;
                        TextColor(RED, BLACK);
                        exitposx = calvaluexpos + 95;
                        exitposy = calvalueypos + 103;
                        redoposy = (calvalueypos * 3) - 32 + 8;
                        CircleFilled(exitposx, exitposy, 9, RED);
                        Circle(exitposx, exitposy, 9, YELLOW);
                        CircleFilled(exitposx, redoposy, 9, RED);
                        Circle(exitposx, redoposy, 9, YELLOW);
                        flash = true;
                        tnow = millis() - 500;
                    }
                }
            }
            if (touch_GetPen() == TOUCH_RELEASED) {
                if (dx2 >= (exitposx - 10) && dx1 <= (exitposx + 10)) {
                    if (dy2 >= (exitposy - 10) && dy2 <= (exitposy + 10)) {
                        calsteps--;
                    }
                    if (dy2 >= (redoposy - 10) && dy2 <= (redoposy + 10)) {
                        delay(500);
                        calsteps = 5;
                    }
                }
                dx1 = -1;
            }
            if (showbuttons == false) {
                if (millis() - tnow > 500) {
                    if (flash) {
                        MoveTo(calvaluexpos, calvalueypos + 96);
                        print("Exit >>>");
                        MoveTo(calvaluexpos, (calvalueypos * 3) - 32);
                        print("Redo >>>");
                        flash = false;
                    }
                    else {
                        MoveTo(calvaluexpos, calvalueypos + 96);
                        print("        ");
                        MoveTo(calvaluexpos, (calvalueypos * 3) - 32);
                        print("        ");
                        flash = true;
                    }
                    tnow = millis();
                }
            }
        }
        if (calsteps == 1) { // Exit or Redo
            preferences.putShort("calibx1", calx1);
            preferences.putShort("calibx2", calx2);
            preferences.putShort("caliby1", caly1);
            preferences.putShort("caliby2", caly2);
            return;
        }
    }
}
