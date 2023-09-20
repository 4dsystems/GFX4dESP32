//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//

// When using Arduino, you need to set the correct include file and class initialization to match your display.
// To save the hassle of doing this is manually, it is recommended to use Workshop4.

#include "gfx4desp32_gen4_ESP32_32CT_CLB.h"

gfx4desp32_gen4_ESP32_32CT_CLB gfx = gfx4desp32_gen4_ESP32_32CT_CLB();

#include "SliderAndGaugeDRdemoConst.h"    // Note. This file will not be created if there are no generated graphics

int frame;
int posn;
int value;
int lastvalue;
int x;
int s = 18;
int oldposn;
int rposn;
int oldrposn;

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.Open4dGFX("SliderAndGaugeDRdemo"); // Opens DAT and GCI files for read using filename without extension.
  gfx.touch_Set(TOUCH_ENABLE);                // Global touch enabled
  gfx.imageTouchEnable(iSlider1, true) ;                     // init_Slider1 enable touch of widget (on Form1)
  gfx.UserImages(iSlider1,0) ;                               // init_Slider1 show initially, if required (on Form1)
  gfx.UserImages(iCoolgauge1,0) ;                            // init_Coolgauge1 show initialy, if required
} // end Setup **do not alter, remove or duplicate this line**

void loop() {
  gfx.touch_Update();
  x = gfx.touch_GetX();
  if (gfx.imageTouched() == iSlider1) {
    posn = x - 28;  // x - left - 8
    rposn = posn + 8;
    if (posn < 0)
      posn = 0;
    else if (posn > 189)  // width - 17)
      posn = 100;         // maxvalue-minvalue
    else
      posn = 100 * posn / 189;  // max-min - (max-min) * posn / (width-17)
    if (rposn > oldrposn) {
      int possize = rposn - oldrposn;
      if (oldrposn > 7) {
        gfx.UserImagesDR(iSlider1, posn, (oldrposn - (8 + (x / 100))), 3, (possize) + 22, 32);  // Draw The segment that has changed.
      } else {
        gfx.UserImagesDR(iSlider1, posn, oldrposn, 3, (possize) + 22, 32);  // Draw The segment that has changed.
      }
    }
    if (rposn < oldrposn) {
      int possize = oldrposn - rposn;
      if (rposn > 8) {
        gfx.UserImagesDR(iSlider1, posn, (rposn)-8, 3, (possize) + 22, 32);  // Draw The segment that has changed.
      } else {
        gfx.UserImagesDR(iSlider1, posn, (rposn), 3, (possize) + 22, 32);  // Draw The segment that has changed.
      }
    }
    oldposn = posn;
    oldrposn = rposn;
    if (value < posn) {
      if ((posn - value) > 5) {
        value = value + 3;
      } else {
        value = value + 1;
      }
    }
    if (value > posn) {
      if ((value - posn) > 5) {
        value = value - 3;
      } else {
        value = value - 1;
      }
    }
    int tvalue = value + 135;
    if (tvalue > 359) {
      tvalue = tvalue - 360;
    }
    // Calculate degrees of current value
    float i = (2.7 * value) + 135;
    if (i > 359) {
      i = i - 360;
    }
    // Get the pointer outer x & y and the pointer inner x & y
    int radiuso = 74;
    int radiusi = 25;
    float rads = i * PI / 180;
    float oxt = 105 + radiuso * cos(rads);
    float oyt = 105 + radiuso * sin(rads);
    float ixt = 105 + radiusi * cos(rads);
    float iyt = 105 + radiusi * sin(rads);
    int ox = oxt;
    int oy = oyt;
    int ix = ixt;
    int iy = iyt;
    // Check for rectangle co-ordinates relative to gauge pointer and resize rectangle
    if (ox < ix && oy < iy) {
      ox = ox - s;
      ix = ix + s;
      oy = oy - s;
      iy = iy + s;
      // Draw The segment that has changed.
      gfx.UserImagesDR(iCoolgauge1, value, ox, oy, ix - ox, iy - oy);
    }
    // Check for rectangle co-ordinates relative to gauge pointer and resize rectangle
    if (ox < ix && oy > iy) {
      ox = ox - s;
      ix = ix + s;
      oy = oy + s;
      iy = iy - s;
      // Draw The segment that has changed.
      gfx.UserImagesDR(iCoolgauge1, value, ox, iy, ix - ox, oy - iy);
    }
    // Check for rectangle co-ordinates relative to gauge pointer and resize rectangle
    if (ox > ix && oy < iy) {
      ox = ox + s;
      ix = ix - s;
      oy = oy - s;
      iy = iy + s;
      // Draw The segment that has changed.
      gfx.UserImagesDR(iCoolgauge1, value, ix, oy, ox - ix, iy - oy);
    }
    // Check for rectangle co-ordinates relative to gauge pointer and resize rectangle
    if (ox > ix && oy > iy) {
      ox = ox + s;
      ix = ix - s;
      oy = oy + s;
      iy = iy - s;
      // Draw The segment that has changed.
      gfx.UserImagesDR(iCoolgauge1, value, ix, iy, ox - ix, oy - iy);
    }
    lastvalue = value;
  }
  if (value < 100) {
    // Draw digit readout of gauge
    gfx.UserImagesDR(iCoolgauge1, value, 94, 162, 28, 28);
  } else {
    // Draw digit readout of gauge
    gfx.UserImagesDR(iCoolgauge1, value, 92, 162, 30, 28);
  }
}
