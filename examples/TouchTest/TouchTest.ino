//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//

// When using Arduino, you need to set the correct include file and class initialization to match your display.
// To save the hassle of doing this is manually, it is recommended to use Workshop4.

#include "gfx4desp32_gen4_ESP32_32CT_CLB.h"

gfx4desp32_gen4_ESP32_32CT_CLB gfx = gfx4desp32_gen4_ESP32_32CT_CLB();

uint16_t tx, lastTx;
uint16_t ty, lastTy;
int TOUCHED;
bool firstTouch = true;

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
  gfx.print("Draw something");
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
  gfx.touch_Update();
  tx = gfx.touch_GetX();
  ty = gfx.touch_GetY();
  TOUCHED = gfx.touch_GetPen();

  // If the screen gets touched
  if (TOUCHED == TOUCH_PRESSED)
  {
    // First touch of the screen, draw a pixel where it was touched
    if (firstTouch)
    {
      gfx.PutPixel(tx, ty, BLUE);
      firstTouch = false;
    }
    // Screen previously touched and pixel drawn, so draw a line from first touch to this location
    else
    {
      gfx.Line(lastTx, lastTy, tx, ty, BLUE);
    }
    // Save the current touch location so it can be referred to next time
    lastTx = tx;
    lastTy = ty;
  }

  // If the screen touch is released after previously touching it, draw a circle to mark the release point
  if (TOUCHED == NOTOUCH || TOUCHED == TOUCH_RELEASED)
  {
    gfx.CircleFilled(tx, ty, 3, RED);
    firstTouch = true;
  }
}

