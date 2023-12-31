//
// NB! This is a file generated from the .4Dino file, changes will be lost
//     the next time the .4Dino file is built
//
/**************************************************************
* WiFiScan                                                    *
* A Demo to show all available access points with a signal    *
* strength meter.                                             *
* This demo shows the smooth scroll feature                   *
* This demo will not connect to the internet.                 *
*                                                             *
**************************************************************/
#include "WiFi.h"

// When using Arduino, you need to set the correct include file and class initialization to match your display.
// To save the hassle of doing this is manually, it is recommended to use Workshop4.

#include "gfx4desp32_gen4_ESP32_32.h"

gfx4desp32_gen4_ESP32_32 gfx = gfx4desp32_gen4_ESP32_32();

#include "WiFiScanConst.h"    // Note. This file will not be created if there are no generated graphics

void setup()
{
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(true);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE);
  gfx.Font(2);
  gfx.TextSize(1);
  //gfx.Open4dGFX("WiFiScan"); // Opens DAT and GCI files for read using filename without extension.

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  gfx.println("Setup done");
} // end Setup **do not alter, remove or duplicate this line**

void loop() {
  gfx.TextColor(ORANGE);
  gfx.println("Scanning ...");


  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();

  gfx.TextColor(GREEN);
  gfx.println("Finished Scan");
  if (n == 0) {
    gfx.TextColor(RED);
    gfx.println("no networks found");
  } else {
    gfx.TextColor(GREEN);
    gfx.print(n);
    gfx.println(" Networks found");
    for (int i = 0; i < n; ++i) {
      gfx.TextColor(ORANGE);
      gfx.print(i + 1);
      gfx.TextColor(WHITE);
      gfx.print(": ");
      gfx.TextColor(YELLOW);
      gfx.print(WiFi.SSID(i));

      sigStrength(219, gfx.getY(), RED, WiFi.RSSI(i));
      if (WiFi.encryptionType(i) != WIFI_AUTH_OPEN) {
        lock(205, gfx.getY(), RED);
      }
      gfx.println();
    }
  }
  gfx.println("");

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  delay(5000);
}

void sigStrength(uint16_t xpos, uint16_t ypos, uint16_t color, int sig) {
  for (int x = 0; x < 5; x++) {
    if (sig > ((5 - x) * -20)) {
      gfx.RectangleFilled(xpos + (x * 4), ypos + (12 - x * 3), xpos + (x * 4) + 2, ypos + (12 - x * 3) + (15 - (12 - x * 3)), GREEN);
    } else {
      gfx.RectangleFilled(xpos + (x * 4), ypos + (12 - x * 3), xpos + (x * 4) + 2, ypos + (12 - x * 3) + (15 - (12 - x * 3)), DARKGRAY);
    }
  }
}

void lock(uint16_t xpos, uint16_t ypos, uint16_t color) {
  gfx.RoundRectFilled(xpos, ypos + 8, xpos + 8, ypos + 8 + 8, 2, LIGHTGREY);
  gfx.Circle(xpos + 4, ypos + 8, 3, LIGHTGREY);
  gfx.CircleFilled(xpos + 4, ypos + 10, 1, BLACK);
  gfx.TriangleFilled(xpos + 4, ypos + 10, xpos + 3, ypos + 13, xpos + 5, ypos + 13, BLACK);
}
