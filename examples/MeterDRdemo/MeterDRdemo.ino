//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//
#include "gfx4desp32_gen4_ESP32_32CT_CLB.h"

gfx4desp32_gen4_ESP32_32CT_CLB gfx = gfx4desp32_gen4_ESP32_32CT_CLB();

#include "MeterDRdemoConst.h"    // Note. This file will not be created if there are no generated graphics

int value = 0;

int vdir = 0;
int buttw;
int but;
int lastbut;

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  gfx.Open4dGFX("MeterDRdemo"); // Opens DAT and GCI files for read using filename without extension.
  gfx.touch_Set(TOUCH_ENABLE);                // Global touch enabled

  gfx.UserImages(iAngularmeter1,0) ;                         // init_Angularmeter1 show initialy, if required
  gfx.UserImage(iLeddigits1);                                // init_Leddigits1 show all digits at 0, only do this once
  gfx.UserImages(iGauge1,0) ;                                // init_Gauge1 show initialy, if required

  gfx.imageTouchEnable(iWinbutton1, true, MOMENTARY) ;       // init_Winbutton1(Start Button) enable touch of widget (on Form1)
  gfx.UserImages(iWinbutton1,0) ;                            // init_Winbutton1(Start Button) show initially, if required (on Form1)
  gfx.imageTouchEnable(iWinbutton2, true, MOMENTARY) ;       // init_Winbutton2(Stop Button) enable touch of widget (on Form1)
  gfx.UserImages(iWinbutton2,0) ;                            // init_Winbutton2(Stop Button) show initially, if required (on Form1)
  gfx.imageTouchEnable(iWinbutton3, true, MOMENTARY) ;       // init_Winbutton3(Reverse Button) enable touch of widget (on Form1)
  gfx.UserImages(iWinbutton3,0) ;                            // init_Winbutton3(Reverse Button) show initially, if required (on Form1)
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
  // put your main code here, to run repeatedly:
  if (value > -1 && value < 19) {
    gfx.UserImagesDR(iAngularmeter1, value, 42, 104, 87, 80);  // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if (value > 18 && value < 51) {
    gfx.UserImagesDR(iAngularmeter1, value, 42, 38, 87, 90);  // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if (value > 50 && value < 82) {
    gfx.UserImagesDR(iAngularmeter1, value, 109, 38, 87, 90);  // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if (value > 81 && value < 101) {
    gfx.UserImagesDR(iAngularmeter1, value, 109, 104, 87, 80);  // where frame is 0 to 100 (for a displayed 0 to 100)
  }

  int intv = map(value, 0, 100, 0, 212);
  if (vdir == 1) {
    gfx.UserImagesDR(iGauge1, value, intv, 4, 8, 20);  // where frame is 0 to 100 (for a displayed 0 to 100)
  }
  if (vdir == -1) {
    gfx.UserImagesDR(iGauge1, value, intv, 4, 8, 20);  // where frame is 0 to 100 (for a displayed 0 to 100)
  }

  gfx.LedDigitsDisplay(value, iiLeddigits1, 3, 1, 30, 0) ;  // Leddigits1
  value = value + vdir;

  if (value > 100) {
    gfx.UserImages(iAngularmeter1, 0);  // Angularmeter1 show initialy, if required
    gfx.UserImages(iGauge1, 0);         // Gauge1 show initialy, if required
    gfx.UserImage(iLeddigits1);         // Leddigits1 show all digits at 0, only do this once
    value = 0;
  }
  if (value < 0) {
    gfx.UserImages(iAngularmeter1, 100);  // Angularmeter1 show initialy, if required
    gfx.UserImages(iGauge1, 100);         // Gauge1 show initialy, if required
    gfx.UserImage(iLeddigits1);           // Leddigits1 show all digits at 0, only do this once
    value = 100;
  }

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
          // case, one for each button or keyboard, default should end up as -1
            break ;
          case iWinbutton1 :                                      // process_Winbutton1(Start Button) process Button (on Form1)
            // process win button, for toggle val will be 1 for down and 0 for up
            vdir = 1;
            break ;
          case iWinbutton2 :                                      // process_Winbutton2(Stop Button) process Button (on Form1)
            // process win button, for toggle val will be 1 for down and 0 for up
            vdir = 0;
            break ;
          case iWinbutton3 :                                      // process_Winbutton3(Reverse Button) process Button (on Form1)
            // process win button, for toggle val will be 1 for down and 0 for up
            vdir = -1;
            break ;
        }                                                     // end button selection **do not alter, remove or duplicate this line**
    }
  }
}

