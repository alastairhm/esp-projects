#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// -----------------------------
// Display pins (your board)
// -----------------------------
#define TFT_CS   5
#define TFT_DC   16
#define TFT_RST  23
#define TFT_BL   4
#define TFT_MOSI 19
#define TFT_SCLK 18

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// -----------------------------
// WiFi credentials
// -----------------------------
const char* ssid     = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";

// -----------------------------
// Web server
// -----------------------------
WebServer server(80);

// -----------------------------
// WiFi scan storage
// -----------------------------
struct NetworkInfo {
  String ssid;
  int32_t rssi;
  int32_t channel;
  String enc;
};

std::vector<NetworkInfo> networks;

// -----------------------------
// Helpers
// -----------------------------
String encType(wifi_auth_mode_t t) {
  switch (t) {
    case WIFI_AUTH_OPEN: return "OPEN";
    case WIFI_AUTH_WEP: return "WEP";
    case WIFI_AUTH_WPA_PSK: return "WPA";
    case WIFI_AUTH_WPA2_PSK: return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2";
    case WIFI_AUTH_WPA3_PSK: return "WPA3";
    default: return "UNK";
  }
}

void doScan() {
  networks.clear();
  int n = WiFi.scanNetworks();

  for (int i = 0; i < n; i++) {
    NetworkInfo info;
    info.ssid = WiFi.SSID(i);
    info.rssi = WiFi.RSSI(i);
    info.channel = WiFi.channel(i);
    info.enc = encType(WiFi.encryptionType(i));
    networks.push_back(info);
  }

  WiFi.scanDelete();
}

// -----------------------------
// JSON endpoints
// -----------------------------
void handleScanJSON() {
  String json = "{ \"networks\": [";

  for (size_t i = 0; i < networks.size(); i++) {
    json += "{";
    json += "\"ssid\":\"" + networks[i].ssid + "\",";
    json += "\"rssi\":" + String(networks[i].rssi) + ",";
    json += "\"channel\":" + String(networks[i].channel) + ",";
    json += "\"enc\":\"" + networks[i].enc + "\"";
    json += "}";
    if (i < networks.size() - 1) json += ",";
  }

  json += "] }";

  server.send(200, "application/json", json);
}

void handleStatusJSON() {
  long uptime = millis() / 1000;

  int battery = 100;  // placeholder
  long rssi = WiFi.RSSI();

  String json = "{";
  json += "\"battery\":" + String(battery) + ",";
  json += "\"signal\":" + String(rssi) + ",";
  json += "\"uptime\":" + String(uptime);
  json += "}";

  server.send(200, "application/json", json);
}

void handleRescan() {
  doScan();
  server.send(200, "text/plain", "OK");
}

// -----------------------------
// Setup
// -----------------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  // Display init
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(135, 240);
  tft.setRotation(3);

  // SPIFFS
  SPIFFS.begin(true);

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Connecting...");
  
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  // Connected!
  Serial.println("\nConnected!");
  Serial.println(WiFi.localIP());

  // --- Show IP on screen ---
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(10, 10);
  tft.println("Connected!");

  tft.setTextSize(1);
  tft.setCursor(10, 40);
  tft.print("IP: ");
  tft.println(WiFi.localIP());
  delay(2000);
  // --------------------------

  // Initial scan
  doScan();

  // Routes
  server.on("/scan.json", handleScanJSON);
  server.on("/status.json", handleStatusJSON);
  server.on("/rescan", handleRescan);

  // Serve dashboard from SPIFFS
  server.serveStatic("/", SPIFFS, "/dashboard.html");

  server.begin();
}

// -----------------------------
// Loop
// -----------------------------
void loop() {
  server.handleClient();
}

