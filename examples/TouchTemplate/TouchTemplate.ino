//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//

// When using Arduino, you need to set the correct include file and class initialization to match your display.
// To save the hassle of doing this is manually, it is recommended to use Workshop4.

#include "gfx4desp32_gen4_ESP32_32CT_CLB.h"

gfx4desp32_gen4_ESP32_32CT_CLB gfx = gfx4desp32_gen4_ESP32_32CT_CLB();

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.touch_Set(TOUCH_ENABLE);                // Global touch enabled
}

void loop()
{
  // put your main code here, to run repeatedly:
  int itouched, x, y, pen;
  if(gfx.touch_Update())
  {
    pen = gfx.touch_GetPen();
    x = gfx.touch_GetX();
    y = gfx.touch_GetY();
    // Evaluate
  }
}

