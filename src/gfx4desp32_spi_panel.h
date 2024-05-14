#ifndef __GFX4D_SPI_PANEL__
#define __GFX4D_SPI_PANEL__

#include "gfx4desp32.h"

// TODO: maybe remove the ones not actually needed here
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_types.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "hal/gpio_hal.h"
#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"

#include "rom/cache.h"

#define DISPLAY_INTERFACE               DISP_INTERFACE_SPI

#define ILI9488_CMD_SLEEP_OUT           0x11
#define ILI9488_CMD_DISPLAY_OFF         0x28
#define ILI9488_CMD_DISPLAY_ON          0x29
#define ILI9488_CMD_DISP_INVERSION_OFF  0x20
#define ILI9488_CMD_DISP_INVERSION_ON   0x21
#define ILI9488_CMD_COLUMN_ADDRESS_SET  0x2A
#define ILI9488_CMD_PAGE_ADDRESS_SET    0x2B
#define ILI9488_CMD_MEMORY_WRITE        0x2C

#define SEND_COORDS(start, end, io, cmd)                \
    esp_lcd_panel_io_tx_param(io, cmd, (uint8_t[]) {    \
        (start >> 8) & 0xFF,                            \
        start & 0xFF,                                   \
        ((end - 1) >> 8) & 0xFF,                        \
        (end - 1) & 0xFF,                               \
    }, 4)

#define GEN4_35CT_GAMMA_P       0xE0
#define GEN4_35CT_GAMMA_N       0xE1
#define GEN4_35CT_DATA_1        0xC0
#define GEN4_35CT_DATA_2        0xC1
#define GEN4_35CT_VCOM          0xC5
#define GEN4_35CT_DATA_3        0x36
#define GEN4_35CT_IM1           0x3A
#define GEN4_35CT_IM2           0xB0
#define GEN4_35CT_FRAME_RATE    0xB1
#define GEN4_35CT_DATA_4        0xB4
#define GEN4_35CT_INTERFACE     0xB6
#define GEN4_35CT_DATA_5        0xE9
#define GEN4_35CT_DATA_6        0xF7
#define ORIENTATION_COMMAND     0x36

#define GEN4_35CT_BCKL_ON_LEVEL   0
#define GEN4_35CT_BCKL_OFF_LEVEL  1
#define GEN4_35CT_SPI_HOST        SPI3_HOST

#define GEN4_35CT_PIN_NUM_BCKL    4
//#define GEN4_35CT_PANEL esp_lcd_new_panel_ili9488
#define GEN4_35CT_HRES            320
#define GEN4_35CT_VRES            240
#define GEN4_35CT_COLOR_SPACE     ESP_LCD_COLOR_SPACE_BGR
#define GEN4_35CT_PIXEL_CLOCK_HZ  40*1000*1000
#define GEN4_35CT_GAP_X           0
#define GEN4_35CT_GAP_Y           0
#define GEN4_35CT_MIRROR_X        false
#define GEN4_35CT_MIRROR_Y        false
#define GEN4_35CT_INVERT_COLOR    true
#define GEN4_35CT_SWAP_XY         true

#define GFX4d_TOUCH_RESET         0x0b
#define GFX4d_TOUCH_INT           0x06

#define GFX4d_DISPLAY_ILI9341_ST7789  0x00
#define GFX4d_DISPLAY_ILI9488         0x01

#define GEN4_SPI_DISPLAY 
#define GEN4_I2C_SDA			  10
#define GEN4_I2C_SCL			  9

#define LCD_OPCODE_WRITE_CMD      (0x02ULL)
#define LCD_OPCODE_READ_CMD       (0x0BULL)
#define LCD_OPCODE_WRITE_COLOR    (0x32ULL)
#define LCD_CMD_MADCTL            0x36


struct lcd_init_cmd_t {
    uint8_t cmd;            // Command byte.
    const uint8_t *param;   // Pointer to parameters.
    uint8_t param_bytes;    // Bytes of parameters.
    uint16_t delay_ms;      // Delay in milliseconds after this command.
};

struct /*st7789_*/vendor_config_t {
	const lcd_init_cmd_t* init_cmds;    ///*!< Pointer to initialization commands array.
    //                                             *   The array should be declared as `static const` and positioned outside the function.
    //                                             *   Please refer to `vendor_specific_init_default` in source file
    //                                             */
    uint16_t init_cmds_size;    /*<! Number of commands in above array */
    struct {
        unsigned int quad_mode: 1;              // Set to 1 if use quad mode (4 data line).
        unsigned int use_external_init_cmds: 1; // Set to 1 if use external initialization commands instead of internal default commands.
		unsigned int use_qspi_interface: 1;     /*<! Set to 1 if use QSPI interface, default is SPI interface */
    } flags;
};

struct Time
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekday;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

enum {
    RTC_TIMEFORMAT_HM,
    RTC_TIMEFORMAT_HMS,
    RTC_TIMEFORMAT_YYYY_MM_DD,
    RTC_TIMEFORMAT_MM_DD_YYYY,
    RTC_TIMEFORMAT_DD_MM_YYYY,
    RTC_TIMEFORMAT_YYYY_MM_DD_H_M_S,
    RTC_TIMEFORMAT_DD_MM_YYYY_H_M_S,
};

// TODO: RTC should be added to QSPI only

class gfx4desp32_spi_panel : virtual public gfx4desp32 {
private:

    const char* TAG = "gfx4desp32_spi_panel";

    int panelPin_CS;
    int panelPin_DC;
    int panelPin_MOSI;
    int panelPin_MISO;
    int panelPin_CLK;
    int panelPin_RST;
    int panel_HRES;
    int panel_VRES;
    int touchPin_SDA;
    int touchPin_SCL;
	int panelPin_DATA0;
	int panelPin_DATA1;
	int panelPin_DATA2;
	int panelPin_DATA3;
	
	uint8_t* init_cmds = NULL;

    esp_lcd_panel_io_spi_config_t* io_config = NULL;
    esp_lcd_panel_dev_config_t* panel_config = NULL;
    spi_bus_config_t* bus_config = NULL;
	vendor_config_t* vendor_config = NULL;

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    spi_device_handle_t spi_handle = NULL;

    int16_t GRAMx1;
    int16_t GRAMy1;
    int16_t GRAMx2;
    int16_t GRAMy2;
    bool wrGRAM;
    int16_t GRAMxpos;
    int16_t GRAMypos;
    uint32_t pixelPos;
    uint32_t pixelCount;
    uint32_t pPos;
    uint32_t nlSumGRAM;
    uint8_t Pixel[2];
    bool writeFBonly;
    uint16_t minFBflush;
    uint16_t maxFBflush;
    int clipX1;
    int clipY1;
    int clipX2;
    int clipY2;
    int clipX1pos;
    int clipY1pos;
    int clipX2pos;
    int clipY2pos;
    bool clippingON;
    uint8_t writeBuffInitial = 1;
    int scroll_X1;
    int scroll_Y1;
    int scroll_X2;
    int scroll_Y2;
    bool scroll_Enable;
    uint8_t scroll_Direction;
    int32_t scroll_blanking;
    uint8_t scroll_speed;
    bool flush_pending;

    int backlight;
    uint16_t _transparentColor;
    uint8_t _transMSB, _transLSB;
    uint8_t* fb = NULL;
    uint8_t* fb35 = NULL;
    uint8_t* fb352 = NULL;
    int v_res;
    int h_res;
    uint16_t* psRAMlvglBuffer;

    bool lvglBufferInit;
    uint16_t* psRAMlvglBuffer2;
    bool lvglBufferInit2;
    uint8_t bufferToggle;
    uint8_t InitData[20];
    uint8_t dispID[21];
    bool dispST7789;
    uint8_t dData[2];
	
	bool QSPI_Display;
	uint8_t madctl_val;
	char format[128];

    uint8_t scroll_Directions[16] = { 0x00, 0x01, 0x02, 0x03,
                                     0x01, 0x00, 0x03, 0x02,
                                     0x02, 0x03, 0x01, 0x00,
                                     0x03, 0x02, 0x00, 0x01 };

    void FlushArea(int y1, int y2, int xpos);
	void tx_param(int32_t lcd_cmd, const void *param, size_t param_size);
	void tx_color(int32_t lcd_cmd, const void *param, size_t param_size);
	void QSPI_mirror(bool mirror_x, bool mirror_y);
    void QSPI_swap_xy(bool swap_axes);
	uint8_t RTCread(uint8_t address);                           // read one byte from selected register
    void RTCwrite(uint8_t address, uint8_t data);               // write one byte of data to the register
    void RTCwrite_OR(uint8_t address, uint8_t data);            // write data to the register using OR operations
    void RTCwrite_AND(uint8_t address, uint8_t data);           // write data to the register using AND operation
    unsigned char bcd_to_number(uint8_t first, uint8_t second); // convert two digits to one number
    uint8_t get_first_number(unsigned short number);            // get ten’s place digit of the number
    uint8_t get_second_number(unsigned short number);           // get unit place digit of the number
    enum registers
    {
        PCF8563_address = 0x51,
        Control_status_1 = 0x00,
        Control_status_2 = 0x01,
        VL_seconds = 0x02,
        Minutes = 0x03,
        Hours = 0x04,
        Days = 0x05,
        Weekdays = 0x06,
        Century_months = 0x07,
        Years = 0x08,
        Minute_alarm = 0x09,
        Hour_alarm = 0x0A,
        Day_alarm = 0x0B,
        Weekday_alarm = 0x0C,
        CLKOUT_control = 0x0D,
        Timer_control = 0x0E,
        Timer = 0x0F,
    };
	
protected:
    // uint8_t touchType;
    int touchXraw;
    int touchYraw;

public:
    gfx4desp32_spi_panel(int panel_Pin_CS, int panel_Pin_DC, int panel_Pin_MOSI, int panel_Pin_MISO, int panel_Pin_CLK, int panel_pin_QSPI, int panel_Pin_RST, /*int* configData, */int bk_pin, int bk_on_level, int bk_off_level, int sd_gpio_SCK, int sd_gpio_MISO, int sd_gpio_MOSI, int sd_gpio_CS, int hres, int vres, bool touchXinvert = false);
    ~gfx4desp32_spi_panel();
    //esp_lcd_panel_handle_t begin();
    virtual esp_lcd_panel_handle_t __begin() override;
    virtual void FlushArea(int x1, int x2, int y1, int y2, int xpos) override;
    virtual void DisplayControl(uint8_t cmd) override;
	virtual void DisplayControl(uint8_t cmd, uint32_t val) override;
    virtual void RectangleFilled(int x1, int y1, int x2, int y2, uint16_t color) override;
    virtual void Vline(int16_t x, int16_t y, int16_t w, uint16_t color) override;
    virtual void Hline(int16_t x, int16_t y, int16_t w, uint16_t hcolor) override;
    virtual void SetGRAM(int16_t x1, int16_t y1, int16_t x2, int16_t y2) override;
    virtual void WrGRAMs(uint8_t* color_data, uint32_t len) override;
    virtual void WrGRAMs(const uint8_t* color_data, uint32_t len) override;
    virtual void WrGRAMs(uint16_t* color_data, uint32_t len) override;
    virtual void WrGRAMs(uint32_t* color_data, uint16_t len) override;
    virtual void WrGRAM(uint16_t color) override;
    virtual void pushColors(uint16_t* color_data, uint32_t len) override;
    virtual void pushColors(uint8_t* color_data, uint32_t len) override;
    virtual void pushColors(const uint8_t* color_data, uint32_t len) override;
    virtual bool StartWrite() override;
    virtual void EndWrite() override;
    virtual void BacklightOn(bool blight) override;
    virtual void Scroll(int steps) override;
    virtual void _ScrollEnable(bool scrEn) override;
    virtual void setScrollArea(int y1, int y2) override;
    virtual void setScrollBlankingColor(int32_t scolor) override;
    virtual void SmoothScrollSpeed(uint8_t sspeed) override;
    virtual int16_t getScrollareaY1() override;
    virtual void PutPixel(int16_t x, int16_t y, uint16_t color) override;
    virtual void panelOrientation(uint8_t r) override;
    virtual void FillScreen(uint16_t color) override;
    virtual int16_t getHeight(void) override;
    virtual int16_t getWidth(void) override;
    virtual uint8_t getPanelOrientation(void) override;
    virtual void Contrast(int cntrst) override;
    virtual void Invert(bool Inv) override;
    virtual void Transparency(bool trans) override;
    virtual void TransparentColor(uint16_t color) override;
    virtual void AlphaBlend(bool alphablend) override;
    virtual void AlphaBlendLevel(uint32_t alphaLevel) override;
    virtual uint16_t ReadPixel(uint16_t xrp, uint16_t yrp) override;
	virtual uint16_t ReadPixelFromFrameBuffer(uint16_t xrp, uint16_t yrp, uint8_t fb) override;
    virtual uint16_t ReadLine(int16_t x, int16_t y, int16_t w, uint16_t* data) override;
    virtual void WriteLine(int16_t x, int16_t y, int16_t w, uint16_t* data) override;
    virtual void DrawFrameBuffer(uint8_t fbnum) override;
    virtual void DrawFrameBufferArea(uint8_t fbnum, int16_t ui) override;
    virtual void DrawFrameBufferArea(uint8_t fbnum, int16_t x1, int16_t y1, int16_t x2, int16_t y2) override;
    virtual void MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1, uint8_t fbfrom2, uint16_t transColor) override;
    virtual void MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1, uint8_t fbfrom2, uint8_t fbfrom3, uint16_t transColor, uint16_t transColor1) override;
    // virtual void drawBitmap(int x1, int y1, int x2, int y2, uint16_t* c_data) override;
    virtual void CopyFrameBuffer(uint8_t fbto, uint8_t fbfrom1) override;
	virtual void CopyFrameBufferLine(int16_t x, int16_t y, int16_t w, int fb) override;
    void drawBitmap(int x1, int y1, int x2, int y2, uint16_t* c_data);
	void draw_bitmap(int x1, int y1, int x2, int y2, uint16_t* c_data);
    virtual void PinMode(byte pin, uint8_t mode) override;
    virtual void DigitalWrite(byte pin, bool state) override;
    virtual int DigitalRead(byte pin) override;
    virtual void WriteToFrameBuffer(uint32_t offset, uint8_t* data, uint32_t len) override;
    virtual void WriteToFrameBuffer(uint32_t offset, uint16_t* data, uint32_t len) override;
	virtual void AllocateFB(uint8_t sel) override;
	virtual void AllocateDRcache(uint32_t cacheSize) override;
    int rotation;
    void* wb = NULL;
    int32_t __scrWidth;
    int32_t __scrHeight;
    int32_t low_X;
    int32_t high_X;
    int32_t low_Y;
    int32_t high_Y;
    int32_t high_ypos;
    uint16_t __width;
    uint16_t __height;
    uint32_t __fbSize;
    bool I2CInit;
    void ClipWindow(int x1, int y1, int x2, int y2);
    void Clipping(bool clipping);
    void setScrollArea(int x1, int y1, int x2, int y2);
    void setScrollDirection(uint8_t scrDir);
    uint8_t* SelectFB(uint8_t sel);
    virtual uint8_t* SelectINIT(bool sel) = 0;
	void RTCinit();                     // initialize the chip
    void RTCstopClock();                // stop the clock
    void RTCstartClock();               // start the clock
    void RTCsetYear(uint16_t year);     // set year
    void RTCsetMonth(uint8_t month);    // set month
    void RTCsetDay(uint8_t day);        // set day
    void RTCsetHour(uint8_t hour);      // set hour
    void RTCsetMinute(uint8_t minute);  // set minut
    void RTCsetSecond(uint8_t second);  // set second
    Time RTCgetTime();                  // get time
    bool RTCcheckClockIntegrity();      // check clock integrity
    const char* RTCformatDateTime(uint8_t sytle);

    uint8_t InitCommandsST[160];
    uint8_t InitCommandsili[160];
    uint8_t InitCommands9488[160];
	uint8_t InitCommandsNV[320];
    uint8_t DisplayModel;
    //uint32_t __alpha;
    //uint32_t __alphatemp;
    //uint16_t __colour;
	int calx1, calx2, caly1, caly2;
};

#endif  // __GFX4D_SPI_PANEL__