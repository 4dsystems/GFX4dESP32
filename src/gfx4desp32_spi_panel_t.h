#ifndef __GFX4D_SPI_PANEL_T__
#define __GFX4D_SPI_PANEL_T__

#include "gfx4desp32_touch.h"
#include "gfx4desp32_spi_panel.h"
#include "Wire.h"

class gfx4desp32_spi_panel_t : public gfx4desp32_spi_panel, public gfx4desp32_touch {
private:
  const char *TAG = "gfx4desp32_spi_panel_t";

public:
  gfx4desp32_spi_panel_t(int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI,
                         int panel_Pin_MISO, int panel_Pin_CLK,
                         int panel_Pin_RST, int bk_pin, int bk_on_level,
                         int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO,
                         int sd_gpio_MOSI, int sd_gpio_CS, int hres, int vres, bool touchXinvert);
  ~gfx4desp32_spi_panel_t();

  virtual void touch_Set(uint8_t mode) override;
  virtual bool touch_Update() override;
};

#endif // __GFX4D_SPI_PANEL_T__