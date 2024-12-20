#ifndef __GFX4D_QSPI_PANEL__
#define __GFX4D_QSPI_PANEL__

#include "gfx4desp32_spi_panel.h"
#include "gfx4desp32_rtc.h"
#include "Wire.h"


class gfx4desp32_qspi_panel : public gfx4desp32_spi_panel, public gfx4desp32_rtc {
private:
  const char* TAG = "gfx4desp32_qspi_panel";

public:
  gfx4desp32_qspi_panel(int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI,
    int panel_Pin_MISO, int panel_Pin_CLK, int panel_Pin_QSPI,
    int panel_Pin_RST, int bk_pin, int bk_on_level,
    int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO,
    int sd_gpio_MOSI, int sd_gpio_CS, int hres, int vres, bool touchXinvert);
  ~gfx4desp32_qspi_panel();

};

#endif // __GFX4D_QSPI_PANEL__