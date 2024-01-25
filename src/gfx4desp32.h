#ifndef __GFX4DESP32__
#define __GFX4DESP32__

#if ARDUINO >= 100
#include "Arduino.h"
#include "Print.h"
#else
#include "WProgram.h"
#endif

#include "HTTPClient.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiClientSecure.h"


#include "esp_err.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_types.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_private/gdma.h"
#include "hal/dma_types.h"
#include "hal/gpio_hal.h"
#include "hal/lcd_hal.h"
#include "hal/lcd_ll.h"


#include "rom/cache.h"

//**** Uncomment below if LittleFS File sysem is used instead of uSD ****
// #define USE_LITTLEFS_FILE_SYSTEM

//**** Uncomment below if SD_MMC File sysem is used instead of SdFat. Note, SPI is not available to user on RGB displays when SDMMC is enabled ****
//#define USE_SDMMC_FILE_SYSTEM

//**** Uncomment below if external SDMMC slot is used instead of on-board slot, Note, not available on RGB displays ****
//#define SDMMC_4BIT

#include "gfx4desp32_types.h"

#ifdef USE_LITTLEFS_FILE_SYSTEM
#include <LittleFS.h>
#else
#ifdef USE_SDMMC_FILE_SYSTEM
#include "SD_MMC.h"
#include <FS.h>
#else   
#include <SdFat.h>
#endif
#endif

#define QSPI_SDMMC_4BIT_CLK     14
#define QSPI_SDMMC_4BIT_DATA2   15
#define QSPI_SDMMC_4BIT_DATA0   16
#define QSPI_SDMMC_4BIT_DATA1   21
#define QSPI_SDMMC_4BIT_DATA3   38
#define QSPI_SDMMC_4BIT_CMD     39

#define SPI_SDMMC_4BIT_CLK      15
#define SPI_SDMMC_4BIT_DATA2    48
#define SPI_SDMMC_4BIT_DATA0    47
#define SPI_SDMMC_4BIT_DATA1    38
#define SPI_SDMMC_4BIT_DATA3    39
#define SPI_SDMMC_4BIT_CMD      40

#define gfx4d_font      File

#define SD_BUFF_SIZE    8 * 1024 // 102400

#define ON              true
#define OFF             false
#define ALL             -1

#define BK_LIGHT_STARTUP_LEVEL      15
#define DEFAULT_BK_LIGHT_ON_LEVEL   1
#define DEFAULT_BK_LIGHT_OFF_LEVEL  !DEFAULT_BK_LIGHT_ON_LEVEL
#define BEGIN_WITH_BACKLIGHT_OFF    false
#define BEGIN_WITH_BACKLIGHT_ON     true

#define DEFAULT_PIN_NUM_BK_LIGHT    2

#define DEFAULT_SD_SCK              12
#define DEFAULT_SD_MISO             13
#define DEFAULT_SD_MOSI             11
#define DEFAULT_SD_CS               10

#define IPS_DISPLAY                 "ips_display"
#define TN_DISPLAY                  "tn_display"
#define DEFAULT_DISPLAY             "default_display"

#define ALICEBLUE                   0xF7DF
#define ANTIQUEWHITE                0xFF5A
#define AQUA                        0x07FF
#define AQUAMARINE                  0x7FFA
#define AZURE                       0xF7FF
#define BEIGE                       0xF7BB
#define BISQUE                      0xFF38
#define BLACK                       0x0000
#define BLANCHEDALMOND              0xFF59
#define BLUE                        0x001F
#define BLUEVIOLET                  0x895C
#define BROWN                       0xA145
#define BURLYWOOD                   0xDDD0
#define CADETBLUE                   0x5CF4
#define CHARTREUSE                  0x7FE0
#define CHOCOLATE                   0xD343
#define CORAL                       0xFBEA
#define CORNFLOWERBLUE              0x64BD
#define CORNSILK                    0xFFDB
#define CRIMSON                     0xD8A7
#define CYAN                        0x07FF
#define DARKBLUE                    0x0011
#define DARKCYAN                    0x0451
#define DARKGOLDENROD               0xBC21
#define DARKGRAY                    0xAD55
#define DARKGREEN                   0x0320
#define DARKKHAKI                   0xBDAD
#define DARKMAGENTA                 0x8811
#define DARKOLIVEGREEN              0x5345
#define DARKORANGE                  0xFC60
#define DARKORCHID                  0x9999
#define DARKRED                     0x8800
#define DARKSALMON                  0xECAF
#define DARKSEAGREEN                0x8DF1
#define DARKSLATEBLUE               0x49F1
#define DARKSLATEGRAY               0x2A69
#define DARKTURQUOISE               0x067A
#define DARKVIOLET                  0x901A
#define DEEPPINK                    0xF8B2
#define DEEPSKYBLUE                 0x05FF
#define DIMGRAY                     0x6B4D
#define DODGERBLUE                  0x1C9F
#define FIREBRICK                   0xB104
#define FLORALWHITE                 0xFFDE
#define FORESTGREEN                 0x2444
#define FUCHSIA                     0xF81F
#define GAINSBORO                   0xDEFB
#define GHOSTWHITE                  0xFFDF
#define GOLD                        0xFEA0
#define GOLDENROD                   0xDD24
#define GRAY                        0x8410
#define GREEN                       0x0400
#define GREENYELLOW                 0xAFE5
#define HONEYDEW                    0xF7FE
#define HOTPINK                     0xFB56
#define INDIANRED                   0xCAEB
#define INDIGO                      0x4810
#define IVORY                       0xFFFE
#define KHAKI                       0xF731
#define LAVENDER                    0xE73F
#define LAVENDERBLUSH               0xFF9E
#define LAWNGREEN                   0x7FE0
#define LEMONCHIFFON                0xFFD9
#define LIGHTBLUE                   0xAEDC
#define LIGHTCORAL                  0xF410
#define LIGHTCYAN                   0xE7FF
#define LIGHTGOLD                   0xFFDA
#define LIGHTGREEN                  0x9772
#define LIGHTGREY                   0xD69A
#define LIGHTPINK                   0xFDB8
#define LIGHTSALMON                 0xFD0F
#define LIGHTSEAGREEN               0x2595
#define LIGHTSKYBLUE                0x867F
#define LIGHTSLATEGRAY              0x7453
#define LIGHTSTEELBLUE              0xB63B
#define LIGHTYELLOW                 0xFFFC
#define LIME                        0x07E0
#define LIMEGREEN                   0x3666
#define LINEN                       0xFF9C
#define MAGENTA                     0xF81F
#define MAROON                      0x8000
#define MEDIUMAQUAMARINE            0x6675
#define MEDIUMBLUE                  0x0019
#define MEDIUMORCHID                0xBABA
#define MEDIUMPURPLE                0x939B
#define MEDIUMSEAGREEN              0x3D8E
#define MEDIUMSLATEBLUE             0x7B5D
#define MEDIUMSPRINGGREEN           0x07D3
#define MEDIUMTURQUOISE             0x4E99
#define MEDIUMVIOLETRED             0xC0B0
#define MIDNIGHTBLUE                0x18CE
#define MINTCREAM                   0xF7FF
#define MISTYROSE                   0xFF3C
#define MOCCASIN                    0xFF36
#define NAVAJOWHITE                 0xFEF5
#define NAVY                        0x0010
#define OLDLACE                     0xFFBC
#define OLIVE                       0x8400
#define OLIVEDRAB                   0x6C64
#define ORANGE                      0xFD20
#define ORANGERED                   0xFA20
#define ORCHID                      0xDB9A
#define PALEGOLDENROD               0xEF55
#define PALEGREEN                   0x9FD3
#define PALETURQUOISE               0xAF7D
#define PALEVIOLETRED               0xDB92
#define PAPAYAWHIP                  0xFF7A
#define PEACHPUFF                   0xFED7
#define PERU                        0xCC27
#define PINK                        0xFE19
#define PLUM                        0xDD1B
#define POWDERBLUE                  0xB71C
#define PURPLE                      0x8010
#define RED                         0xF800
#define ROSYBROWN                   0xBC71
#define ROYALBLUE                   0x435C
#define SADDLEBROWN                 0x8A22
#define SALMON                      0xFC0E
#define SANDYBROWN                  0xF52C
#define SEAGREEN                    0x2C4A
#define SEASHELL                    0xFFBD
#define SIENNA                      0xA285
#define SILVER                      0xC618
#define SKYBLUE                     0x867D
#define SLATEBLUE                   0x6AD9
#define SLATEGRAY                   0x7412
#define SNOW                        0xFFDF
#define SPRINGGREEN                 0x07EF
#define STEELBLUE                   0x4416
#define TAN                         0xD5B1
#define TEAL                        0x0410
#define THISTLE                     0xDDFB
#define TOMATO                      0xFB08
#define TURQUOISE                   0x471A
#define VIOLET                      0xEC1D
#define WHEAT                       0xF6F6
#define WHITE                       0xFFFF
#define WHITESMOKE                  0xF7BE
#define YELLOW                      0xFFE0
#define YELLOWGREEN                 0x9E66

#define HLSMAX                      127
#define HLSMAXm2d3                  85
#define HLSMAXd12                   10
#define HLSMAXd6                    21
#define HLSMAXd2                    63
#define HLSMAXd3                    42
#define RGBMAX                      127
#define RGBMAXm2                    254
#define UNDEFINED                   85
#define _redmask                    0xF800
#define _greenmask                  0x07E0
#define _bluemask                   0x001F

#define IMAGE_XPOS                  1
#define IMAGE_YPOS                  2
#define IMAGE_WIDTH                 4
#define IMAGE_HEIGHT                8
#define IMAGE_VALUE                 16
#define IMAGE_XYPOS                 32
#define IMAGE_INDEX                 64

#define SOLID                       0
#define DOTMATRIXROUND              1
#define DOTMATRIXLED                3
#define DOTMATRIXSQUARE             4
#define DOTMATRIXFADE               5
#define SYSTEM                      1
#define FONT1                       1
#define FONT2                       2
#define Pressed                     1
#define Released                    0
#define SLIDER_RAISED               0
#define SLIDER_SUNKEN               1
#define NOTOUCH                     0
#define T_SENSE                     10
#define TOUCH_PRESSED               1
#define TOUCH_RELEASED              2
#define TOUCH_STATUS                0
#define TOUCH_GETX                  1
#define TOUCH_GETY                  2
#define SLIDER3IMAGE                6
#define GAUGE2IMAGE                 5
#define TOGGLE4STATES               4
#define KEYPAD                      3
#define MOMENTARY                   2
#define TOGGLE                      1
#define DRAW_UPDOWN                 2
#define DRAW_DOWNONLY               1
#define DRAW_NONE                   0

#define MAX_ARCSIZE                 200
#define GRADIENT_RAISED             0
#define GRADIENT_SUNKEN             1
#define GRADIENT_FLATTEN            -1
#define GRADIENT_HIGH               63
#define GRADIENT_MEDIUM             32
#define GRADIENT_LOW                24
#define GRADIENT_OFF                0
#define GRADIENT_DARKEN0            0
#define GRADIENT_DARKEN1            1
#define GRADIENT_DARKEN2            2
#define BUTTON_SQUARE               0
#define BUTTON_ROUNDED              1
#define BUTTON_CIRCULAR             2
#define BUTTON_CIRCULAR_TOP         3

#define max_ARCSIZE                 300

#define HORIZONTAL                  false
#define VERTICAL                    true
#define HORIZONTAL_SLIDER           0x00
#define VERTICAL_SLIDER             0x01
#define SHIFT                       0x01
#define CAPSLOCK                    0x02
#define CTRL                        0x03
#define SPRITE_ACTIVE               0x00
#define SPRITE_MEMPOS               0x01
#define SPRITE_X                    0x02
#define SPRITE_Y                    0x03
#define SPRITE_WIDTH                0x04
#define SPRITE_HEIGHT               0x05
#define SPRITE_COLLIDE1             0x06
#define SPRITE_COLLIDE2             0x07
#define SPRITE_16BIT                0x01
#define SPRITE_8BIT                 0x02
#define SPRITE_4BIT                 0x03

#define TOUCH_ENABLE                0x00
#define TOUCH_DISABLE               0x01

#define SCROLL_UP                   0x00
#define SCROLL_DOWN                 0x01
#define SCROLL_RIGHT                0x02
#define SCROLL_LEFT                 0x03

#define NORMAL                      0x00
#define FAST                        0x01
#define TRANSPARENT                 0x01
#define OPAQUE                      0x00
#define NO_TRANSPARENCY             -1

#define GCI_SYSTEM_USD              0x00
#define GCI_SYSTEM_LITTLEFS         0x01
#define GCI_SYSTEM_PROGMEM          0x02
#define GCI_SYSTEM_SPECIAL          0x03

#define FILE_SYSTEM_DEFAULT         0x00
#define FILE_SYSTEM_USD             0x01
#define FILE_SYSTEM_LITTLEFS        0x02

#define GFX4d_FILE_WRITE            0x14 // unused

#define DISP_INTERFACE_RGB          0
#define DISP_INTERFACE_SPI          1
#define DISP_INTERFACE_QSPI         2

#define DISP_TOUCH_NORMAL           0
#define DISP_TOUCH_INVERT_Y         1
#define DISP_TOUCH_INVERT_X         2
#define DISP_TOUCH_INVERT_XY        3

#define DISP_TOUCH_NONE             0x00
#define DISP_TOUCH_CTP              0x01
#define DISP_TOUCH_RTP              0x02

#define DISP_CTRL_RE_INIT           0x00
#define DISP_CTRL_RESET             0x01
#define DISP_CTRL_NEW               0x02
#define DISP_CTRL_INIT              0x03
#define DISP_CTRL_STOP              0x04
#define DISP_CTRL_START_TX          0x05
#define DISP_CTRL_DEL               0x06
#define DISP_CTRL_START             0x07
#define DISP_CTRL_FLUSH             0x08
#define DISP_CTRL_RESTART_TX        0x09
#define DISP_CTRL_RESET_ON_VSYNC    0x0a
#define DISP_CTRL_SET_CLOCK         0x0b
#define DISP_CTRL_RESTART_DISPLAY   0x0c
#define DISP_CTRL_FILL_BOUNCE_BUFFER 0x0d

#define DISPLAY_BUS_SPI             0x01
#define DISPLAY_BUS_QSPI            0x02
#define DISPLAY_BUS_RGB             0x03

#define GCI_IMAGE                   0xf0000UL
#define FRAMEBUFFER_IMAGE           0xf00000UL

#define SetGRAM                     setGRAM
#define startWrite                  StartWrite
#define endWrite                    EndWrite

#define setRotation                 Orientation

static const uint8_t font1[] = {
#include "System_FONT1.h"
};

static const uint8_t font2[] = {
#include "System_FONT2.h"
};

static const uint16_t RGB3322565[] = {
    0x0000, 0x000a, 0x0015, 0x001f, 0x0120, 0x012a, 0x0135, 0x013f, 0x0240,
    0x024a, 0x0255, 0x025f, 0x0360, 0x036a, 0x0375, 0x037f, 0x0480, 0x048a,
    0x0495, 0x049f, 0x05a0, 0x05aa, 0x05b5, 0x05bf, 0x06c0, 0x06ca, 0x06d5,
    0x06df, 0x07e0, 0x07ea, 0x07f5, 0x07ff, 0x2000, 0x200a, 0x2015, 0x201f,
    0x2120, 0x212a, 0x2135, 0x213f, 0x2240, 0x224a, 0x2255, 0x225f, 0x2360,
    0x236a, 0x2375, 0x237f, 0x2480, 0x248a, 0x2495, 0x249f, 0x25a0, 0x25aa,
    0x25b5, 0x25bf, 0x26c0, 0x26ca, 0x26d5, 0x26df, 0x27e0, 0x27ea, 0x27f5,
    0x27ff, 0x4800, 0x480a, 0x4815, 0x481f, 0x4920, 0x492a, 0x4935, 0x493f,
    0x4a40, 0x4a4a, 0x4a55, 0x4a5f, 0x4b60, 0x4b6a, 0x4b75, 0x4b7f, 0x4c80,
    0x4c8a, 0x4c95, 0x4c9f, 0x4da0, 0x4daa, 0x4db5, 0x4dbf, 0x4ec0, 0x4eca,
    0x4ed5, 0x4edf, 0x4fe0, 0x4fea, 0x4ff5, 0x4fff, 0x6800, 0x680a, 0x6815,
    0x681f, 0x6920, 0x692a, 0x6935, 0x693f, 0x6a40, 0x6a4a, 0x6a55, 0x6a5f,
    0x6b60, 0x6b6a, 0x6b75, 0x6b7f, 0x6c80, 0x6c8a, 0x6c95, 0x6c9f, 0x6da0,
    0x6daa, 0x6db5, 0x6dbf, 0x6ec0, 0x6eca, 0x6ed5, 0x6edf, 0x6fe0, 0x6fea,
    0x6ff5, 0x6fff, 0x9000, 0x900a, 0x9015, 0x901f, 0x9120, 0x912a, 0x9135,
    0x913f, 0x9240, 0x924a, 0x9255, 0x925f, 0x9360, 0x936a, 0x9375, 0x937f,
    0x9480, 0x948a, 0x9495, 0x949f, 0x95a0, 0x95aa, 0x95b5, 0x95bf, 0x96c0,
    0x96ca, 0x96d5, 0x96df, 0x97e0, 0x97ea, 0x97f5, 0x97ff, 0xb000, 0xb00a,
    0xb015, 0xb01f, 0xb120, 0xb12a, 0xb135, 0xb13f, 0xb240, 0xb24a, 0xb255,
    0xb25f, 0xb360, 0xb36a, 0xb375, 0xb37f, 0xb480, 0xb48a, 0xb495, 0xb49f,
    0xb5a0, 0xb5aa, 0xb5b5, 0xb5bf, 0xb6c0, 0xb6ca, 0xb6d5, 0xb6df, 0xb7e0,
    0xb7ea, 0xb7f5, 0xb7ff, 0xd800, 0xd80a, 0xd815, 0xd81f, 0xd920, 0xd92a,
    0xd935, 0xd93f, 0xda40, 0xda4a, 0xda55, 0xda5f, 0xdb60, 0xdb6a, 0xdb75,
    0xdb7f, 0xdc80, 0xdc8a, 0xdc95, 0xdc9f, 0xdda0, 0xddaa, 0xddb5, 0xddbf,
    0xdec0, 0xdeca, 0xded5, 0xdedf, 0xdfe0, 0xdfea, 0xdff5, 0xdfff, 0xf800,
    0xf80a, 0xf815, 0xf81f, 0xf920, 0xf92a, 0xf935, 0xf93f, 0xfa40, 0xfa4a,
    0xfa55, 0xfa5f, 0xfb60, 0xfb6a, 0xfb75, 0xfb7f, 0xfc80, 0xfc8a, 0xfc95,
    0xfc9f, 0xfda0, 0xfdaa, 0xfdb5, 0xfdbf, 0xfec0, 0xfeca, 0xfed5, 0xfedf,
    0xffe0, 0xffea, 0xfff5, 0xffff
};

class gfx4desp32 : public Print {

private:
    const char* TAG = "gfx4desp32";
    uint16_t width;
    uint16_t height;
    bool sdok;
    int _nlh;
    int16_t scrollAreaX0;
    int16_t scrollAreaY0;
    int16_t scrollAreaX1;
    int16_t scrollAreaY1;
    uint8_t ssSpeed;
    int8_t lastfsh;
    int8_t lastfsw;
    uint16_t tdark;
    uint16_t tlight;
    bool DISPtype = false;
    uint16_t Piwidth;
    uint16_t Piheight;
    int16_t cacheYpos;
    int16_t cacheHeight;
    int16_t cacheWidth;
    int16_t cacheiheight;
    int16_t cacheiypos;
    int16_t cachedYpos;
    int16_t lastcacheYpos;
    int16_t lastcachedYpos;
    int16_t lastcacheHeight;
    char empty[2] = { 0, 0 };
    void TWstring2write(String istr);
    int __gciCharWidth(char ch);

    void __tempFont(int8_t f);
    void __tempFont(const uint8_t* f, bool compressed);
    void __tempFont(gfx4d_font f);
    void __restoreFont();

    // font backups
    gfx4d_font gciFontBkup;
    const uint8_t* fontPtrBkup;
    bool fntCmprsBkup;
    int8_t fnoBkup;

protected:
#ifndef USE_LITTLEFS_FILE_SYSTEM
#ifndef USE_SDMMC_FILE_SYSTEM
    static SdFat uSD;
#endif
#endif
    esp_lcd_panel_handle_t panel_handle = NULL;
    gfx4desp32_backlight_config bk_config = {
        .pin = DEFAULT_PIN_NUM_BK_LIGHT,
        .on_level = DEFAULT_BK_LIGHT_ON_LEVEL,
        .off_level = DEFAULT_BK_LIGHT_OFF_LEVEL
    };

    int8_t GCItype = GCI_SYSTEM_USD;
    uint8_t DisplayType;
    uint8_t __TImode;

    int sd_sck;
    int sd_miso;
    int sd_mosi;
    int sd_cs;

    uint32_t datArrayPos;
    uint32_t gciArrayPos;
    bool gcidatArray;
    size_t datArraySize;
    size_t gciArraySize;

    uint32_t* tuiIndex;
    int16_t* tuix;
    int16_t* tuiy;
    int16_t* tuiw;
    int16_t* tuih;
    uint16_t* tuiImageIndex;
    uint16_t* gciobjframes;
    uint16_t* tuiExtra1;
    uint16_t* tuiExtra2;
    uint8_t* cdv;
    int16_t MAX_WIDGETS = 600;
    uint16_t opgfx = 0;

    String dat4d;
    String gci4d;
    int16_t xic;
    int16_t yic;
    uint16_t gciobjnum;
    uint8_t buff[SD_BUFF_SIZE + 100];
    bool sEnable;
    bool uimage;
    bool wrap;
    bool tchen;
    bool dlok;
    int16_t h, l, s;
    uint8_t fstyle;
    uint8_t rotation;
    uint8_t textsizeht;
    uint8_t textsize;
    uint8_t lastsizeht;
    bool nl;
    bool scrolled;
    int16_t cursor_x, cursor_y;
    uint16_t textcolor, textbgcolor;
    bool opacitystate;
    uint16_t savedbgcolor;
    uint16_t txtf;
    uint16_t txtb;
    uint16_t txtx;
    uint16_t txty;
    uint16_t txtw;
    uint16_t txth;
    uint16_t twcurx;
    uint16_t twcury;

    uint16_t bposx[256];
    uint16_t bposy[256];
    uint16_t bposw[256];
    uint16_t bposh[256];
    uint16_t bposc[256];

    uint16_t* txfcol;
    int16_t* spriteData;
    int16_t* spriteList;
    int16_t* spriteLast;
    int16_t* spriteNum;
    int16_t sdetaila;
    int16_t sdetaily;
    int16_t sdetailh;
    int16_t sdetailx;
    int16_t sdetailw;
    uint16_t spriteArea[4];
    uint16_t numSprites;
    uint8_t msprites;
    bool saSet;
    bool gciImagesUsed;

    uint8_t bstat[256];
    bool ButtonxInitial = true;

    bool bactive[256];
    uint8_t bxStyle;
    int16_t TWimage;
    int16_t TWimageCount;
    uint16_t chracc, chrdwn;
    uint16_t TWimgSet = 0;
    uint16_t TWimageTextCol;
    uint16_t twframecol;
    bool twframe;
    uint8_t* txtbuf;
    uint8_t TWcharByte;
    uint8_t TWcharBit;
    String cmdtxt;
    String twtext;
    bool txtwin;
    uint8_t twxpos;
    uint8_t twypos;
    uint8_t twcl;
    uint8_t UIDRcharOn;
    bool TWInitial = true;

    bool decodeKP;
    uint32_t DRcache = 2048 * 1000;
    int g2;

    uint8_t at[102] = {
        0,  1,  1,  2,  2,  3,  3,  4,  5,  5,  6,  6,  7,  7,  8,
        9,  9,  10, 10, 11, 11, 12, 12, 13, 13, 14, 15, 15, 16, 16,
        17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24,
        24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29, 30, 30, 31,
        31, 31, 32, 32, 33, 33, 33, 34, 34, 35, 35, 35, 36, 36, 37,
        37, 37, 38, 38, 38, 39, 39, 39, 40, 40, 40, 41, 41, 41, 42,
        42, 42, 43, 43, 43, 44, 44, 44, 44, 45, 45
    };

public:
    File dataFile;
    File dataFileDI;
    File userImag;
    File userDat;
    File Dwnload;

    gfx4desp32();
    ~gfx4desp32();
#ifndef USE_LITTLEFS_FILE_SYSTEM
#ifndef USE_SDMMC_FILE_SYSTEM
    static SdFat& getSdFatInstance();
#endif
#endif
    virtual esp_lcd_panel_handle_t __begin() = 0;
    virtual void DisplayControl(uint8_t cmd) = 0;
    virtual void RectangleFilled(int x1, int y1, int x2, int y2,
        uint16_t color) = 0;
    virtual void Vline(int16_t x, int16_t y, int16_t w, uint16_t color) = 0;
    virtual void Hline(int16_t x, int16_t y, int16_t w, uint16_t hcolor) = 0;
    virtual void SetGRAM(int16_t x1, int16_t y1, int16_t x2, int16_t y2) = 0;
    virtual void WrGRAMs(uint8_t* color_data, uint32_t len) = 0;
    virtual void WrGRAMs(uint16_t* color_data, uint32_t len) = 0;
    virtual void WrGRAMs(uint32_t* color_data, uint16_t len) = 0;
    virtual void WrGRAM(uint16_t color) = 0;
    virtual void WrGRAMs(const uint8_t* color_data, uint32_t len) = 0;
    virtual void pushColors(uint16_t* color_data, uint32_t len) = 0;
    virtual void pushColors(uint8_t* color_data, uint32_t len) = 0;
    virtual void pushColors(const uint8_t* color_data, uint32_t len) = 0;
    virtual bool StartWrite() = 0;
    virtual void EndWrite() = 0;
    virtual void _ScrollEnable(bool scrEn) = 0;
    virtual void setScrollArea(int y1, int y2) = 0;
    virtual void Scroll(int steps) = 0;
    virtual void setScrollBlankingColor(int32_t scolor) = 0;
    virtual void SmoothScrollSpeed(uint8_t sspeed) = 0;
    virtual int16_t getScrollareaY1() = 0;
    virtual void PutPixel(int16_t x, int16_t y, uint16_t color) = 0;
    virtual void panelOrientation(uint8_t r) = 0;
    virtual uint8_t getPanelOrientation(void) = 0;
    virtual int16_t getHeight(void) = 0;
    virtual int16_t getWidth(void) = 0;
    virtual void FillScreen(uint16_t color) = 0;
    virtual void BacklightOn(bool blight) = 0;
    virtual void Contrast(int cntrst) = 0;
    virtual void Invert(bool Inv) = 0;
    virtual void Transparency(bool trans) = 0;
    virtual void TransparentColor(uint16_t color) = 0;
    virtual void AlphaBlend(bool alphablend) = 0;
    virtual void AlphaBlendLevel(uint32_t alphaLevel) = 0;
    virtual uint16_t ReadPixel(uint16_t xrp, uint16_t yrp) = 0;
    virtual uint16_t ReadPixelFromFrameBuffer(uint16_t xrp, uint16_t yrp, uint8_t fb) = 0;
    virtual uint16_t ReadLine(int16_t x, int16_t y, int16_t w, uint16_t* data) = 0;
    virtual void WriteLine(int16_t x, int16_t y, int16_t w, uint16_t* data) = 0;
    virtual void DrawFrameBuffer(uint8_t fbnum) = 0;
    virtual void DrawFrameBufferArea(uint8_t fbnum, int16_t ui) = 0;
    virtual void DrawFrameBufferArea(uint8_t fbnum, int16_t x1, int16_t y1, int16_t x2, int16_t y2) = 0;
    virtual void MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1, uint8_t fbfrom2, uint16_t transColor) = 0;
    virtual void MergeFrameBuffers(uint8_t fbto, uint8_t fbfrom1, uint8_t fbfrom2, uint8_t fbfrom3, uint16_t transColor, uint16_t transColor1) = 0;
    // virtual void drawBitmap(int x1, int y1, int x2, int y2, uint16_t* c_data) = 0;
    virtual void CopyFrameBuffer(uint8_t fbto, uint8_t fbfrom1) = 0;
    virtual void CopyFrameBufferLine(int16_t x, int16_t y, int16_t w, int fb) = 0;
    void CopyImageLine(int inum, int x, int y, int w);
    virtual void PinMode(byte pin, uint8_t mode) = 0;
    virtual void DigitalWrite(byte pin, bool state) = 0;
    virtual int DigitalRead(byte pin) = 0;
    virtual void FlushArea(int x1, int x2, int y1, int y2, int xpos) = 0;
    virtual void WriteToFrameBuffer(uint32_t offset, uint8_t* data, uint32_t len) = 0;
    virtual void WriteToFrameBuffer(uint32_t offset, uint16_t* data, uint32_t len) = 0;
    virtual void AllocateFB(uint8_t sel) = 0;
    virtual void AllocateDRcache(uint32_t cacheSize) = 0;
    void Cls();
    void Cls(uint16_t color);
    void begin(String ips = DEFAULT_DISPLAY, int pval = 0, bool backightStartOn = true);
    void begin(String ips);
    void begin(int pval);
    void begin(bool backLightStartOn);
    void begin(String ips, int pval);
    void begin(String ips, bool backLightStartOn);
    void begin(int pval, bool backLightStartOn);
    void ScrollEnable(bool sEn);
    int16_t getX(void);
    int16_t getY(void);
    void PutPixelAlpha(int x, int y, int32_t color, uint8_t alpha);
    void CircleFilled(int16_t xc, int16_t yc, int16_t r, uint16_t color);
    void Circle(int16_t xc, int16_t yc, int16_t r, uint16_t color);
    void Ellipse(int16_t xe, int16_t ye, int16_t radx, int16_t rady, uint16_t color);
    void EllipseFilled(int16_t xe, int16_t ye, int16_t radx, int16_t rady, uint16_t color);
    void ArcFilled(int16_t xa, int16_t ya, int16_t r, int16_t sa, int16_t ea, uint16_t color);
    void Rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void Orientation(uint8_t r);
    uint8_t Orientation();
    void RoundRectFilled(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void RoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color);
    void Arc(int16_t x0, int16_t y0, int16_t r, uint16_t sa, uint16_t color);
    void Triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void TriangleFilled(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint16_t c);
    void Line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void setGCIsystem(uint8_t gs);
    uint8_t getGCIsystem();
    virtual void Open4dGFX(String file4d);
    void Open4dGFX(const uint8_t* DATa, uint32_t DATlen, const uint8_t* GCIa, uint32_t GCIlen);
    gfx4d_font Open4dFont(String font4d);
    void SetMaxWidgets(int mw);
    virtual void _Open4dGFX(String file4d, bool scan);
    uint16_t getWidgetNumFrames(int widget);
    uint32_t getIndexfromString(String strval, uint8_t indx);
    uint32_t getCoordfromString(String strval, uint8_t indx);
    virtual void Close4dGFX();
    void DrawWidget(uint32_t Index, int16_t uix, int16_t uiy, int16_t uiw, int16_t uih, uint16_t frame, int16_t bar, bool images, byte cdv);
    void UserImage(uint16_t ui);
    void UserImage(uint16_t ui, int altx, int alty);
    virtual void UserImages(uint16_t uisnb, int16_t framenb);
    void UserImages(uint16_t uis, int16_t frame, int offset, int16_t altx, int16_t alty);
    void UserImages(uint16_t uisnb, int16_t framenb, int16_t newx, int16_t newy);
    void UserImages(uint16_t uis, int16_t frame, int offset);
    void UserImages2image(int16_t ui, int16_t val, int16_t range, bool orientation, int16_t gap1, int16_t gap2);
    void UserImages3image(int16_t ui, int16_t val, int16_t range, bool orientation, int16_t gap1, int16_t gap2, int32_t tc);
    void outofrange(int16_t euix, int16_t euiy, int16_t euiw, int16_t euih);
    void LedDigitsDisplaySigned(int64_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks);
    void LedDigitsDisplaySigned(int64_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks, int16_t altx, int16_t alty);
    void LedDigitsDisplay(int64_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks);
    void LedDigitsDisplay(int64_t newval, uint16_t index, int16_t Digits, int16_t MinDigits, int16_t WidthDigit, int16_t LeadingBlanks, int16_t altx, int16_t alty);
    void PrintImage(uint16_t ui);
    void UserImagesDR(uint16_t uino, int frames, int16_t uxpos, int16_t uypos, int16_t uwidth, int16_t uheight);
    void UserImageDR(uint16_t ui, int16_t uxpos, int16_t uypos, int16_t uwidth, int16_t uheight, int16_t uix, int16_t uiy);
    void UserImageDRcache(uint16_t ui, int16_t uxpos, int16_t uypos, int16_t uwidth, int16_t uheight, int16_t uix, int16_t uiy);
    virtual size_t write(uint8_t) override;
    void newLine(int8_t f1, int8_t ts, uint16_t ux);
    void drawChar1(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t sizew, uint8_t sizeht);
    void drawChar2(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t sizew, uint8_t sizeht);
    void drawChar4D(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t sizew, uint8_t sizeht);
    void drawChar4Dcmp(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t sizew, uint8_t sizeht);
    void MoveTo(int16_t x, int16_t y);
    int8_t Font(void);
    void Font(uint8_t f);
    void Font(gfx4d_font f);
    void Font(const uint8_t* f, bool compressed = false);
    uint32_t bevelColor(uint16_t colorb);
    uint32_t HighlightColors(uint16_t colorh, int step);
    uint16_t RGBs2COL(uint8_t r, uint8_t g, uint8_t b);
    void HLS2RGB(int H, int L, int S);
    uint8_t hue_RGB(int Hin, int M1, int M2);
    void c565toRGBs(uint16_t i565);
    uint16_t RGBto565(uint8_t rc, uint8_t gc, uint8_t bc);
    void RGB2HLS();
    void TextSize(uint8_t s);
    void TextColor(uint16_t c);
    void TextColor(uint16_t c, uint16_t b);
    void TextWrap(boolean w);
    int8_t FontHeight(void);
    void ButtonXstyle(byte bs);
    void drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t colorb, String btext, int8_t tfont, int8_t tfontsize,
        int8_t tfontsizeht, uint16_t tcolor);
    void drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t colorb, String btext, const uint8_t* tfont,
        int8_t tfontsize, int8_t tfontsizeht, uint16_t tcolor,
        bool compressed = false);
    void drawButton(uint8_t updn, int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t colorb, String btext, gfx4d_font tfont,
        int8_t tfontsize, int8_t tfontsizeht, uint16_t tcolor);
    void Slider(uint8_t state, int16_t x, int16_t y, int16_t r, int16_t b,
        uint16_t colorb, uint16_t colort, int16_t scale, int16_t value);
    void Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb,
        uint16_t tcolor, int8_t tfont, int8_t tfontsizeh,
        int8_t tfontsize, String btext);
    void Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb,
        uint16_t tcolor, const uint8_t* tfont, int8_t tfontsizeh,
        int8_t tfontsize, String btext, bool compressed = false);
    void Button(uint8_t state, int16_t x, int16_t y, uint16_t colorb,
        uint16_t tcolor, gfx4d_font tfont, int8_t tfontsizeh,
        int8_t tfontsize, String btext);
    void Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t colorb, String btext, int8_t tfont, uint16_t tcolor);
    void Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t colorb, String btext, const uint8_t* tfont,
        uint16_t tcolor, bool compressed = false);
    void Buttonx(uint8_t hndl, int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t colorb, String btext, gfx4d_font tfont,
        uint16_t tcolor);
    void GradTriangleFilled(int x0, int y0, int x1, int y1, int x2, int y2,
        int color, int ncCol, int h, int ypos, int lev,
        int erase);
    void HlineD(int y, int x1, int x2, uint16_t color);
    void VlineD(int x, int y1, int y2, uint16_t color);
    void gradientShape(int vert, int ow, int xPos, int yPos, int w, int h, int r1,
        int r2, int r3, int r4, int darken, uint16_t color,
        int sr1, int gl1, uint16_t colorD, int sr3, int gl3,
        int gtb);
    uint16_t Grad(int GraisSunk, int Gstate, int Gglev, int Gh1, int Gpos,
        uint16_t colToAdj);
    void drawChar2tw(int16_t x, int16_t y, unsigned char c, uint16_t color,
        uint16_t bg, uint8_t size);
    void drawChar1tw(int16_t x, int16_t y, unsigned char c, uint16_t color,
        uint16_t bg, uint8_t size);
    void TWprintln(String istr);
    void TWprintln(char* istr);
    void TWprintln(int8_t istr);
    void TWprintln(uint8_t istr);
    void TWprintln(int16_t istr);
    void TWprintln(uint16_t istr);
    void TWprintln(int32_t istr);
    void TWprintln(uint32_t istr);
    void TWprintln(int64_t istr);
    void TWprintln(uint64_t istr);
    void TWprintln(float istr);
    void TWprint(String istr);
    void TWprint(char* istr);
    void TWprint(int8_t istr);
    void TWprint(uint8_t istr);
    void TWprint(int16_t istr);
    void TWprint(uint16_t istr);
    void TWprint(int32_t istr);
    void TWprint(uint32_t istr);
    void TWprint(int64_t istr);
    void TWprint(uint64_t istr);
    void TWprint(float istr);
    String GetCommand();
    void TWtextcolor(uint16_t twc);
    boolean TWMoveTo(uint8_t twcrx, uint8_t twcry);
    void TWprintAt(uint8_t pax, uint8_t pay, String istr);
    void TWwrite(const char txtinput);
    void TWcursorOn(bool twco);
    void TWcls();
    void TWcolor(uint16_t fcol);
    void TWcolor(uint16_t fcol, uint16_t bcol);
    void TextWindowImage(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t txtcolor, uint16_t TWimg, uint16_t frcolor);
    void TextWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t txtcolor,
        uint16_t txbcolor, uint16_t frcolor);
    void TWenable(boolean t);
    void TextWindowRestore();
    void TextWindow(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t txtcolor,
        uint16_t txbcolor);
    void drawChar2TWimage(uint8_t ch, uint16_t uino, int frames, int16_t uxpos,
        int16_t uypos, uint16_t txtcol);
    void Panel(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t colorp);
    void PanelRecessed(int16_t x, int16_t y, int16_t w, int16_t h,
        uint16_t colorpr);
    uint16_t getNumberofObjects(void);
    void ButtonUp(int hndl);
    void ButtonDown(int hndl);
    void ButtonActive(uint8_t butno, boolean act);
    void FontStyle(uint8_t ctyp);
    void DeleteButton(int hndl, uint16_t color);
    void DeleteButton(int hndl);
    void DeleteButtonBG(int hndl, int objBG);
    void setAddrWindow(int16_t x1, int16_t y1, int16_t w, int16_t h);
    void Orbit(int angle, int lngth, int* oxy);
    void Orbit(float angle, float lngth, float* oxy);
    void UserImageHide(int hndl);
    virtual void UserImageHide(int hndl, uint16_t color);
    virtual void UserImageHideBG(int hndl, int objBG);
    bool ScreenCapture(int16_t x, int16_t y, int16_t w, int16_t h, String fname);
    void DrawToframebuffer(uint8_t fbnum);
    void SpriteAreaSet(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1);
    void SetSprite(int num, bool active, int x, int y, uint16_t bscolor,
        uint16_t* sdata);
    void UpdateSprites(uint16_t bscolor, uint16_t* sdata);
    void SetNumberSprites(uint16_t numspr);
    void SpriteEnable(int snum, bool sen);
    void SpriteUpdate(int16_t tsx, int16_t tsy, int16_t tsx1, int16_t tsy1,
        uint16_t bscolor, uint16_t* sdata);
    void SetMaxNumberSprites(uint16_t snos);
    bool SpriteInit(uint16_t* sdata, size_t nums);
    bool SpriteAdd(int pos, int snum, int x, int y, uint16_t* sdata);
    uint16_t SpriteGetPixel(int snum, int xo, int yo, uint16_t tcolor,
        uint16_t* sdata);
    uint16_t SpriteGetPalette(int pnumber);
    int GetSpritesAt(int xo, int yo, uint16_t tcolor, int* slist,
        uint16_t* sdata);
    int GetSprite(int snum, int choice);
    int GetNumberSprites();
    int16_t GetSpriteImageNum(int snum);
    void SpriteSetPalette(int pnumber, uint16_t plcolor);
    int16_t XYposToDegree(int curX, int curY);
    void Opacity(bool opacity);
    void BGcolour(uint16_t c);
    void FGcolour(uint16_t c);
    void putstr(String strg);
    void putstr(const char* strg);
    void putstrXY(int xpos, int ypos, String strg);
    void putstrXY(int xpos, int ypos, const char* strg);
    void putch(char chr);
    void putchXY(int xpos, int ypos, char chr);
    int charWidth(char ch);
    int charHeight(char ch);
    int strWidth(String ts);
    int strWidth(char* ts);
    int imageGetWord(uint16_t img, byte controlIndex);
    void imageSetWord(uint16_t img, byte controlIndex, int val1, int val2 = 0);
    int16_t imageAutoSlider(uint16_t ui, uint8_t axis, uint16_t uiv,
        uint16_t ming, uint16_t maxg);
    int16_t imageAutoKnob(int hndl, int uix, int uiy, int minarc, int maxarc,
        int ming, int maxg);
    virtual int16_t getImageValue(uint16_t ui);
    void DownloadFile(String WebAddr, String Fname);
    void DownloadFile(String WebAddr, String Fname, const char* sha1);
    void DownloadFile(String Address, uint16_t port, String hfile, String Fname);
    void Download(String Address, uint16_t port, String hfile, String Fname,
        bool certUsed, const char* sha1);
    void PrintImageFile(String ifile);
    void UserCharacter(uint32_t* data, uint8_t ucsize, int16_t ucx, int16_t ucy,
        uint16_t color, uint16_t bgcolor);
    void UserCharacterBG(uint32_t* data, uint8_t ucsize, int16_t ucx, int16_t ucy,
        uint16_t color, boolean draw, uint32_t bgindex);
    void UserCharacterBG(int8_t ui, uint32_t* data, uint8_t ucsize, int16_t ucx,
        int16_t ucy, uint16_t color, boolean draw);
    void UserCharacterBG(uint32_t* data, uint8_t ucsize, int16_t ucx, int16_t ucy,
        uint16_t color, boolean draw, uint32_t bgindex,
        bool type, int8_t ui);
    bool CheckSD(void);
    bool CheckDL(void);
    void DrawImageFile(String Fname);
    void DrawImageFile(const char* Fname);
    void DrawImageArray(uint8_t* ImageArray);
    void DrawImageArray(uint16_t* ImageArray);
    void GCIreadToBuff(uint32_t Index, uint32_t len);
    void GCIreadToBuff2(uint32_t Index, uint32_t len);
    void GCIreadToBuff2(uint32_t Index, uint32_t pos, uint32_t len);
    void GCIreadToBuf(uint32_t Index, uint32_t len);
    void GCIread(uint8_t* dest, uint32_t len);
    void DATAreadToBuff(uint32_t Index, uint32_t len);
    void DATAread(uint8_t* dest, uint32_t len);
    int16_t GCIread();
    int16_t DATread();
    int16_t DATAread();
    void GCIseek(uint32_t Index);
    void DATAseek(uint32_t Index);
    uint32_t DATAsize();
    void setCacheSize(uint32_t cs);
    int16_t getLastPointerPos();
    uint8_t GetFrameBuffer();
    uint16_t ReadImagePixel(int inum, int x, int y);
    void HlineX(int x, int y, int w, int32_t color);
    void VlineX(int x, int y, int w, int32_t color);
    void RectangleFilledX(int x0, int y0, int x1, int y1, int32_t color);

    uint8_t GFX4dESP32_RED;
    uint8_t GFX4dESP32_BLUE;
    uint8_t GFX4dESP32_GREEN;
    uint16_t palette4bit[16] = { BLACK, MAROON,  GREEN, OLIVE, NAVY, PURPLE,
                                TEAL,  SILVER,  GRAY,  RED,   LIME, YELLOW,
                                BLUE,  FUCHSIA, AQUA,  WHITE };
    bool twcurson;
    bool shift;
    bool caps;
    bool ctrl;
    bool ddos;
    bool twen;
    int8_t fsh;
    int8_t fsh1;
    uint16_t fsb; // number of bytes per GCI character
    uint16_t fsc; // last character in GCI font
    uint8_t fso;
    int8_t fsw;
    int8_t fno;
    gfx4d_font gciFont;
    const uint8_t* fontPtr = NULL;
    const uint8_t* fntWidths = NULL;
    const uint8_t* fntData = NULL;
    uint16_t fntCharCount;
    bool fntCmprs = false;
    uint16_t twcolnum;
    int GSCropArcLeft = -1;
    int GSCropArcRight = -1;
    int xpGSaPos[16];
    int ypGSaPos[16];
    int radsGS[16];
    int GSCropLastVal;
    int lastAsize;
    int keepLastArc = 0;
    int protectLA = 0;
    int GSErase;
    int GSEraseXpos;
    int GSEraseYpos;
    int GSEraseHeight;
    int GSErasePHeight;
    int GSEraseColour;
    int GSEraseGLevel;
    int GSERaisedSunk;
    int GSSLastSliderVal;
    int GSSBGColor;
    int GSSArconly;
    int GSSsxpos = -9999;
    int lastArcOld[MAX_ARCSIZE];
    int inx[MAX_ARCSIZE];
    uint16_t oldgTX, oldgTY, oldgPEN;
    uint16_t gTX, gTY, gPEN;
    uint8_t* fb;
    uint8_t* psRAMbuffer1;
    uint8_t* psRAMbuffer2;
    uint8_t* psRAMbuffer3;
    uint8_t* psRAMbuffer4;
    uint8_t* psRAMbuffer5;
    uint8_t* psRAMbuffer6;
    uint8_t* workbuffer;
    uint32_t screenArea;
    bool cache_Enabled;
    uint32_t cache_Start;
    uint32_t cache_End;
    uint32_t cache_pos;
    bool cached;
    int lastui;
    uint8_t frame_buffer;
    bool framebufferInit1;
    bool framebufferInit2;
    bool framebufferInit3;
    bool framebufferInit4;
    bool workbufferInit;
    bool IPS_Display = false;
    const uint8_t* DATarray = NULL;
    const uint8_t* GCIarray = NULL;
    int clipx1;
    int clipx2;
    int clipy1;
    int clipy2;
    bool transparency;
    bool alpha;
    bool transalpha;
    bool usePushColors;
    bool WriteFBonly;
    int16_t LastLinearPointerPosition;
    bool changePCLK;
    uint32_t PCLKval;
    uint8_t displayBus;
    bool bkStartOn;
    uint32_t __alpha;
    uint32_t __alphatemp;
    uint16_t __colour;
    uint16_t lbuff[800];
    int grad1[21];
    int grad2[7];
    uint8_t gradON;
    bool gradientVert;
};

#endif
