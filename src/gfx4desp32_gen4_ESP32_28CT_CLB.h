#ifndef __GEN4_28CT_CLB__
#define __GEN4_28CT_CLB__

#include "gfx4desp32_spi_panel_t.h"

#define DISPLAY_TOUCH_TYPE DISP_TOUCH_CTP
#define GEN4_28CT_SWAP_TOUCHX     1

#define DISP_ILI9341

#include "gfx4desp32_spi_common.h"

#define GEN4_SPI_HRES 320
#define GEN4_SPI_VRES 240


class gfx4desp32_gen4_ESP32_28CT_CLB : public gfx4desp32_spi_panel_t {
private:
  const char * TAG = "gfx4desp32_gen4_ESP32_28CT_CLB";
  
  int bk_pin = GEN4_SPI_PIN_NUM_BCKL;
  int bk_on_lvl = GEN4_SPI_BCKL_ON_LEVEL;
  int bk_off_lvl = GEN4_SPI_BCKL_OFF_LEVEL;
  
  int sd_gpio_SCK = GEN4_SPI_SD_SCK;
  int sd_gpio_MISO = GEN4_SPI_SD_MISO;
  int sd_gpio_MOSI = GEN4_SPI_SD_MOSI;
  int sd_gpio_CS = GEN4_SPI_SD_CS;

public:

#include "gfx4desp32_gen4_ESP32_28_Init.h"

  gfx4desp32_gen4_ESP32_28CT_CLB();
  ~gfx4desp32_gen4_ESP32_28CT_CLB();

  virtual uint8_t * SelectINIT(bool sel) override;
};

#endif  // __GEN4_28CT_CLB__