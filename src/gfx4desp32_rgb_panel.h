#ifndef __GFX4D_RGB_PANEL__
#define __GFX4D_RGB_PANEL__
#define USE_SDFAT


#include "gfx4desp32.h"
#include "Wire.h"

#define RGB_LCD_PANEL_MAX_FB_NUM         1 // maximum supported frame buffer number
#define RGB_LCD_PANEL_BOUNCE_BUF_NUM     2 // bounce buffer number
#define RGB_LCD_PANEL_DMA_LINKS_REPLICA  2

#define DISPLAY_INTERFACE             DISP_INTERFACE_RGB

#define GFX4d_IO_EXPANDER_ADDR        0x39
#define GFX4d_INPUT_PORT_REG          0x00
#define GFX4d_OUTPUT_PORT_REG         0x01
#define GFX4d_POLARITY_INVERSION_REG  0x02
#define GFX4d_CONFIG_REG              0x03
#define GFX4d_TOUCH_RESET             0x07
#define GFX4d_TOUCH_INT               0x06

#define GEN4_RGB_DISPLAY 
#define GEN4_I2C_SDA              17
#define GEN4_I2C_SCL              18


struct esp_rgb_panel_t {
    esp_lcd_panel_t base;     // Base class of generic lcd panel
    int panel_id;             // LCD panel ID
    lcd_hal_context_t hal;    // Hal layer object
    size_t data_width;        // Number of data lines
    size_t fb_bits_per_pixel; // Frame buffer color depth, in bpp
    size_t num_fbs;           // Number of frame buffers
    size_t output_bits_per_pixel; // Color depth seen from the output data line. Default to fb_bits_per_pixel, but can be changed by YUV-RGB conversion
    size_t sram_trans_align;  // Alignment for framebuffer that allocated in SRAM
    size_t psram_trans_align; // Alignment for framebuffer that allocated in PSRAM
    int disp_gpio_num;        // Display control GPIO, which is used to perform action like "disp_off"
    intr_handle_t intr;       // LCD peripheral interrupt handle
    esp_pm_lock_handle_t pm_lock; // Power management lock
    size_t num_dma_nodes;     // Number of DMA descriptors that used to carry the frame buffer
    //uint8_t *fb;
    uint8_t* fbs[RGB_LCD_PANEL_MAX_FB_NUM]; // Frame buffers
    size_t dummy1;
    size_t dummy2;
    uint8_t cur_fb_index;     // Current frame buffer index
    uint8_t bb_fb_index;      // Current frame buffer index which used by bounce buffer
    size_t fb_size;           // Size of frame buffer
    int data_gpio_nums[16]; // GPIOs used for data lines, we keep these GPIOs for action like "invert_color"

    uint32_t src_clk_hz;      // Peripheral source clock resolution    
    esp_lcd_rgb_timing_t timings; // RGB timing parameters (e.g. pclk, sync pulse, porch width)
    size_t bb_size;           // If not-zero, the driver uses two bounce buffers allocated from internal memory
    int bounce_pos_px;        // Position in whatever source material is used for the bounce buffer, in pixels
    uint8_t* bounce_buffer[RGB_LCD_PANEL_BOUNCE_BUF_NUM]; // Pointer to the bounce buffers
    size_t bb_eof_count;      // record the number we received the DMA EOF event, compare with `expect_eof_count` in the VSYNC_END ISR
    size_t expect_eof_count;  // record the number of DMA EOF event we expected to receive
    gdma_channel_handle_t dma_chan; // DMA channel handle
    esp_lcd_rgb_panel_vsync_cb_t on_vsync; // VSYNC event callback
    esp_lcd_rgb_panel_bounce_buf_fill_cb_t on_bounce_empty; // callback used to fill a bounce buffer rather than copying from the frame buffer
    esp_lcd_rgb_panel_bounce_buf_finish_cb_t on_bounce_frame_finish; // callback used to notify when the bounce buffer finish copying the entire frame
    void* user_ctx;           // Reserved user's data of callback functions
    int x_gap;                // Extra gap in x coordinate, it's used when calculate the flush window
    int y_gap;                // Extra gap in y coordinate, it's used when calculate the flush window
    portMUX_TYPE spinlock;    // to protect panel specific resource from concurrent access (e.g. between task and ISR)
    int lcd_clk_flags;        // LCD clock calculation flags
    int rotate_mask;          // panel rotate_mask mask, Or'ed of `panel_rotate_mask_t`
    struct {
        uint32_t disp_en_level : 1;       // The level which can turn on the screen by `disp_gpio_num`
        uint32_t stream_mode : 1;         // If set, the LCD transfers data continuously, otherwise, it stops refreshing the LCD when transaction done
        uint32_t fb_in_psram : 1;         // Whether the frame buffer is in PSRAM
        uint32_t need_update_pclk : 1;    // Whether to update the PCLK before start a new transaction
        uint32_t need_restart : 1;        // Whether to restart the LCD controller and the DMA
        uint32_t bb_invalidate_cache : 1; // Whether to do cache invalidation in bounce buffer mode
    } flags;
    dma_descriptor_t* dma_links[RGB_LCD_PANEL_DMA_LINKS_REPLICA]; // fbs[0] <-> dma_links[0], fbs[1] <-> dma_links[1], etc
    dma_descriptor_t dma_restart_node; // DMA descriptor used to restart the transfer
    dma_descriptor_t dma_nodes[];      // DMA descriptors pool
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


class gfx4desp32_rgb_panel : virtual public gfx4desp32 {
private:

    int RGB_InvertFix[16] = { 8, 3, 46, 9, 1, 5, 6, 7, 15, 16, 4, 45, 48, 47, 21, 14 };
    const char* TAG = "gfx4desp32_rgb_panel";
    esp_lcd_rgb_panel_config_t* panel_config = NULL;
    esp_rgb_panel_t* rgb_panel = NULL;
    int currFB;
    int32_t GRAMx1;
    int32_t GRAMy1;
    int32_t GRAMx2;
    int32_t GRAMy2;
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
    //bool clippingON;
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
    int backlight = 1;
    uint16_t _transparentColor;
    uint8_t _transMSB, _transLSB;
    uint8_t m_inp;
    uint8_t m_out;
    uint8_t m_pol;
    uint8_t m_ctrl;
    bool IOexpInit;
    int lasttxpos;
    char format[128];
    bool frameBufferIData;

    uint8_t pinNum2bitNum[8] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

    uint8_t scroll_Directions[16] = { 0x00, 0x01, 0x03, 0x03,
                                     0x01, 0x00, 0x02, 0x03,
                                     0x03, 0x02, 0x01, 0x00,
                                     0x02, 0x03, 0x00, 0x01 };

    void __start_transmission();
    bool __lcd_rgb_panel_fill_bounce_buffer(uint8_t* buffer);

    void FlushArea(int y1, int y2, int xpos);
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
    gfx4desp32_rgb_panel(esp_lcd_rgb_panel_config_t* panel_config, int bk_pin = DEFAULT_PIN_NUM_BK_LIGHT, int bk_on_level = DEFAULT_BK_LIGHT_ON_LEVEL, int bk_off_level = DEFAULT_BK_LIGHT_OFF_LEVEL, int sd_gpio_SCK = DEFAULT_SD_SCK, int sd_gpio_MISO = DEFAULT_SD_MISO, int sd_gpio_MOSI = DEFAULT_SD_MOSI, int sd_gpio_CS = DEFAULT_SD_CS, bool touchYinvert = false, uint8_t tType = DISP_TOUCH_CTP);
    ~gfx4desp32_rgb_panel();
    //esp_lcd_panel_handle_t begin();
    virtual esp_lcd_panel_handle_t __begin() override;
    virtual void FlushArea(int x1, int x2, int y1, int y2, int xpos) override;
    virtual void WriteToFrameBuffer(uint32_t offset, uint8_t* data, uint32_t len) override;
    virtual void WriteToFrameBuffer(uint32_t offset, uint16_t* data, uint32_t len) override;
    virtual void DisplayControl(uint8_t cmd) override;
    virtual void DisplayControl(uint8_t cmd, uint32_t val) override;
    virtual void RectangleFilled(int x1, int y1, int x2, int y2, uint16_t color) override;
    virtual void Vline(int16_t x, int16_t y, int16_t w, uint16_t color) override;
    virtual void Hline(int16_t x, int16_t y, int16_t w, uint16_t hcolor) override;
    virtual void SetGRAM(int16_t x1, int16_t y1, int16_t x2, int16_t y2) override;
    virtual void WrGRAMs(uint8_t* color_data, uint32_t len) override;
    virtual void WrGRAMs(uint16_t* color_data, uint32_t len) override;
    virtual void WrGRAMs(uint32_t* color_data, uint16_t len) override;
    virtual void WrGRAM(uint16_t color) override;
    virtual void WrGRAMs(const uint8_t* color_data, uint32_t len) override;
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
    virtual void PinMode(byte pin, uint8_t mode) override;
    virtual void DigitalWrite(byte pin, bool state) override;
    virtual int DigitalRead(byte pin) override;
    virtual void AllocateFB(uint8_t sel) override;
    virtual void AllocateDRcache(uint32_t cacheSize) override;
    int rotation;
    void* wb = NULL;
    int32_t __scrWidth;
    int32_t __scrHeight;
    int32_t low_Y;
    int32_t high_Y;
    int32_t low_X;
    int32_t high_X;
    uint32_t high_ypos;
    uint16_t __width;
    uint16_t __height;
    uint32_t __fbSize;
    bool I2CInit;
    virtual void ClipWindow(int x1, int y1, int x2, int y2) override;
    virtual void Clipping(bool clipping) override;
    void setScrollArea(int x1, int y1, int x2, int y2);
    void setScrollDirection(uint8_t scrDir);
    uint8_t* SelectFB(uint8_t sel);
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
    int calx1, calx2, caly1, caly2;
    //bool clippingON;
    //uint32_t __alpha;
    //uint32_t __alphatemp;
    //uint16_t __colour;
};

#endif  // __GFX4D_RGB_PANEL__