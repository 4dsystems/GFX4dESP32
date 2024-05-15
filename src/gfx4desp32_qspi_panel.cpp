#include "gfx4desp32_qspi_panel.h"

gfx4desp32_qspi_panel::gfx4desp32_qspi_panel(
    int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI, int panel_Pin_MISO,
    int panel_Pin_CLK, int panel_pin_QSPI, int panel_Pin_RST, int bk_pin, int bk_on_level,
    int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO, int sd_gpio_MOSI,
    int sd_gpio_CS, int hres, int vres, bool touchXinvert)
    : gfx4desp32_spi_panel(
        panel_Pin_CS, panel_Pin_DC, panel_Pin_MOSI, panel_Pin_MISO,
        panel_Pin_CLK, panel_pin_QSPI, panel_Pin_RST, bk_pin, bk_on_level, bk_off_level,
        sd_gpio_SCK, sd_gpio_MISO, sd_gpio_MOSI, sd_gpio_CS, hres, vres, touchXinvert),
      gfx4desp32_rtc() {
}
gfx4desp32_qspi_panel::~gfx4desp32_qspi_panel() {}
