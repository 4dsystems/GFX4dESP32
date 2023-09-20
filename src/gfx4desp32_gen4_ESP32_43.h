#ifndef __GFX4D_43__
#define __GFX4D_43__

#define DISPLAY_TOUCH_TYPE          DISP_TOUCH_NONE
#define GEN4_43_PIXEL_CLOCK_HZ      (16 * 1000 * 1000)
#define GEN4_43_SWAP_TOUCHY         0 // ignored

#include "gfx4desp32_rgb_common.h"
#include "gfx4desp32_rgb_panel.h"

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_types.h"
#include "esp_err.h"
#include "esp_log.h"


class gfx4desp32_gen4_ESP32_43 : public gfx4desp32_rgb_panel {
private:
    const char* TAG = "gfx4desp32_gen4_ESP32_43";

    int bk_pin = GEN4_RGB_PIN_NUM_BK_LIGHT;
    int bk_on_lvl = GEN4_RGB_BK_LIGHT_ON_LEVEL;
    int bk_off_lvl = GEN4_RGB_BK_LIGHT_OFF_LEVEL;

    int sd_gpio_SCK = GEN4_RGB_SD_SCK;
    int sd_gpio_MISO = GEN4_RGB_SD_MISO;
    int sd_gpio_MOSI = GEN4_RGB_SD_MOSI;
    int sd_gpio_CS = GEN4_RGB_SD_CS;

    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_PLL160M,  //LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = GEN4_43_PIXEL_CLOCK_HZ,
            .h_res = GEN4_RGB_H_RES,
            .v_res = GEN4_RGB_V_RES,
            // The following parameters should refer to LCD spec
            .hsync_pulse_width = 1,
            .hsync_back_porch = 40,
            .hsync_front_porch = 20,
            .vsync_pulse_width = 1,
            .vsync_back_porch = 8,
            .vsync_front_porch = 4,
            .flags = {
                .pclk_active_neg = true,
            }
        },
        .data_width = 16,  // RGB565 in parallel mode, thus 16bit in width
        // .sram_trans_align
        .psram_trans_align = 64,
        .hsync_gpio_num = GEN4_RGB_PIN_NUM_HSYNC,
        .vsync_gpio_num = GEN4_RGB_PIN_NUM_VSYNC,
        .de_gpio_num = GEN4_RGB_PIN_NUM_DE,
        .pclk_gpio_num = GEN4_RGB_PIN_NUM_PCLK,
        .data_gpio_nums = {
            GEN4_RGB_PIN_NUM_DATA0,
            GEN4_RGB_PIN_NUM_DATA1,
            GEN4_RGB_PIN_NUM_DATA2,
            GEN4_RGB_PIN_NUM_DATA3,
            GEN4_RGB_PIN_NUM_DATA4,
            GEN4_RGB_PIN_NUM_DATA5,
            GEN4_RGB_PIN_NUM_DATA6,
            GEN4_RGB_PIN_NUM_DATA7,
            GEN4_RGB_PIN_NUM_DATA8,
            GEN4_RGB_PIN_NUM_DATA9,
            GEN4_RGB_PIN_NUM_DATA10,
            GEN4_RGB_PIN_NUM_DATA11,
            GEN4_RGB_PIN_NUM_DATA12,
            GEN4_RGB_PIN_NUM_DATA13,
            GEN4_RGB_PIN_NUM_DATA14,
            GEN4_RGB_PIN_NUM_DATA15,
        },
        .disp_gpio_num = GEN4_RGB_PIN_NUM_DISP_EN,
        // .on_frame_trans_done
        // .user_ctx
        .flags = {
            .fb_in_psram = true,  // allocate frame buffer in PSRAM
        }
    };
public:
    gfx4desp32_gen4_ESP32_43();
    ~gfx4desp32_gen4_ESP32_43();
};

#endif  // __GFX4D_43__