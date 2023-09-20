#ifndef __GFX4D_TYPES__
#define __GFX4D_TYPES__

typedef enum {
    LANDSCAPE,
    LANDSCAPE_R,
    PORTRAIT,
    PORTRAIT_R
} gfx4desp32_orientation;

typedef struct {
    int pin;
    int on_level;
    int off_level;
} gfx4desp32_backlight_config;

typedef struct {
    int x;
    int y;
} gfx4desp32_position;

#define FONT_TYPE           0
#define FONT_CHAR_COUNT     1

#define FONT0_OFFSET        2
#define FONT0_WIDTH         3
#define FONT0_HEIGHT        4

#define FONT1_OFFSET        2
#define FONT1_WIDTH         3
#define FONT1_HEIGHT        4
#define FONT1_DATA          5

#define FONT2_OFFSET        2
#define FONT2_WIDTH         3
#define FONT2_HEIGHT        4
#define FONT2_WIDTH_TABLE   5

#define FONT3_CHAR_COUNT_H  2
#define FONT3_OFFSET        3
#define FONT3_WIDTH         4
#define FONT3_HEIGHT        5
#define FONT3_WIDTH_TABLE   6


#endif // __GFX4D_TYPES__