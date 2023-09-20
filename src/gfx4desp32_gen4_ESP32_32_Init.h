// ST7789 driver never shipped in 3.2"
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

//==============================================================
// ILI9341 LCD Driver Registers
//==============================================================
#define GFX4dESP32_ILI9341_READ_DISPLAY_PIXEL_FORMAT    0x0C
#define GFX4dESP32_ILI9341_SLEEP_OUT                    0x11
#define GFX4dESP32_ILI9341_GAMMA_SET                    0x26
#define GFX4dESP32_ILI9341_DISPLAY_OFF                  0x28
#define GFX4dESP32_ILI9341_DISPLAY_ON                   0x29
#define GFX4dESP32_ILI9341_SET_COLUMN_ADDRESS           0x2A
#define GFX4dESP32_ILI9341_SET_PAGE_ADDRESS             0x2B
#define GFX4dESP32_ILI9341_WRITE_MEMORY                 0x2C
#define GFX4dESP32_ILI9341_READ_MEMORY                  0x2E
#define GFX4dESP32_ILI9341_MEMORY_ACCESS_CONTROL        0x36
#define GFX4dESP32_ILI9341_WRITE_MEMORY_CONTINUE        0x3C
#define GFX4dESP32_ILI9341_READ_MEMORY_CONTINUE         0x3E
#define GFX4dESP32_ILI9341_PIXEL_FORMAT_SET             0x3A
#define GFX4dESP32_ILI9341_FRAME_RATE_CONTROL           0xB1
#define GFX4dESP32_ILI9341_DISPLAY_FUNCTION_CONTROL     0xB6
#define GFX4dESP32_ILI9341_POWER_CONTROL_1              0xC0
#define GFX4dESP32_ILI9341_POWER_CONTROL_2              0xC1
#define GFX4dESP32_ILI9341_VCOM_CONTROL_1               0xC5
#define GFX4dESP32_ILI9341_VCOM_CONTROL_2               0xC7
#define GFX4dESP32_ILI9341_POWER_CONTROL_A              0xCB
#define GFX4dESP32_ILI9341_POWER_CONTROL_B              0xCF
#define GFX4dESP32_ILI9341_POSITIVE_GAMMA_CORRECTION    0xE0
#define GFX4dESP32_ILI9341_NEGATIVE_GAMMA_CORRECTION    0xE1
#define GFX4dESP32_ILI9341_DRIVER_TIMING_CONTROL_A      0xE8
#define GFX4dESP32_ILI9341_DRIVER_TIMING_CONTROL_B      0xEA
#define GFX4dESP32_ILI9341_POWER_ON_SEQUENCE_CONTROL    0xED
#define GFX4dESP32_ILI9341_UNDOCUMENTED_0xEF            0xEF
#define GFX4dESP32_ILI9341_ENABLE_3G                    0xF2
#define GFX4dESP32_ILI9341_INTERFACE_CONTROL            0xF6
#define GFX4dESP32_ILI9341_PUMP_RATIO_CONTROL           0xF7
#define GFX4dESP32_ILI9341_INVOFF                       0x20
#define GFX4dESP32_ILI9341_INVON                        0x21

uint8_t Init_Commandili[160] = {
    3,  GFX4dESP32_ILI9341_INTERFACE_CONTROL,           0x01, 0x01, 0x00,
    3,  GFX4dESP32_ILI9341_UNDOCUMENTED_0xEF,           0x03, 0x80, 0x02,
    3,  GFX4dESP32_ILI9341_POWER_CONTROL_B,             0x00, 0xF2, 0xA0,
    4,  GFX4dESP32_ILI9341_POWER_ON_SEQUENCE_CONTROL,   0x64, 0x03, 0x12, 0x81,
    5,  GFX4dESP32_ILI9341_POWER_CONTROL_A,             0x39, 0x2C, 0x00, 0x34, 0x02,
    2,  GFX4dESP32_ILI9341_DRIVER_TIMING_CONTROL_B,     0x00, 0x00,
    3,  GFX4dESP32_ILI9341_DRIVER_TIMING_CONTROL_A,     0x85, 0x10, 0x7A,
    1,  GFX4dESP32_ILI9341_POWER_CONTROL_1,             0x21,               //VRH[5:0]
    1,  GFX4dESP32_ILI9341_POWER_CONTROL_2,             0x11,               //SAP[2:0];BT[3:0]
    2,  GFX4dESP32_ILI9341_VCOM_CONTROL_1,              0x3F, 0x3C,
    1,  GFX4dESP32_ILI9341_VCOM_CONTROL_2,              0xC6,               // 0xD2
    1,  GFX4dESP32_ILI9341_PIXEL_FORMAT_SET,            0x55,
    1,  GFX4dESP32_ILI9341_MEMORY_ACCESS_CONTROL,       0x00,
    2,  GFX4dESP32_ILI9341_FRAME_RATE_CONTROL,          0x00, 0x1B,
    2,  GFX4dESP32_ILI9341_DISPLAY_FUNCTION_CONTROL,    0x0A, 0x82,         // changed A2 to 82
    1,  GFX4dESP32_ILI9341_ENABLE_3G,                   0x00,               // 3Gamma function disable
    1,  GFX4dESP32_ILI9341_GAMMA_SET,                   0x01,               // Gamma curve selected
    15, GFX4dESP32_ILI9341_POSITIVE_GAMMA_CORRECTION,   0x0f, 0x24, 0x21, 0x0F, 0x13, 0x0A, 0x52, 0xC9, 0x3B, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00,
    15, GFX4dESP32_ILI9341_NEGATIVE_GAMMA_CORRECTION,   0x00, 0x1B, 0x1E, 0x00, 0x0C, 0x04, 0x2F, 0x36, 0x44, 0x0a, 0x1F, 0x0F, 0x3F, 0x3F, 0x0F,
    0xff 
};
