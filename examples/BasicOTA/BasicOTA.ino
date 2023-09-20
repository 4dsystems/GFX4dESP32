#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// TODO: Edit this include file to match your display module
#include "gfx4desp32_gen4_ESP32_32CT_CLB.h"

// TODO: Edit this class initialization to match your display module
gfx4desp32_gen4_ESP32_32CT_CLB gfx = gfx4desp32_gen4_ESP32_32CT_CLB();

// Change this match the Wi-Fi network
const char* ssid = "..........";
const char* password = "..........";


void setup() {
  gfx.begin();
  gfx.Cls();
  gfx.ScrollEnable(true);
  gfx.BacklightOn(true);
  gfx.Orientation(PORTRAIT);
  gfx.SmoothScrollSpeed(5);
  gfx.TextColor(WHITE, BLACK);
  gfx.Font(2);
  gfx.TextSize(1);

  gfx.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    gfx.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname("4D-ESP32");

  // No authentication by default so let's use 'admin'
  ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      gfx.println("Start updating " + type);
    })
    .onEnd([]() {
      gfx.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      gfx.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      gfx.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) gfx.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) gfx.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) gfx.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) gfx.println("Receive Failed");
      else if (error == OTA_END_ERROR) gfx.println("End Failed");
    });

  ArduinoOTA.begin();

  gfx.println("Ready");
  gfx.print("IP address: ");
  gfx.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
}