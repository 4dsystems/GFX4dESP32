//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//

// When using Arduino, you need to set the correct include file and class initialization to match your display.
// To save the hassle of doing this is manually, it is recommended to use Workshop4.

#include "gfx4desp32_gen4_ESP32_28.h"

gfx4desp32_gen4_ESP32_28 gfx = gfx4desp32_gen4_ESP32_28();

#include "SpectrumDemoConst.h"    // Note. This file will not be created if there are no generated graphics

// Uncomment if using GC* in program flash.
#include "SpectrumDemoGCx.h"     // Note. This file will be generated when graphics destination program flash

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(false);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK); gfx.Font(2);  gfx.TextSize(1);
  // Uncomment one of the three followin statements depending on the type of graphics file you are using GCI, GCJ, or GC* in program flash.
//gfx.Open4dGFX("SpectrumDemo"); // Opens DAT and GCI files for read using filename without extension.
//gfx.Open4dGFX("SpectrumDemo.gcj"); // Opens GCJ file for read using complete filename.
  gfx.Open4dGFX(SpectrumDemo, SpectrumDemo_size); // Opens GCJ from program space using array name and size
//gfx.Open4dGFX(SpectrumDemo_dat, SpectrumDemo_dat_size, SpectrumDemo_gci, SpectrumDemo_gci_size); // Opens DAT/GCI from program space using array name and size
  gfx.UserImage(iSpectrum1) ;                                // init_Spectrum1 show initial spectrum
} // end Setup **do not alter, remove or duplicate this line**

void loop()
{
  for (int bar = 0; bar <= 9; bar++) {
    int frame = random(100);
    gfx.UserImages(iiSpectrum1, frame, 5 + bar * 23) ; // where frame is 0 to 100 (for a displayed 0 to 100) and bar is 0 to 9
  }
}

