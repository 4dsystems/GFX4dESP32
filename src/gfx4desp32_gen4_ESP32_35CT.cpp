#include "gfx4desp32_gen4_ESP32_35CT.h"


gfx4desp32_gen4_ESP32_35CT::gfx4desp32_gen4_ESP32_35CT()
    : gfx4desp32_spi_panel_t(GEN4_SPI_PIN_NUM_CS, GEN4_SPI_PIN_NUM_DC, GEN4_SPI_PIN_NUM_MOSI, GEN4_SPI_PIN_NUM_MISO, GEN4_SPI_PIN_NUM_CLK, GEN4_SPI_PIN_NUM_RST, /*configData, */GEN4_SPI_PIN_NUM_BCKL, GEN4_SPI_BCKL_ON_LEVEL, GEN4_SPI_BCKL_OFF_LEVEL, GEN4_SPI_SD_SCK, GEN4_SPI_SD_MISO, GEN4_SPI_SD_MOSI, GEN4_SPI_SD_CS, GEN4_SPI_HRES, GEN4_SPI_VRES, GEN4_35CT_SWAP_TOUCHX) {

    memcpy(InitCommandsST, Init_CommandST, 160);
    memcpy(InitCommandsili, Init_Commandili, 160);
    memcpy(InitCommands9488, Init_Command9488, 160);
#ifdef IPS_DISPLAY
    IPS_Display = true;
#endif
    DisplayModel = GFX4d_DISPLAY_ILI9488;
}

gfx4desp32_gen4_ESP32_35CT::~gfx4desp32_gen4_ESP32_35CT() {
}

uint8_t* gfx4desp32_gen4_ESP32_35CT::SelectINIT(bool sel) {
    return InitCommands9488;
}