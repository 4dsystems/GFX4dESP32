#include "gfx4desp32_ESP32_90T.h"

gfx4desp32_ESP32_90T::gfx4desp32_ESP32_90T()
    : gfx4desp32_rgb_panel_rt(&panel_config, GEN4_RGB_PIN_NUM_BK_LIGHT, GEN4_RGB_BK_LIGHT_ON_LEVEL, GEN4_RGB_BK_LIGHT_OFF_LEVEL, GEN4_RGB_SD_SCK, GEN4_RGB_SD_MISO, GEN4_RGB_SD_MOSI, GEN4_RGB_SD_CS, ESP32_90T_SWAP_TOUCHY)
{
    calx1 = touchCalx1;
    calx2 = touchCalx2;
    caly1 = touchCaly1;
    caly2 = touchCaly2;
}

gfx4desp32_ESP32_90T::~gfx4desp32_ESP32_90T()
{
}