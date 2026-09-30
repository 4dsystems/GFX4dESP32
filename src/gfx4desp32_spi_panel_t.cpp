// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 4D Systems Pty Ltd

#include "gfx4desp32_spi_panel_t.h"
#include "GT911.h";
GT911 tspi = GT911();

gfx4desp32_spi_panel_t::gfx4desp32_spi_panel_t(
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
	h_res = hres;
	v_res = vres;
}
gfx4desp32_spi_panel_t::~gfx4desp32_spi_panel_t() {}

/****************************************************************************/
/*!
  @brief  Enable / disable touch functions
  @param  mode - TOUCH_ENABLE / TOUCH_DISABLE
  @note   experimental
*/
/****************************************************************************/
void gfx4desp32_spi_panel_t::touch_Set(uint8_t mode, const uint8_t * config) {
    delay(100);
	if (mode == TOUCH_ENABLE) {
        _TouchEnable = true;
		if (touchFirstEnable) {
            pinMode(GFX4d_TOUCH_RESET, OUTPUT);
            pinMode(GFX4d_TOUCH_INT, INPUT);
            digitalWrite(GFX4d_TOUCH_RESET, LOW);
            delay(100);
            digitalWrite(GFX4d_TOUCH_RESET, HIGH);
            if (I2CInit == false) {
                if (Wire.begin(10, 9, 400000)) {
                    I2CInit = true;
                }
                else {
                }
            }
            pinMode(GFX4d_TOUCH_RESET, OUTPUT);
            digitalWrite(GFX4d_TOUCH_RESET, HIGH);
            if (I2CInit == false) {
				if (Wire.begin(10, 9, 400000)) {
					I2CInit = true;
				}
			}
			delay(50);
			t_controller = 0;
			Wire.beginTransmission(0x38);
			int error = Wire.endTransmission();
			if (error == 0){
				pinMode(GFX4d_TOUCH_INT, INPUT_PULLUP);
				pinMode(GFX4d_TOUCH_RESET, OUTPUT);
				digitalWrite(GFX4d_TOUCH_RESET, HIGH);
			} else {
				t_controller = 1;			
				delay(100);
				pinMode(GFX4d_TOUCH_INT, OUTPUT);
				pinMode(GFX4d_TOUCH_RESET, OUTPUT);
				digitalWrite(GFX4d_TOUCH_RESET, LOW);
				delay(15);
				digitalWrite(GFX4d_TOUCH_INT, LOW);
				delay(15);
				digitalWrite(GFX4d_TOUCH_RESET, HIGH);
				delay(55);
				pinMode(GFX4d_TOUCH_INT, INPUT);
				pinMode(GFX4d_TOUCH_RESET, INPUT);					
				if (tspi.begin(-1, -1, 0x5d, 400000)) {
				} else {
					tspi.begin(-1, -1, 0x14, 400000);
				}
				delay(200);
				if (config != NULL) {
				    tspi.fwWrite(config, 226, GT967_TOUCH_CONTROLLER); 
				} else {
					tspi.fwResolution(v_res, h_res, GT967_TOUCH_CONTROLLER);
				} 
				tspi.SetRotation(3);
				
			}
			touchFirstEnable = false;
		}
    }
    else {
        _TouchEnable = false;
    }
}

/****************************************************************************/
/*!
  @brief   Get all touch points
  @param   tpx - array to store all x touch points
  @param   tpy - array to store all y touch points
  @returns number of current touch points
  @note    if there are any touch points the number of touches will be returned
           and the tpx, tpy arrays will be filled with x and y co-ordinates.
*/
/****************************************************************************/
int gfx4desp32_spi_panel_t::touch_GetTouchPoints(int* tpx, int* tpy){
	int num = 0;
	if (!(_TouchEnable)) return 0;
	if (t_controller == 0){
		int bytesReceived, tps;
        Wire.beginTransmission(0x38);
        Wire.write((byte)2);
        Wire.endTransmission();
        bytesReceived = Wire.requestFrom(0x38, CTP_DATA_LEN);
        Wire.readBytes(gCTPData, bytesReceived);
        Wire.endTransmission();
        if (tps != 0xff) {
			num = gCTPData[0] ;
			if (num > 5) num = 5 ;
			for (int i = 0; i < num; i++) {
				tpy[i] = (((gCTPData[1 + i * 6] & 0x0f) << 8) + gCTPData[2 + i * 6]) - 1;
				tpx[i] = ((gCTPData[3 + i * 6] & 0x0f) << 8) + gCTPData[4 + i * 6] ;
			}
		}
	}
	if (t_controller == 1){
		num = tspi.touched(GT911_MODE_POLLING);
		GTPoint* tpp;
		if(num > 0){
			tpp = tspi.getPoints();
			for(int t = 0; t < num; t++){
				if (t_controller == 0){
					switch(rotation){
						case 0:
							tpx[t] = tpp[t].x;
							tpy[t] = tpp[t].y;			
							break;
						case 1:
							tpx[t] = __width - tpp[t].x;
							tpy[t] = __height - tpp[t].y;
							break;
						case 2:
							tpx[t] = __width - tpp[t].y;
							tpy[t] = tpp[t].x;
							break;
						case 3:
							tpx[t] = tpp[t].y;
							tpy[t] = __height - tpp[t].x;
							break;
					}
				} else {
					switch(rotation){
						case 0:
							tpx[t] = tpp[t].x;
							tpy[t] = tpp[t].y;			
							break;
						case 1:
							tpx[t] = __width - tpp[t].x;
							tpy[t] = __height - tpp[t].y;
							break;
						case 2:
							tpx[t] = tpp[t].y;
							tpy[t] = __height - tpp[t].x;
							break;
						case 3:
							tpx[t] = __width - tpp[t].y;
							tpy[t] = tpp[t].x;
							break;
					}
				}
			}
		}
	}
	return num;
}

/****************************************************************************/
/*!
  @brief  Update touch controller
  @note   if a touch event has occurred pen, xpos, ypos and images touched
          will be updated.
*/
/****************************************************************************/
bool gfx4desp32_spi_panel_t::touch_Update() {
	bool intStatus = digitalRead(GFX4d_TOUCH_INT);
	// *** if touch is not enabled and no touch int or touch status is not no touch
    // then just return ***
    // *** need to create touch release state before no touch ***
    if (!_TouchEnable || (intStatus && tPen == 0))
        return false;
	//_intIRQ = false;
    bool update = false;
    bool validTouch = false;
    int i = 0;
    int tps;
    uint8_t bytesReceived;
    if (t_controller == 0){
    // *** process capacitive touch ***
		Wire.beginTransmission(0x38);
		Wire.write((byte)2);
		Wire.endTransmission();
		bytesReceived = Wire.requestFrom(0x38, 6);
		Wire.readBytes(gCTPData, bytesReceived);
		Wire.endTransmission();
		tps = gCTPData[0];
		if (tps != 0xff) { // some FT chips return garbage before first valid read,
        // try and detect that
			tPen = (gCTPData[0] > 0);
			//MoveTo(300, 180); print(String(tPen)); 
			num_touch_points = tps;
			if (tPen == 1) {
				switch (rotation) {
				case LANDSCAPE_R:
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
            case LANDSCAPE:
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
            case PORTRAIT:
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
            case PORTRAIT_R:
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
			validTouch = true;
		}
	}
	if (t_controller == 1){
		tps = tspi.touched(GT911_MODE_POLLING);
		if (tps != -1) { // -1 means timeout for GT967, we might want to return
        // try and detect that
			tPen = (tps > 0);
			num_touch_points = tps;
			GTPoint* tp;
			if (tps) tp = tspi.getPoints();
			if (tPen >= 1) {
				//int temp = tp[0].y;
				//tp[0].y = tp[0].x;
				//tp[0].x = temp;
				switch (rotation) {
				case 0:
					lasttouchYpos = touchYpos;
					lasttouchXpos = touchXpos;
					touchYpos = __height - tp[0].x;
					touchXpos = tp[0].y;
					break;
				case 1:
					lasttouchYpos = touchYpos;
					lasttouchXpos = touchXpos;
					touchYpos = tp[0].x;
					touchXpos = __width - tp[0].y;
					break;
				case 2:
					lasttouchYpos = touchYpos;
					lasttouchXpos = touchXpos;
					touchYpos = tp[0].y;
					touchXpos = tp[0].x;
					break;
				case 3:
					lasttouchYpos = touchYpos;
					lasttouchXpos = touchXpos;
					touchYpos = __height - tp[0].y;
					touchXpos = __width - tp[0].x;
					break;
				}
			}
			validTouch = true;
		}
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
				if (animIndexCounter > 0){
					int aPos;
					for (int n = 0; n < animIndexCounter; n++) {
						aPos = n << 3;
						if ((anims[aPos + 7] & 0x1) == 1){
							if (touchXpos >= anims[aPos + 3] &&
                                touchXpos <= anims[aPos + 5] &&
                                touchYpos >= anims[aPos + 4] &&
                                touchYpos <= anims[aPos + 6]) {
                                gciobjtouched = -32767 + n;
                                break;
                            }
						}
					}
				}
            }
        }
    }
    return update;
}
