#include "gfx4desp32_gen4_ESP32_50CT.h"


gfx4desp32_gen4_ESP32_50CT::gfx4desp32_gen4_ESP32_50CT()
    : gfx4desp32_rgb_panel_t(&panel_config, GEN4_RGB_PIN_NUM_BK_LIGHT, GEN4_RGB_BK_LIGHT_ON_LEVEL, GEN4_RGB_BK_LIGHT_OFF_LEVEL, GEN4_RGB_SD_SCK, GEN4_RGB_SD_MISO, GEN4_RGB_SD_MOSI, GEN4_RGB_SD_CS, GEN4_50CT_SWAP_TOUCHY) {
}

gfx4desp32_gen4_ESP32_50CT::~gfx4desp32_gen4_ESP32_50CT() {
}
