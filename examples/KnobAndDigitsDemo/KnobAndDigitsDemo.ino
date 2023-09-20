//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//

// When using Arduino, you need to set the correct include file and class initialization to match your display.
// To save the hassle of doing this is manually, it is recommended to use Workshop4.

#include "gfx4desp32_gen4_ESP32_32CT_CLB.h"

gfx4desp32_gen4_ESP32_32CT_CLB gfx = gfx4desp32_gen4_ESP32_32CT_CLB();

#include "KnobAndDigitsDemoConst.h"    // Note. This file will not be created if there are no generated graphics

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.Open4dGFX("KnobAndDigitsDemo"); // Opens DAT and GCI files for read using filename without extension.
  gfx.touch_Set(TOUCH_ENABLE);                // Global touch enabled
  gfx.imageTouchEnable(iKnob1, true) ;                       // init_Knob1 enable touch of widget (on Form1)
  gfx.UserImages(iKnob1,0) ;                                 // init_Knob1 show initially, if required (on Form1)
  gfx.UserImage(iLeddigits1);                                // init_Leddigits1 show all digits at 0, only do this once
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
  // put your main code here, to run repeatedly:
  int itouched, val ;
  if(gfx.touch_Update())
  {
    itouched = gfx.imageTouched() ;
    switch (itouched)
    {                                                         // start touched selection **do not alter, remove or duplicate this line**
      // case statements for Knobs and Sliders go here
      case iKnob1 :                                      // process_Knob1 process (on Form1)
        val = gfx.imageAutoKnob(iKnob1, gfx.touch_GetX(), gfx.touch_GetY(), 45, 315, 0, 100);
        // process knob based on val
        gfx.LedDigitsDisplay(val, iiLeddigits1, 3, 1, 46, 0) ;  // Leddigits1

        break ;
      default :                                               // end touched selection **do not alter, remove or duplicate this line**
        int button = gfx.ImageTouchedAuto();    // use default for keyboards and buttons
        val = gfx.getImageValue(button);
        switch (button)
        {                                                     // start button selection **do not alter, remove or duplicate this line**
          // case, one for each button or keyboard, default should end up as -1
        }                                                     // end button selection **do not alter, remove or duplicate this line**
    }
  }
}

