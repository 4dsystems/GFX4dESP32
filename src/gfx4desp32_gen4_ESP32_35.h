#ifndef __GEN4_35__
#define __GEN4_35__

#include "gfx4desp32_spi_panel.h"

#define DISPLAY_TOUCH_TYPE DISP_TOUCH_NONE
#define GEN4_35CT_SWAP_TOUCHX     0

#define DISP_ILI9488

#include "gfx4desp32_spi_common.h"

#define GEN4_SPI_HRES 480
#define GEN4_SPI_VRES 320   

#ifndef GFX4d_DISPLAY_ILI9488
#define GFX4d_DISPLAY_ILI9488  0x01
#endif

#define ILI9488_INTRFC_MODE_CTL                     0xB0
#define ILI9488_FRAME_RATE_NORMAL_CTL               0xB1
#define ILI9488_INVERSION_CTL                       0xB4
#define ILI9488_FUNCTION_CTL                        0xB6
#define ILI9488_ENTRY_MODE_CTL                      0xB7
#define ILI9488_POWER_CTL_ONE                       0xC0
#define ILI9488_POWER_CTL_TWO                       0xC1
#define ILI9488_POWER_CTL_THREE                     0xC5
#define ILI9488_POSITIVE_GAMMA_CTL                  0xE0
#define ILI9488_NEGATIVE_GAMMA_CTL                  0xE1
#define ILI9488_ADJUST_CTL_THREE                    0xF7
#define ILI9488_COLOR_MODE_16BIT                    0x55
#define ILI9488_COLOR_MODE_18BIT                    0x66
#define ILI9488_INTERFACE_MODE_USE_SDO              0x00
#define ILI9488_INTERFACE_MODE_IGNORE_SDO           0x80
#define ILI9488_IMAGE_FUNCTION_DISABLE_24BIT_DATA   0x00
#define ILI9488_WRITE_MODE_BCTRL_DD_ON              0x28
#define ILI9488_FRAME_RATE_60HZ                     0xA0
#define ILI9488_INIT_LENGTH_MASK                    0x1F
#define ILI9488_INIT_DONE_FLAG                      0xFF
#define ILI9488_MADCTL                              0x36
#define ILI9488_PIXFMT                              0x3A
#define ILI9488_CMD_SLEEP_OUT                       0x11
#define ILI9488_CMD_DISPLAY_OFF                     0x28
#define ILI9488_CMD_DISPLAY_ON                      0x29
#define ILI9488_SET_IMAGE_FUNCTION                  0xe9

#define GFX4d_35_PTLAR                              0x30
#define GFX4d_35_VSCRDEF                            0x33
#define GFX4d_35_MADCTL                             0x36
#define GFX4d_35_VSCRSADD                           0x37
#define GFX4d_35_PIXFMT                             0x3A
#define GFX4d_35_FRMCTR1                            0xB1
#define GFX4d_35_FRMCTR2                            0xB2
#define GFX4d_35_FRMCTR3                            0xB3
#define GFX4d_35_INVCTR                             0xB4
#define GFX4d_35_DFUNCTR                            0xB6
#define GFX4d_35_PWCTR1                             0xC0
#define GFX4d_35_PWCTR2                             0xC1
#define GFX4d_35_PWCTR3                             0xC2
#define GFX4d_35_PWCTR4                             0xC3
#define GFX4d_35_PWCTR5                             0xC4
#define GFX4d_35_VMCTR1                             0xC5
#define GFX4d_35_VMCTR2                             0xC7
#define GFX4d_35_RDID1                              0xDA
#define GFX4d_35_RDID2                              0xDB
#define GFX4d_35_RDID3                              0xDC
#define GFX4d_35_RDID4                              0xDD
#define GFX4d_35_GMCTRP1                            0xE0
#define GFX4d_35_GMCTRN1                            0xE1
#define GFX4d_35_GAMMASET                           0x26


class gfx4desp32_gen4_ESP32_35 : public gfx4desp32_spi_panel {
private:
    const char* TAG = "gfx4desp32_gen4_ESP32_35";

    int bk_pin = GEN4_SPI_PIN_NUM_BCKL;
    int bk_on_lvl = GEN4_SPI_BCKL_ON_LEVEL;
    int bk_off_lvl = GEN4_SPI_BCKL_OFF_LEVEL;

    int sd_gpio_SCK = GEN4_SPI_SD_SCK;
    int sd_gpio_MISO = GEN4_SPI_SD_MISO;
    int sd_gpio_MOSI = GEN4_SPI_SD_MOSI;
    int sd_gpio_CS = GEN4_SPI_SD_CS;

public:
    // init as per Diablo
    uint8_t Init_Command9488[160] = {
        15, ILI9488_POSITIVE_GAMMA_CTL,     0x00, 0x13, 0x18, 0x04, 0x0F, 0x06, 0x3A, 0x56, 0x4D, 0x03, 0x0A, 0x06, 0x30, 0x3E, 0x0F,
        15, ILI9488_NEGATIVE_GAMMA_CTL,     0x00, 0x13, 0x18, 0x01, 0x11, 0x06, 0x38, 0x34, 0x4D, 0x06, 0x0D, 0x0B, 0x31, 0x37, 0x0F,
        02, ILI9488_POWER_CTL_ONE,          0x18, 0x16,
        01, ILI9488_POWER_CTL_TWO,          0x45,
        03, ILI9488_POWER_CTL_THREE,        0x00, 0x63, 0x01,
        01, ILI9488_MADCTL,                 0x48,
        01, ILI9488_PIXFMT,                 ILI9488_COLOR_MODE_18BIT,
        01, ILI9488_INTRFC_MODE_CTL,        ILI9488_INTERFACE_MODE_USE_SDO,
        01, ILI9488_FRAME_RATE_NORMAL_CTL,  0xB0,
        01, ILI9488_INVERSION_CTL,          0x02,
        02, ILI9488_FUNCTION_CTL,           0x02, 0x02,
        01, ILI9488_SET_IMAGE_FUNCTION,     0x00,
        04, ILI9488_ADJUST_CTL_THREE,       0xA9, 0x51, 0x2C, 0x82,
        0xff
    };

    uint8_t Init_CommandST[160] = {
        0x02, 0xB0, 0x00, 0xc4,
        0x01, 0xB1, 0xc0,
        0x01, 0x3A, 0x55,
        0x01, 0x36, 0x00,
        0x05, 0xb2, 0x0c, 0x0c, 0x00, 0x33, 0x33,
        //0x01, 0xB7, 0x75,
        0x01, 0xBB, 0x20, //vcom
        0x01, 0xC0, 0x2C,
        0x01, 0xC2, 0x01,
        0x01, 0xC3, 0x19,
        0x01, 0xC4, 0x20,
        0x01, 0xC6, 0x0F, //frame
        0x02, 0xD0, 0xA7, 0xA1,
        0x01, 0xbb, 0x35,
        0x01, 0xc0, 0x2c,
        0x01, 0xc2, 0x01,
        0x01, 0xc3, 0x11,
        0x01, 0xc4, 0x20,
        0x01, 0xc6, 0x0f,
        0x02, 0xd0, 0xa4, 0xa1,
        0x0E, 0xE0, 0xD0, 0x0B, 0x11, 0x0B, 0x0A, 0x26, 0x36, 0x44, 0x4B, 0x38, 0x14, 0x14, 0x2A, 0x30,
        0x0E, 0xE1, 0xD0, 0x0B, 0x11, 0x0B, 0x0A, 0x26, 0x35, 0x43, 0x4A, 0x38, 0x14, 0x14, 0x2A, 0x30,
        0x00, 0x11, //Exit Sleep
        0x00, 0x21,
        0xff
    };

    uint8_t Init_Commandili[160] = {
        0x03, 0xEF, 0x01, 0x01, 0x00,
        0x03, 0xCF, 0x00, 0xC1, 0x30,
        0x04, 0xED, 0x64, 0x03, 0x12, 0x81,
        0x03, 0xE8, 0x85, 0x00, 0x7a,
        0x05, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02,
        0x01, 0xF7, 0x20,
        0x02, 0xEA, 0x00, 0x00,
        #ifndef IPS_DISPLAY
        0x01, GFX4d_35_PWCTR1, 0x26,
        #else
        0x01, GFX4d_35_PWCTR1, 0x12,// 0x12,
        #endif
        0x01, GFX4d_35_PWCTR2, 0x11,
        0x02, GFX4d_35_VMCTR1, 0x39, 0x27,
        0x01, GFX4d_35_VMCTR2, 0xa6,
        0x01, GFX4d_35_MADCTL, 0x48,
        0x01, GFX4d_35_PIXFMT, 0x55,
        0x02, GFX4d_35_FRMCTR1, 0x00, 0x1b,
        0x03, GFX4d_35_DFUNCTR, 0x08, 0x82, 0x27,
        0x01, 0xF2, 0x00,
        0x01, GFX4d_35_GAMMASET, 0x01,
        #ifndef IPS_DISPLAY
        0x0f, GFX4d_35_GMCTRP1, 0x0F, 0x2d, 0x0e, 0x08, 0x12, 0x0a, 0x3d, 0x95, 0x31, 0x04, 0x10, 0x09, 0x09, 0x0d, 0x00,
        0x0f, GFX4d_35_GMCTRN1, 0x00, 0x12, 0x17, 0x03, 0x0d, 0x05, 0x2c, 0x44, 0x41, 0x05, 0x0F, 0x0a, 0x30, 0x32, 0x0F,
        #else
        0x0f, GFX4d_35_GMCTRP1, 0x0F, 0x31, 0x2b, 0x0c, 0x0e, 0x08, 0x4e, 0xf1, 0x37, 0x07, 0x10, 0x03, 0x0e, 0x09, 0x00,
        0x0f, GFX4d_35_GMCTRN1, 0x00, 0x0e, 0x14, 0x03, 0x11, 0x07, 0x31, 0xc1, 0x48, 0x08, 0x0f, 0x0c, 0x31, 0x36, 0x0F,
        #endif
        0xff
    };

    gfx4desp32_gen4_ESP32_35();
    ~gfx4desp32_gen4_ESP32_35();

    virtual uint8_t* SelectINIT(bool sel) override;
};

#endif  // __GEN4_35__