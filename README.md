# esp-projects

A collection of standalone ESP32 Arduino sketches exploring WiFi scanning and small displays,
built incrementally from a bare serial scan up to a scrolling, button-driven UI and a WiFi
dashboard web server. Each top-level directory is one independent sketch — open it directly in
the Arduino IDE (or `arduino-cli`), no shared build system or project file ties them together.

## Projects

### `wifi_scan/`
Baseline WiFi scanner. Scans for nearby networks and prints SSID, RSSI, channel, and
open/secured status to the serial console every 5 seconds. No display, no extra hardware.

### `display_test/`
Bring-up sketch for the ST7789 SPI TFT. No WiFi — just initializes the display and SPI bus and
prints "Display OK!" to confirm wiring/pins before building anything more complex on top.

### `wifi_scan_screen/`
Combines the scanner and the display: renders a live WiFi scan as a table (SSID, RSSI, channel,
encryption) directly on the ST7789 TFT, with RSSI values color-coded green/yellow/red by signal
strength. Rescans and redraws every 5 seconds.

### `wifi_scan_screen_scrolling/`
Same table UI as `wifi_scan_screen`, but only redraws the list area (not the whole screen) and
supports scrolling through more results than fit on one screen.

### `wifi_scan_screen_scrolling_buttons/`
Adds physical input: two buttons (`BTN_UP` / `BTN_DOWN`) scroll the network list up and down, and
holding both together triggers a rescan. Builds directly on the scrolling sketch's rendering.

### `wifi_scan_screen_battery/` (`battery.ino`)
A variant targeting a 128x64 SSD1306 I2C OLED instead of the ST7789 TFT. Scans and lists the top
5 networks sorted by signal strength, and adds a battery icon/percentage read from an ADC pin via
a resistor-divider voltage reading.

### `webserver/`
Turns the ESP32 into a WiFi-connected dashboard server: connects to a configured network (STA
mode), serves a dashboard UI (`webserver/data/dashboard.html`) from SPIFFS, and exposes JSON
endpoints — `/scan.json` (last scan results), `/status.json` (battery/signal/uptime), and
`/rescan` (trigger a new scan) — backed by an in-memory network list. Also shows connection status
and the device's IP address on the ST7789 TFT during startup.

## Hardware

Display sketches target an ST7789 135x240 SPI TFT wired as:

| Signal | Pin |
|--------|-----|
| CS     | 5   |
| DC     | 16  |
| RST    | 23  |
| BL (backlight) | 4 |
| MOSI   | 19  |
| SCLK   | 18  |

The battery/OLED sketch instead targets a 128x64 SSD1306 I2C display, with battery voltage read
from ADC pin 34. See `info/esp32_screen/` for board photos and pinout reference.

## Dependencies

Third-party Arduino libraries used by the display sketches are vendored under `libraries/` (not
modified from upstream):

- Adafruit GFX Library 1.12.4
- Adafruit ST7735 and ST7789 Library 1.11.0
- Adafruit BusIO 1.17.4
- Adafruit seesaw Library 1.7.9
- SD 1.3.0

Sketches also use the ESP32 Arduino core's built-in `WiFi`, `WebServer`, and `SPIFFS` libraries.

## Getting started

1. Open the desired sketch's `.ino` file in the Arduino IDE (or use `arduino-cli`), targeting an
   ESP32 board.
2. Make sure `libraries/` is on your Arduino library path (or copy its contents into your
   sketchbook's `libraries/` folder) so the Adafruit/SD dependencies resolve.
3. For `webserver/`, set your WiFi credentials (`ssid`/`password`) in `webserver.ino` and upload
   `webserver/data/dashboard.html` to SPIFFS before flashing.
4. Compile and flash to your board.

## Contributing

Changes are made on a branch and merged via pull request; update this README and
[`CHANGELOG.md`](CHANGELOG.md) alongside code changes when they affect usage or behavior. See
[`CLAUDE.md`](CLAUDE.md) for more detail on repo conventions.

## License

[MIT](LICENSE)
