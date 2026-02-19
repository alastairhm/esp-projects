#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Battery ADC pin
#define BATTERY_PIN 34

// -----------------------------
// Battery voltage reading
// -----------------------------
float readBatteryVoltage() {
    int raw = analogRead(BATTERY_PIN);
    float voltage = (raw / 4095.0) * 2.0 * 3.3;
    return voltage;
}

int batteryPercent(float v) {
    if (v >= 4.20) return 100;
    if (v >= 4.00) return 90;
    if (v >= 3.95) return 75;
    if (v >= 3.85) return 60;
    if (v >= 3.80) return 50;
    if (v >= 3.70) return 30;
    if (v >= 3.60) return 15;
    if (v >= 3.50) return 5;
    return 0;
}

void drawBatteryIcon(int percent) {
    int x = 108;
    int y = 0;

    display.drawRect(x, y, 18, 8, WHITE);
    display.fillRect(x + 18, y + 2, 2, 4, WHITE);

    int fill = map(percent, 0, 100, 0, 16);
    display.fillRect(x + 1, y + 1, fill, 6, WHITE);

    display.setCursor(x - 26, y);
    display.print(percent);
    display.print("%");
}

// -----------------------------
// WiFi scanning
// -----------------------------
void scanNetworks() {
    int n = WiFi.scanNetworks();

    // Store and sort networks by RSSI descending
    struct Net {
        String ssid;
        int rssi;
    };
    std::vector<Net> nets;

    for (int i = 0; i < n; i++) {
        nets.push_back({WiFi.SSID(i), WiFi.RSSI(i)});
    }

    std::sort(nets.begin(), nets.end(), [](Net a, Net b) {
        return a.rssi > b.rssi;
    });

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Battery
    float v = readBatteryVoltage();
    int p = batteryPercent(v);
    drawBatteryIcon(p);

    display.setCursor(0, 0);
    display.println("Wi-Fi Scan");

    for (int i = 0; i < nets.size() && i < 5; i++) {
        display.setCursor(0, 12 + (i * 10));
        display.print(nets[i].ssid);
        display.print("  ");
        display.print(nets[i].rssi);
        display.print("dBm");
    }

    display.display();
    WiFi.scanDelete();
}

// -----------------------------
// Setup
// -----------------------------
void setup() {
    Serial.begin(115200);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("OLED failed");
        while (1);
    }

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.println("Starting...");
    display.display();

    WiFi.mode(WIFI_STA);
    delay(500);
}

// -----------------------------
// Loop
// -----------------------------
void loop() {
    scanNetworks();
    delay(60000); // 1 minute pause
}

