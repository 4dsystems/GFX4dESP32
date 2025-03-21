#ifndef __GFX4D_QSPI_PANEL_T__
#define __GFX4D_QSPI_PANEL_T__

#include "gfx4desp32_touch.h"
#include "gfx4desp32_qspi_panel.h"
#include "Wire.h"
#define MAX_PENS 			 5	
#define CTP_DATA_LEN    	 1+6*MAX_PENS

class gfx4desp32_qspi_panel_t : public gfx4desp32_qspi_panel, public gfx4desp32_touch {
private:
  const char* TAG = "gfx4desp32_qspi_panel_t";

public:
  gfx4desp32_qspi_panel_t(int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI,
    int panel_Pin_MISO, int panel_Pin_CLK, int panel_Pin_QSPI,
    int panel_Pin_RST, int bk_pin, int bk_on_level,
    int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO,
    int sd_gpio_MOSI, int sd_gpio_CS, int hres, int vres, bool touchXinvert);
  ~gfx4desp32_qspi_panel_t();

  virtual void touch_Set(uint8_t mode) override;
  virtual bool touch_Update() override;
  virtual int touch_GetTouchPoints(int* tpx, int* tpy) override;
  uint8_t gCTPData[CTP_DATA_LEN];
};

#endif // __GFX4D_QSPI_PANEL_T__
