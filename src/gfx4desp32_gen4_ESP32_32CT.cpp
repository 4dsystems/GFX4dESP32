#include "gfx4desp32_gen4_ESP32_32CT.h"


gfx4desp32_gen4_ESP32_32CT::gfx4desp32_gen4_ESP32_32CT()
    : gfx4desp32_spi_panel_t(GEN4_SPI_PIN_NUM_CS, GEN4_SPI_PIN_NUM_DC, GEN4_SPI_PIN_NUM_MOSI, GEN4_SPI_PIN_NUM_MISO, GEN4_SPI_PIN_NUM_CLK, -1, GEN4_SPI_PIN_NUM_RST, /*configData, */GEN4_SPI_PIN_NUM_BCKL, GEN4_SPI_BCKL_ON_LEVEL, GEN4_SPI_BCKL_OFF_LEVEL, GEN4_SPI_SD_SCK, GEN4_SPI_SD_MISO, GEN4_SPI_SD_MOSI, GEN4_SPI_SD_CS, GEN4_SPI_HRES, GEN4_SPI_VRES, GEN4_32CT_SWAP_TOUCHX) {

    memcpy(InitCommandsST, Init_CommandST, 160);
    memcpy(InitCommandsili, Init_Commandili, 160);
    IPS_Display = false;
}

gfx4desp32_gen4_ESP32_32CT::~gfx4desp32_gen4_ESP32_32CT() {
}

uint8_t* gfx4desp32_gen4_ESP32_32CT::SelectINIT(bool sel)
{
    if (sel)
    {
        return InitCommandsST;
    }
    else
    {
        return InitCommandsili;
    }
}