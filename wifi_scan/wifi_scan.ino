#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32 WiFi Scanner");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
}

void loop() {
  Serial.println("Scanning...");

  int n = WiFi.scanNetworks();
  Serial.println("Scan complete.");

  if (n == 0) {
    Serial.println("No networks found.");
  } else {
    Serial.printf("%d networks found:\n", n);
    for (int i = 0; i < n; ++i) {
      Serial.printf("%2d: %s (%d dBm) CH:%d %s\n",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i),
                    (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "open" : "secured");
      delay(10);
    }
  }

  Serial.println();
  delay(5000);
}
