#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <TelnetStream.h>
#include <WiFi.h>
#include <WiFiMulti.h>


#define LED_SIG 2
#define BUZZ_PIN 12
#define WL_WAIT_CONNECT_TIME 20000

void setupServers(const bool reset, bool enable_led = true, bool enable_buzz = true);
void addWifiAP(const char* ssid, const char* password);
void tone(int freq = 2000, unsigned long duration = 100);
