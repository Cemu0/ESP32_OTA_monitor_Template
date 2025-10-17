# 🚀 ESP32 OTA (mDNS Support) + Telnet Template

## 🧩 Introduction
This project provides a ready-to-use ESP32 template featuring:
- ✅ Multi-AP Wi-Fi connection using `WiFiMulti`  
- ✅ OTA updates via **PlatformIO → OTA Upload** and Monitor
- ✅ mDNS support (with mDNS hostname resolution automatically)
- ✅ TelnetStream for debugging 

### 🛠 Requirements
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- ESP32 development board (e.g., `esp32dev`)

---

## 📁 Project Structure

```

project_dir
├── lib
│   ├── Utils
│   │   ├── include
│   │   │   └── utils.h
│   │   └── src
│   │       └── utils.cpp
├── scripts
│   └── resolve_ota.py
├── src
│   └── main.cpp
├── platformio.ini
└── README.md

````

### File Descriptions

| Path | Description |
|------|--------------|
| `lib/Utils` | Contains Wi-Fi setup, Telnet server, and OTA utility functions |
| `scripts/resolve_ota.py` | Resolves mDNS names (`.local`) to IP for OTA uploads |
| `src/main.cpp` | Your main Arduino sketch |
| `platformio.ini` | PlatformIO project configuration |
| `README.md` | This documentation |

---

## ⚙️ Usage Example

```cpp
#include "utils.h"

void setup() {
  addWifiAP("ssid1", "12345678");
  addWifiAP("ssid2", "12345678");
  // Add more networks as needed
  setupServers(true);  // true = enable OTA + Telnet
}

void loop() {
  // Your main loop logic
}
````

## 🧰 Tips

* Ping your device: `ping esp32.local`
* Connect via Telnet: `telnet esp32.local 23`
* Monitor: `pio device monitor`

Note: that because we are using socket connection for device monitor, you need to press enter for it to reconnect after OTA upload!

---

## 🧑‍💻 MIT License

This project is provided as an open template for educational and prototyping use.
You are free to modify and reuse it in your own projects.

---

**Enjoy seamless OTA + Telnet development on ESP32 🚀**