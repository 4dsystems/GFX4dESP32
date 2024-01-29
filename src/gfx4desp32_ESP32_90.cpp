#include "gfx4desp32_ESP32_90.h"

gfx4desp32_ESP32_90::gfx4desp32_ESP32_90()
    : gfx4desp32_rgb_panel(&panel_config, GEN4_RGB_PIN_NUM_BK_LIGHT, GEN4_RGB_BK_LIGHT_ON_LEVEL, GEN4_RGB_BK_LIGHT_OFF_LEVEL, GEN4_RGB_SD_SCK, GEN4_RGB_SD_MISO, GEN4_RGB_SD_MOSI, GEN4_RGB_SD_CS, ESP32_90_SWAP_TOUCHY, DISPLAY_TOUCH_TYPE)
{
}

gfx4desp32_ESP32_90::~gfx4desp32_ESP32_90()
{
}
