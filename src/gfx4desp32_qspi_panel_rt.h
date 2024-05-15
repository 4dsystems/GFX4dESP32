#ifndef __GFX4D_QSPI_PANEL_RT__
#define __GFX4D_QSPI_PANEL_RT__

#include "gfx4desp32_qspi_panel_t.h"

class gfx4desp32_qspi_panel_rt : public gfx4desp32_qspi_panel_t {
private:
    const char* TAG = "gfx4desp32_qspi_panel_rt";
    int txpos1;
    int touchXswap;
public:
    gfx4desp32_qspi_panel_rt(int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI,
        int panel_Pin_MISO, int panel_Pin_CLK, int panel_Pin_QSPI,
        int panel_Pin_RST, int bk_pin, int bk_on_level,
        int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO,
        int sd_gpio_MOSI, int sd_gpio_CS, int hres, int vres, bool touchXinvert);
    ~gfx4desp32_qspi_panel_rt();

    virtual void touch_Set(uint8_t mode) override;
    virtual bool touch_Update() override;
    void touchCalibration();

};

#endif  // __GFX4D_QSPI_PANEL_RT__