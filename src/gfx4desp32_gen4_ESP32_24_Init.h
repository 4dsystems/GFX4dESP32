//==============================================================
// ST7789 LCD Driver Registers
// Warning!!!! Many of the register names used here at at odds
//             with certain versions of the ST7789 manual!!!
//==============================================================

#define GFX4dESP32_ST7789_READ_DISPLAY_PIXEL_FORMAT     0x0C
#define GFX4dESP32_ST7789_SLEEP_OUT                     0x11
#define GFX4dESP32_ST7789_GAMMA_SET                     0x26
#define GFX4dESP32_ST7789_DISPLAY_OFF                   0x28
#define GFX4dESP32_ST7789_DISPLAY_ON                    0x29
#define GFX4dESP32_ST7789_SET_COLUMN_ADDRESS            0x2A
#define GFX4dESP32_ST7789_SET_PAGE_ADDRESS              0x2B
#define GFX4dESP32_ST7789_WRITE_MEMORY                  0x2C
#define GFX4dESP32_ST7789_READ_MEMORY                   0x2E
#define GFX4dESP32_ST7789_MEMORY_ACCESS_CONTROL         0x36
#define GFX4dESP32_ST7789_WRITE_MEMORY_CONTINUE         0x3C
#define GFX4dESP32_ST7789_READ_MEMORY_CONTINUE          0x3E
#define GFX4dESP32_ST7789_PIXEL_FORMAT_SET              0x3A
#define GFX4dESP32_ST7789_RAM_CONTROL                   0xB0
#define GFX4dESP32_ST7789_FRAME_RATE_CONTROL            0xB1
#define GFX4dESP32_ST7789_PORCH_CONTROL                 0xB2
#define GFX4dESP32_ST7789_DISPLAY_FUNCTION_CONTROL      0xB6
#define GFX4dESP32_ST7789_GATE_CONTROL                  0xB7
#define GFX4dESP32_ST7789_VCOM_SETTING                  0xBB
#define GFX4dESP32_ST7789_POWER_CONTROL_1               0xC0
#define GFX4dESP32_ST7789_POWER_CONTROL_2               0xC1
#define GFX4dESP32_ST7789_VDVVRH_COMMAND_ENABLE         0xC2
#define GFX4dESP32_ST7789_VRH_SET                       0xC3
#define GFX4dESP32_ST7789_VDV_SET                       0xC4
#define GFX4dESP32_ST7789_VCOM_CONTROL_1                0xC5
#define GFX4dESP32_ST7789_FRAME_RATE_CONTROL_2          0xC6
#define GFX4dESP32_ST7789_VCOM_CONTROL_2                0xC7
#define GFX4dESP32_ST7789_POWER_CONTROL_A               0xCB
#define GFX4dESP32_ST7789_POWER_CONTROL_B               0xCF
#define GFX4dESP32_ST7789_POWER_CONTROL_1x              0xD0
#define GFX4dESP32_ST7789_POSITIVE_GAMMA_CORRECTION     0xE0
#define GFX4dESP32_ST7789_NEGATIVE_GAMMA_CORRECTION     0xE1
#define GFX4dESP32_ST7789_DRIVER_TIMING_CONTROL_A       0xE8
#define GFX4dESP32_ST7789_DRIVER_TIMING_CONTROL_B       0xEA
#define GFX4dESP32_ST7789_POWER_ON_SEQUENCE_CONTROL     0xED
#define GFX4dESP32_ST7789_UNDOCUMENTED_0xEF             0xEF
#define GFX4dESP32_ST7789_ENABLE_3G                     0xF2
#define GFX4dESP32_ST7789_INTERFACE_CONTROL             0xF6
#define GFX4dESP32_ST7789_PUMP_RATIO_CONTROL            0xF7

uint8_t Init_CommandST[160] = {
    02, GFX4dESP32_ST7789_RAM_CONTROL,                  0x00, 0xc4,
    01, GFX4dESP32_ST7789_FRAME_RATE_CONTROL,           0xc0, // c0
    03, GFX4dESP32_ST7789_PIXEL_FORMAT_SET,             0x55, 0x36, 0x00,
    05, GFX4dESP32_ST7789_PORCH_CONTROL, 0x0c, 0x0c, 0x00, 0x33, 0x33,

    01, GFX4dESP32_ST7789_GATE_CONTROL, 0x75,
    01, GFX4dESP32_ST7789_VCOM_SETTING, 0x20, // vcom   1d
    01, GFX4dESP32_ST7789_POWER_CONTROL_1, 0x2C,
    01, GFX4dESP32_ST7789_VDVVRH_COMMAND_ENABLE, 0x01,
    01, GFX4dESP32_ST7789_VRH_SET, 0x19,
    01, GFX4dESP32_ST7789_VDV_SET, 0x20,
    01, GFX4dESP32_ST7789_FRAME_RATE_CONTROL_2, 0x0F, // frame
    02, GFX4dESP32_ST7789_POWER_CONTROL_1x, 0xA7, 0xA1,

    //---------------------------------ST7789S Power setting--------------------------------------//
    01, GFX4dESP32_ST7789_VCOM_SETTING, 0x35,
    01, GFX4dESP32_ST7789_POWER_CONTROL_1, 0x2c,
    01, GFX4dESP32_ST7789_VDVVRH_COMMAND_ENABLE, 0x01,
    01, GFX4dESP32_ST7789_VRH_SET, 0x11,
    01, GFX4dESP32_ST7789_VDV_SET, 0x20,
    01, GFX4dESP32_ST7789_FRAME_RATE_CONTROL_2, 0x0f,
    02, GFX4dESP32_ST7789_POWER_CONTROL_1x, 0xa4, 0xa1,
    //--------------------------------ST7789S gamma setting---------------------------------------//
    14, GFX4dESP32_ST7789_POSITIVE_GAMMA_CORRECTION, 0xD0, 0x0B, 0x11, 0x0B, 0x0A, 0x26, 0x36, 0x44, 0x4B, 0x38, 0x14, 0x14, 0x2A, 0x30,
    14, GFX4dESP32_ST7789_NEGATIVE_GAMMA_CORRECTION, 0xD0, 0x0B, 0x11, 0x0B, 0x0A, 0x26, 0x35, 0x43, 0x4A, 0x38, 0x14, 0x14, 0x2A, 0x30,
    0xff };

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

uint8_t Init_Commandili[160] = {
    5, GFX4dESP32_ILI9341_POWER_CONTROL_A, 0x39, 0x2C, 0x00, 0x34, 0x02,
    3, GFX4dESP32_ILI9341_POWER_CONTROL_B, 0x00, 0xC1, 0x30,
    3, GFX4dESP32_ILI9341_DRIVER_TIMING_CONTROL_A, 0x85, 0x00, 0x78,
    2, GFX4dESP32_ILI9341_DRIVER_TIMING_CONTROL_B, 0x00, 0x00,
    4, GFX4dESP32_ILI9341_POWER_ON_SEQUENCE_CONTROL, 0x64, 0x03, 0x12, 0x81,
    1, GFX4dESP32_ILI9341_PUMP_RATIO_CONTROL, 0x20,
    1, GFX4dESP32_ILI9341_POWER_CONTROL_1, 0x1b, // VRH[5:0]
    1, GFX4dESP32_ILI9341_POWER_CONTROL_2, 0x10, // SAP[2:0];BT[3:0]
    2, GFX4dESP32_ILI9341_VCOM_CONTROL_1, 0x2d, 0x33,
    1, GFX4dESP32_ILI9341_MEMORY_ACCESS_CONTROL, 0x08,
    1, GFX4dESP32_ILI9341_PIXEL_FORMAT_SET, 0x55,
    2, GFX4dESP32_ILI9341_FRAME_RATE_CONTROL, 0x00, 0x1d,
    2, GFX4dESP32_ILI9341_DISPLAY_FUNCTION_CONTROL, 0x0A, 0x82, // 0x82 was 0xa2,
    1, GFX4dESP32_ILI9341_ENABLE_3G, 0x00,
    1, GFX4dESP32_ILI9341_GAMMA_SET, 0x01,
    15, GFX4dESP32_ILI9341_POSITIVE_GAMMA_CORRECTION, 0x0F, 0x3a, 0x36, 0x0b, 0x0d, 0x06, 0x4c, 0x91, 0x31, 0x08, 0x10, 0x04, 0x11, 0x0c, 0x00,
    15, GFX4dESP32_ILI9341_NEGATIVE_GAMMA_CORRECTION, 0x00, 0x06, 0x0a, 0x05, 0x12, 0x09, 0x2c, 0x92, 0x3f, 0x08, 0x0e, 0x0b, 0x2e, 0x33, 0x0F,
    0xff };
