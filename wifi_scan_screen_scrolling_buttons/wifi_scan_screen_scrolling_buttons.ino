#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Display pins
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL   4

// SPI pins
#define TFT_MOSI 19
#define TFT_SCLK 18

// Button pins
#define BTN_UP    0     // Left button
#define BTN_DOWN  35    // Right button

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

int scrollIndex = 0;
int networksFound = 0;

void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  tft.init(135, 240);
  tft.setRotation(3);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);
  delay(100);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("WiFi Scanner");
  delay(1500);

  // Initial scan
  networksFound = WiFi.scanNetworks();
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

void drawList() { // Clear ONLY the list area (not the whole screen) tft.fillRect(0, 20, 240, 115, ST77XX_BLACK); int y = 25; int rowsPerScreen = 8; for (int i = 0; i < rowsPerScreen; i++) { int idx = scrollIndex + i; if (idx >= networksFound) break; String ssid = WiFi.SSID(idx); int rssi = WiFi.RSSI(idx); int ch = WiFi.channel(idx); wifi_auth_mode_t enc = WiFi.encryptionType(idx); if (ssid.length() > 12) ssid = ssid.substring(0, 12); tft.setCursor(5, y); tft.setTextSize(1); tft.setTextColor(ST77XX_WHITE); tft.print(ssid); tft.setCursor(120, y); tft.setTextColor(rssiColor(rssi)); tft.printf("%d", rssi); tft.setCursor(165, y); tft.setTextColor(ST77XX_CYAN); tft.printf("%d", ch); tft.setCursor(200, y); tft.setTextColor(enc == WIFI_AUTH_OPEN ? ST77XX_GREEN : ST77XX_RED); tft.print(enc == WIFI_AUTH_OPEN ? "O" : "S"); y += 12; } }

void loop() {
  drawList();

  // Scroll UP
  if (digitalRead(BTN_UP) == LOW) {
    if (scrollIndex > 0) scrollIndex--;
    delay(200);
  }

  // Scroll DOWN
  if (digitalRead(BTN_DOWN) == LOW) {
    if (scrollIndex < networksFound - 1) scrollIndex++;
    delay(200);
  }

  // Long press DOWN to rescan
  if (digitalRead(BTN_DOWN) == LOW && digitalRead(BTN_UP) == LOW) {
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(10, 40);
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.println("Scanning...");
    networksFound = WiFi.scanNetworks();
    scrollIndex = 0;
    delay(500);
  }
}
