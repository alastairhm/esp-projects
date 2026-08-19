# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Top-level workflow rule

- Never commit directly to `main`. Create a branch for every change, and open a PR for it.
- Update `README.md` when a change affects how the repo/sketches are used or organized, and update
  `CHANGELOG.md` (Keep a Changelog format) for every user-facing change, as part of the same PR.

## What this is

A collection of standalone ESP32 Arduino sketches (`.ino`) exploring WiFi scanning and small SPI/I2C
displays, built incrementally from a bare scan to a scrolling, button-driven UI and a WiFi dashboard
web server. There is no build system, package manager, or test suite — this is a plain Arduino IDE
sketchbook, not a PlatformIO project.

## Repository layout

- Each top-level directory is one independent Arduino sketch, named after what it does and built on
  the previous one:
  - `wifi_scan/` — serial-only WiFi scan (baseline).
  - `display_test/` — bring-up sketch for the ST7789 TFT, no WiFi.
  - `wifi_scan_screen/` — WiFi scan rendered as a table on the ST7789.
  - `wifi_scan_screen_scrolling/` — same, with scrollable list rendering.
  - `wifi_scan_screen_scrolling_buttons/` — adds two-button (`BTN_UP`/`BTN_DOWN`) scroll and
    long-press-to-rescan input handling.
  - `wifi_scan_screen_battery/` (`battery.ino`) — SSD1306 OLED variant that adds battery voltage
    read from an ADC pin and a battery icon overlay.
  - `webserver/` — connects to WiFi (STA mode with hardcoded credentials), serves a dashboard UI
    from SPIFFS (`webserver/data/dashboard.html`) via `WebServer`, and exposes `/scan.json`,
    `/status.json`, `/rescan` JSON endpoints backed by an in-memory `NetworkInfo` vector.
  - Each sketch's directory name must match its `.ino` filename (Arduino IDE requirement) —
    `webserver/webserver.ino`, `battery.ino` lives in `wifi_scan_screen_battery/`, etc.
- `libraries/` — vendored third-party Arduino libraries used by the display sketches: Adafruit_GFX,
  Adafruit_BusIO, Adafruit_ST7735_and_ST7789, Adafruit_seesaw, and SD. These are unmodified upstream
  library sources, not project code — don't edit them; if a display bug shows up here, it's almost
  certainly in the sketch's use of the library, not the library itself.
- `info/esp32_screen/` — reference photos/pinout for the physical board and display in use.

## Hardware conventions

Display sketches target an ST7789 135x240 SPI TFT with a fixed pinout repeated across sketches:
`TFT_CS=5, TFT_DC=16, TFT_RST=23, TFT_BL=4, TFT_MOSI=19, TFT_SCLK=18` (SPI is initialized manually via
`SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS)` since MISO is unused). The battery/OLED sketch instead
targets an SSD1306 128x64 I2C display and reads battery voltage from ADC pin 34 via a resistor divider
assumed to halve the voltage (see `batteryPercent()` thresholds in `wifi_scan_screen_battery/battery.ino`).
When adding a new display sketch, copy the pin `#define` block and SPI/I2C init pattern from the
closest existing sketch rather than inventing a new one.

## Working in this repo

- There is no compiler/linter/test invocable from the CLI — verification is "open the sketch in the
  Arduino IDE (or `arduino-cli compile`) targeting an ESP32 board and flash it," which Claude Code
  cannot do headlessly. When changing a sketch, cross-check against the Adafruit library headers in
  `libraries/` for correct API usage instead of assuming compilation succeeded.
- `webserver/webserver.ino` has WiFi credentials (`ssid`/`password`) hardcoded as placeholders at the
  top of the file — treat any real credentials a user adds here as local/uncommitted, not something to
  propagate elsewhere.
- JSON responses in `webserver.ino` are hand-built via string concatenation (no JSON library) — follow
  that existing pattern for new fields rather than introducing a JSON library dependency, unless asked.
