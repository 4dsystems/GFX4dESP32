#ifndef __GFX4D_RGB_PANEL_T__
#define __GFX4D_RGB_PANEL_T__

#include "gfx4desp32_touch.h"
#include "gfx4desp32_rgb_panel.h"

class gfx4desp32_rgb_panel_t : public gfx4desp32_rgb_panel, public gfx4desp32_touch {
private:
  const char* TAG = "gfx4desp32_rgb_panel_t";
public:
  gfx4desp32_rgb_panel_t(esp_lcd_rgb_panel_config_t* panel_config, int bk_pin, int bk_on_level, int bk_off_level, int sd_gpio_sck, int sd_gpio_miso, int sd_gpio_mosi, int sd_gpio_cs, bool touchYinvert);
  ~gfx4desp32_rgb_panel_t();

  virtual void touch_Set(uint8_t mode) override;
  virtual bool touch_Update() override;
};

#endif  // __GFX4D_RGB_PANEL_T__