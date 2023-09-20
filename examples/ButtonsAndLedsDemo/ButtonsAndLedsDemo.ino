//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//

// When using Arduino, you need to set the correct include file and class initialization to match your display.
// To save the hassle of doing this is manually, it is recommended to use Workshop4.

#include "gfx4desp32_gen4_ESP32_32CT_CLB.h"

gfx4desp32_gen4_ESP32_32CT_CLB gfx = gfx4desp32_gen4_ESP32_32CT_CLB();

#include "ButtonsAndLedsDemoConst.h"    // Note. This file will not be created if there are no generated graphics

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.Open4dGFX("ButtonsAndLedsDemo"); // Opens DAT and GCI files for read using filename without extension.
  gfx.touch_Set(TOUCH_ENABLE);                // Global touch enabled

  gfx.imageTouchEnable(i4Dbutton1, true, MOMENTARY) ;        // init_4Dbutton1 enable touch of widget (on Form1)
  gfx.UserImages(i4Dbutton1,0) ;                             // init_4Dbutton1 show initially, if required (on Form1)
  gfx.imageTouchEnable(i4Dbutton2, true, MOMENTARY) ;        // init_4Dbutton2 enable touch of widget (on Form1)
  gfx.UserImages(i4Dbutton2,0) ;                             // init_4Dbutton2 show initially, if required (on Form1)
  gfx.imageTouchEnable(i4Dbutton3, true, TOGGLE4STATES) ;    // init_4Dbutton3 enable touch of widget (on Form1)
  gfx.UserImages(i4Dbutton3,0) ;                             // init_4Dbutton3 show initially, if required (on Form1)
  gfx.imageTouchEnable(i4Dbutton4, true, TOGGLE4STATES) ;    // init_4Dbutton4 enable touch of widget (on Form1)
  gfx.UserImages(i4Dbutton4,2) ;                             // init_4Dbutton4 show initially in on state, if required (on Form1)
  gfx.UserImages(iUserled1,0) ;                              // init_Userled1 show initialy, if required
  gfx.UserImages(iUserled2,1) ;                              // init_Userled2 show initialy, if required
  gfx.UserImages(iUserled3,0) ;                              // init_Userled3 show initialy, if required
  gfx.UserImages(iUserled4,1) ;                              // init_Userled4 show initialy, if required, changed manually to 1 to match the partner button
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
  // put your main code here, to run repeatedly:

  static int iUserled1Val = 0;
  static int iUserled2Val = 1;

  int itouched, val ;
  if(gfx.touch_Update())
  {
    itouched = gfx.imageTouched() ;
    switch (itouched)
    {                                                         // start touched selection **do not alter, remove or duplicate this line**
      // case statements for Knobs and Sliders go here
      default :                                               // end touched selection **do not alter, remove or duplicate this line**
        int button = gfx.ImageTouchedAuto();    // use default for keyboards and buttons
        val = gfx.getImageValue(button);
        switch (button)
        {                                                     // start button selection **do not alter, remove or duplicate this line**

          case i4Dbutton1 :                                      // process_4Dbutton1 process Button (on Form1)
            // process 4dbutton, for momentary val can be ignored
            iUserled1Val = (iUserled1Val != 0) ? 0 : 1;
            gfx.UserImages(iUserled1, iUserled1Val) ;      // Userled1 where val is 0 (Off) or 1 (On)
            break ;

          case i4Dbutton2 :                                      // process_4Dbutton2 process Button (on Form1)
            // process 4dbutton, for momentary val can be ignored
            iUserled2Val = (iUserled2Val != 0) ? 0 : 1;
            gfx.UserImages(iUserled2, iUserled2Val) ;      // Userled1 where val is 0 (Off) or 1 (On)
            break ;

          case i4Dbutton3 :                                      // process_4Dbutton3 process Button (on Form1)
            // process 4dbutton, for toggle val will be 1 for down and 0 for up
            gfx.UserImages(iUserled3, val) ;      // Userled3 where val is 0 (Off) or 1 (On)
            break ;

          case i4Dbutton4 :                                      // process_4Dbutton4 process Button (on Form1)
            // process 4dbutton, for toggle val will be 1 for down and 0 for up
            gfx.UserImages(iUserled4, val) ;      // Userled4 where val is 0 (Off) or 1 (On)
            break ;
        }                                                     // end button selection **do not alter, remove or duplicate this line**
    }
  }
}
