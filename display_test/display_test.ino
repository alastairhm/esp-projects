#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// Your board's ST7789 pins
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL   4   // Backlight pin (optional)

// SPI pins (fixed for your board)
#define TFT_MOSI 19
#define TFT_SCLK 18

// Create display object (135x240)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);

  // Backlight on
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  // Init SPI manually
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

  // Init display
  tft.init(135, 240);     // Height, Width
  tft.setRotation(1);     // Landscape mode

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Display OK!");
}

void loop() {
}
