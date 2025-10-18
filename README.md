# 🚀 ESP32 OTA (mDNS Support) + Telnet Template

## 🧩 Introduction
This project provides a ready-to-use ESP32 template featuring:
- ✅ Multi-AP Wi-Fi connection using `WiFiMulti`  
- ✅ OTA updates via **PlatformIO → OTA Upload** and Monitor; including failback to UART with no config change needed!
- ✅ mDNS support (with mDNS hostname resolution automatically)
- ✅ TelnetStream for debugging 
- ✅ Buzz + Blink RTOS tasks for your conviniene! so you can focus on your main task!
This suitable for custom Robotic project and was build for my micromouse setup, I recommend you to use ESPHome for IOT project!

### 🛠 Requirements
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
- ESP32 development board (e.g., `esp32dev`)

## ⚙️ Usage Example


I highly recommend using secret.h to prevent your SSID from leak!

When cloned this repo, use need to copy the env/secret.template.h to env/secret.h and config the correct env.

Then you can use like this:

```cpp
#include <../env/secret.h>
#include "utils.h"

void setup() {
  addWifiAP(SECRET_SSID1, SECRET_PASS1);
  addWifiAP("ssid2", "12345678");
  // Add more networks as needed
  setupServers(true);  // true = reset if cannot connectwifi!
  setBlink(2000); //blink at 0.5Hz
  tone(2000, 100); //2000Hz tone for 100ms
}

void loop() {
  // Your main loop logic
}
````
---

## 📁 Project Structure

```

project_dir
├── lib
│   ├── Utils
│   │   ├── include
│   │   │   └── utils.h
│   │   └── src
│   │       └── utils.cpps
├── scripts
│   ├── resolve_ota.py
│   └── prevent_uart.py
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
| `scripts/prevent_uart.py` | Cancel UART upload if OTA success! |
| `src/main.cpp` | Your main Arduino sketch |
| `platformio.ini` | PlatformIO project configuration |
| `README.md` | This documentation |

---


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