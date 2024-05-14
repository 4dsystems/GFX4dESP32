#ifndef __GEN4Q_43T__
#define __GEN4Q_43T__

#define DISPLAY_TOUCH_TYPE          DISP_TOUCH_RTP
#define GEN4_43T_SWAP_TOUCHX       0 // maybe need to change

#define DISP_NV3041

#include "gfx4desp32_spi_common.h"
#include "gfx4desp32_qspi_panel_rt.h"

#define GEN4_SPI_HRES 480
#define GEN4_SPI_VRES 272

class gfx4desp32_gen4_ESP32Q_43T : public gfx4desp32_qspi_panel_rt {
private:
    const char* TAG = "gfx4desp32_gen4_ESP32Q_43CT";

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

    gfx4desp32_gen4_ESP32Q_43T();
    ~gfx4desp32_gen4_ESP32Q_43T();

    virtual uint8_t* SelectINIT(bool sel) override;
};

#endif  // __GEN4Q_43T__