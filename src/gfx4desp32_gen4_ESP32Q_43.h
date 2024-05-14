#ifndef __GEN4Q_43__
#define __GEN4Q_43__

#include "gfx4desp32_spi_panel.h"

#define DISPLAY_TOUCH_TYPE          DISP_TOUCH_NONE
#define GEN4_43T_SWAP_TOUCHX       0 // maybe need to change

#define DISP_NV3041

#include "gfx4desp32_spi_common.h"

#define GEN4_SPI_HRES 480
#define GEN4_SPI_VRES 272

class gfx4desp32_gen4_ESP32Q_43 : public gfx4desp32_spi_panel {
private:
    const char* TAG = "gfx4desp32_gen4_ESP32Q_43";

    int bk_pin = GEN4_QSPI_PIN_NUM_BCKL;
    int bk_on_lvl = GEN4_SPI_BCKL_ON_LEVEL;
    int bk_off_lvl = GEN4_SPI_BCKL_OFF_LEVEL;

    int sd_gpio_SCK = GEN4_QSPI_SD_SCK;
    int sd_gpio_MISO = GEN4_QSPI_SD_MISO;
    int sd_gpio_MOSI = GEN4_QSPI_SD_MOSI;
    int sd_gpio_CS = GEN4_QSPI_SD_CS;
	
	int touchCalx1 = 186;
    int touchCalx2 = 3912;
    int touchCaly1 = 245;
    int touchCaly2 = 3650;

public:

#include "gfx4desp32_gen4_ESP32Q_43_Init.h"

    gfx4desp32_gen4_ESP32Q_43();
    ~gfx4desp32_gen4_ESP32Q_43();

    virtual uint8_t* SelectINIT(bool sel) override;
};

#endif  // __GEN4Q_43__