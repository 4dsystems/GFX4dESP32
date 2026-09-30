// SPDX-License-Identifier: Apache-2.0
// Copyright (c) 4D Systems Pty Ltd

#ifndef __GFX4D_RGB_PANEL_T__
#define __GFX4D_RGB_PANEL_T__

#include "gfx4desp32_touch.h"
#include "gfx4desp32_rgb_panel.h"
#define MAX_PENS 			 5	
#define CTP_DATA_LEN    	 1+6*MAX_PENS


class gfx4desp32_rgb_panel_t : public gfx4desp32_rgb_panel, public gfx4desp32_touch {
private:
  const char* TAG = "gfx4desp32_rgb_panel_t";
  int trotate;
public:
  gfx4desp32_rgb_panel_t(esp_lcd_rgb_panel_config_t* panel_config, int bk_pin, int bk_on_level, int bk_off_level, int sd_gpio_sck, int sd_gpio_miso, int sd_gpio_mosi, int sd_gpio_cs, bool touchYinvert);
  ~gfx4desp32_rgb_panel_t();

  virtual void touch_Set(uint8_t mode, const uint8_t * config = NULL) override;
  virtual bool touch_Update() override;
  virtual int touch_GetTouchPoints(int* tpx, int* tpy) override;
  int t_controller;
  int num_touch_points;
  int touchPoints[20];
  uint8_t gCTPData[CTP_DATA_LEN];
  int h_res;
  int v_res;
  int touch_res;
  int touch_int;
  bool alt_int;
  int lasttPenD;
};

#endif  // __GFX4D_RGB_PANEL_T__