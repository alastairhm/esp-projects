#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Display pins for your board
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL   4

// SPI pins
#define TFT_MOSI 19
#define TFT_SCLK 18

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);

  // Backlight on
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Init SPI
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  // Init display (135 high, 240 wide)
  tft.init(135, 240);
  tft.setRotation(3);   // Landscape, correct orientation for your board

  // WiFi setup
  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);

  // Welcome screen
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WiFi Scanner");
}

void drawHeader() {
  tft.fillRect(0, 0, 240, 20, ST77XX_BLUE);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(5, 5);
  tft.setTextSize(1);
  tft.print("SSID                  RSSI   CH  ENC");
}

uint16_t rssiColor(int rssi) {
  if (rssi > -55) return ST77XX_GREEN;
  if (rssi > -70) return ST77XX_YELLOW;
  return ST77XX_RED;
}

void loop() {
  tft.fillScreen(ST77XX_BLACK);
  drawHeader();

  int n = WiFi.scanNetworks();
  int y = 25;

  if (n == 0) {
    tft.setCursor(10, 40);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(2);
    tft.println("No networks");
  } else {
    for (int i = 0; i < n && y < 135; i++) {
      String ssid = WiFi.SSID(i);
      int rssi = WiFi.RSSI(i);
      int ch = WiFi.channel(i);
      wifi_auth_mode_t enc = WiFi.encryptionType(i);

      // Trim SSID to fit
      if (ssid.length() > 12) ssid = ssid.substring(0, 12);

      tft.setCursor(5, y);
      tft.setTextSize(1);
      tft.setTextColor(ST77XX_WHITE);
      tft.print(ssid);

      tft.setCursor(120, y);
      tft.setTextColor(rssiColor(rssi));
      tft.printf("%d", rssi);

      tft.setCursor(165, y);
      tft.setTextColor(ST77XX_CYAN);
      tft.printf("%d", ch);

      tft.setCursor(200, y);
      tft.setTextColor(enc == WIFI_AUTH_OPEN ? ST77XX_GREEN : ST77XX_RED);
      tft.print(enc == WIFI_AUTH_OPEN ? "O" : "S");

      y += 12;
    }
  }

  WiFi.scanDelete();
  delay(5000);
}
