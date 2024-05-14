#ifndef __GEN4Q_43CT__
#define __GEN4Q_43CT__

#include "gfx4desp32_qspi_panel_t.h"

#define DISPLAY_TOUCH_TYPE          DISP_TOUCH_CTP
#define GEN4_43CT_SWAP_TOUCHX       0 // maybe need to change

#define DISP_NV3041

#include "gfx4desp32_spi_common.h"

#define GEN4_SPI_HRES 480
#define GEN4_SPI_VRES 272

class gfx4desp32_gen4_ESP32Q_43CT : public gfx4desp32_qspi_panel_t {
private:
    const char* TAG = "gfx4desp32_gen4_ESP32Q_43CT";

    int bk_pin = GEN4_QSPI_PIN_NUM_BCKL;
    int bk_on_lvl = GEN4_SPI_BCKL_ON_LEVEL;
    int bk_off_lvl = GEN4_SPI_BCKL_OFF_LEVEL;

    int sd_gpio_SCK = GEN4_QSPI_SD_SCK;
    int sd_gpio_MISO = GEN4_QSPI_SD_MISO;
    int sd_gpio_MOSI = GEN4_QSPI_SD_MOSI;
    int sd_gpio_CS = GEN4_QSPI_SD_CS;

public:

#include "gfx4desp32_gen4_ESP32Q_43_Init.h"

    gfx4desp32_gen4_ESP32Q_43CT();
    ~gfx4desp32_gen4_ESP32Q_43CT();

    virtual uint8_t* SelectINIT(bool sel) override;
};

#endif  // __GEN4Q_43CT__