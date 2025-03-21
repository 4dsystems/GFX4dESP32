#include "gfx4desp32_gen4_ESP32Q_43CT.h"


gfx4desp32_gen4_ESP32Q_43CT::gfx4desp32_gen4_ESP32Q_43CT()
    : gfx4desp32_qspi_panel_t(GEN4_QSPI_PIN_NUM_CS, GEN4_QSPI_PIN_NUM_DATA0, GEN4_QSPI_PIN_NUM_DATA1, GEN4_QSPI_PIN_NUM_DATA2, GEN4_QSPI_PIN_NUM_CLK, GEN4_QSPI_PIN_NUM_DATA3, GEN4_QSPI_PIN_NUM_RST, /*configData, */GEN4_QSPI_PIN_NUM_BCKL, GEN4_SPI_BCKL_ON_LEVEL, GEN4_SPI_BCKL_OFF_LEVEL, GEN4_QSPI_SD_SCK, GEN4_QSPI_SD_MISO, GEN4_QSPI_SD_MOSI, GEN4_QSPI_SD_CS, GEN4_SPI_HRES, GEN4_SPI_VRES, GEN4_43CT_SWAP_TOUCHX) {

    memcpy(InitCommandsNV, Init_CommandNV, 320);
    IPS_Display = true;
}

gfx4desp32_gen4_ESP32Q_43CT::~gfx4desp32_gen4_ESP32Q_43CT() {
}

uint8_t* gfx4desp32_gen4_ESP32Q_43CT::SelectINIT(bool sel)
{
    return InitCommandsNV;
}