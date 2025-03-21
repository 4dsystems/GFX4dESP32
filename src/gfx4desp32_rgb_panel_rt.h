#ifndef __GFX4D_RGB_PANEL_RT__
#define __GFX4D_RGB_PANEL_RT__

#include "gfx4desp32_rgb_panel_t.h"

class gfx4desp32_rgb_panel_rt : public gfx4desp32_rgb_panel_t {
private:
    const char* TAG = "gfx4desp32_rgb_panel_rt";
    // int txpos1; // seems to be unused, only being set to 0, nothing else

public:
    gfx4desp32_rgb_panel_rt(esp_lcd_rgb_panel_config_t* panel_config, int bk_pin, int bk_on_level, int bk_off_level, int sd_gpio_sck, int sd_gpio_miso, int sd_gpio_mosi, int sd_gpio_cs, bool touchYinvert);
    ~gfx4desp32_rgb_panel_rt();

    virtual void touch_Set(uint8_t mode) override;
    virtual bool touch_Update() override;
    void touchCalibration();
	virtual int touch_GetTouchPoints(int* tpx, int* tpy) override;
};

#endif  // __GFX4D_RGB_PANEL_RT__