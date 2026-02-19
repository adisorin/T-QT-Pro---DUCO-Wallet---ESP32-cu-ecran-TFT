/*MIT License

Copyright (c) 2026 adisorin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/
/* https://github.com/adisorin?tab=repositories */


#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino_GFX_Library.h>
#include <time.h>

//////////////////////////////
// CULORI RGB565
#define GC9107_BLACK   0x0000
#define GC9107_WHITE   0xFFFF
#define GC9107_RED     0xF800
#define GC9107_GREEN   0x07E0
#define GC9107_BLUE    0x001F
#define GC9107_YELLOW  0xFFE0
#define GC9107_DARKGREY  0x4208
//////////////////////////////

//////////////////////////////
// TFT PINS
#define TFT_MOSI 2
#define TFT_SCLK 3
#define TFT_CS   5
#define TFT_DC   6
#define TFT_RST  1
#define TFT_BL   4

//////////////////////////////
// TFT INIT
Arduino_DataBus *bus = new Arduino_SWSPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, -1);
Arduino_GFX *gfx = new Arduino_GC9107(bus, TFT_RST, TFT_BL, true, 128, 128);


//////////////////////////////
// WIFI
WiFiMulti wifiMulti;

//////////////////////////////
// DUINO COIN
const char* ducoUser = "duco user"; //duco user
String apiUrl = String("https://server.duinocoin.com/balances/") + ducoUser;

//////////////////////////////
// NTP
const char* ntpServer = "pool.ntp.org";

//////////////////////////////
// TIMERE
unsigned long lastWifiCheck = 0;
unsigned long lastClockUpdate = 0;
unsigned long lastApiCheck = 0;

bool timeConfigured = false;

////////////////////////////////////////////////////////////
//////////////////// INDICATOR WIFI ////////////////////////

void afiseazaNivelWiFi() {

  int latime = 4;
  int spatiu = 2;
  int inaltimeMax = 15;

  int totalWidth = 5 * (latime + spatiu);

  int x = 128 - totalWidth - 4;   // dreapta sus
  int y = inaltimeMax + 4;        // sus cu mic padding

  int rssi = WiFi.RSSI();
  int nivel = 0;

  if (WiFi.status() != WL_CONNECTED) nivel = 0;
  else if (rssi > -50) nivel = 5;
  else if (rssi > -60) nivel = 4;
  else if (rssi > -70) nivel = 3;
  else if (rssi > -80) nivel = 2;
  else if (rssi > -90) nivel = 1;

  // sterge zona
  gfx->fillRect(x - 2, 0, totalWidth + 4, inaltimeMax + 6, GC9107_BLACK);

  // deseneaza bare
  for (int i = 0; i < 5; i++) {
    int h = (i + 1) * (inaltimeMax / 5);
    uint16_t culoare = (i < nivel) ? GC9107_GREEN : GC9107_DARKGREY;
    gfx->fillRect(x + i * (latime + spatiu), y - h, latime, h, culoare);
  }
}

////////////////////////////////////////////////////////////

void drawStaticUI() {
  gfx->fillScreen(GC9107_BLACK);
  gfx->setTextColor(GC9107_WHITE);
  gfx->setTextSize(1);
  gfx->setCursor(5, 8);
  gfx->println("DUCO Wallet");
}

////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  gfx->begin();
  gfx->fillScreen(GC9107_BLACK);

  drawStaticUI();

  // ==== Wifi network ssid and password ====
  wifiMulti.addAP("SSID1", "PASSWORD1");
  wifiMulti.addAP("SSID2", "PASSWORD2");

  gfx->setCursor(10, 35);
  gfx->setTextColor(GC9107_YELLOW);
  gfx->setTextSize(1);
  gfx->println("Connecting WiFi...");
}

////////////////////////////////////////////////////////////

void checkWiFi() {

  wifiMulti.run();

  if (WiFi.status() == WL_CONNECTED) {

    gfx->fillRect(0, 35, 128, 15, GC9107_BLACK);
    gfx->setCursor(10, 35);
    gfx->setTextColor(GC9107_GREEN);
    gfx->setTextSize(1);
    gfx->println("WiFi Connected");

    if (!timeConfigured) {
      configTime(0, 0, ntpServer);
      setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
      tzset();
      timeConfigured = true;
      Serial.println("NTP configured");
    }

  } else {

    for (int i = 0; i < 10; i++) {

      // Afișează
      gfx->fillRect(0, 35, 128, 15, GC9107_BLACK);
      gfx->setCursor(10, 35);
      gfx->setTextColor(GC9107_RED);
      gfx->setTextSize(1);
      gfx->println("...WRONG WIFI...");

      delay(500);

      // Șterge
      gfx->fillRect(0, 35, 128, 15, GC9107_BLACK);

      delay(500);
    }



  }
}

////////////////////////////////////////////////////////////

void afiseazaCeas() {

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return;

  char buffer[10];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);

  gfx->fillRect(0, 100, 128, 25, GC9107_BLACK);
  gfx->setTextColor(GC9107_YELLOW);
  gfx->setTextSize(2);
  gfx->setCursor(15, 105);
  gfx->print(buffer);
}

////////////////////////////////////////////////////////////

void updateDucoBalance() {

  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;
  http.begin(apiUrl);
  int httpCode = http.GET();

  if (httpCode == 200) {

    String payload = http.getString();
    StaticJsonDocument<512> doc;

    if (!deserializeJson(doc, payload)) {

      float balance = doc["result"]["balance"].as<float>();

      gfx->fillRect(0, 60, 128, 30, GC9107_BLACK);
      gfx->setCursor(10, 65);
      gfx->setTextColor(GC9107_GREEN);
      gfx->setTextSize(2);
      gfx->print(balance, 2);
    }

  } else {

    gfx->fillRect(0, 60, 128, 30, GC9107_BLACK);
    gfx->setCursor(10, 65);
    gfx->setTextColor(GC9107_RED);
    gfx->setTextSize(2);
    gfx->println("API error");
  }

  http.end();
}

////////////////////////////////////////////////////////////

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - lastWifiCheck > 10000) {
    checkWiFi();
    lastWifiCheck = currentMillis;
  }

  if (currentMillis - lastClockUpdate > 1000) {
    afiseazaCeas();
    afiseazaNivelWiFi();   // indicator dreapta sus
    lastClockUpdate = currentMillis;
  }

  if (currentMillis - lastApiCheck > 60000) {
    updateDucoBalance();
    lastApiCheck = currentMillis;
  }
}
