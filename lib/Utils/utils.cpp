#include "utils.h"

bool enable_led_flag = true;
bool enable_buzz_flag = true;

WiFiMulti wifiMulti;

TaskHandle_t wifiTaskHandle;

void handleOTA(void *parameter) {
  // This runs on Core 0
  Serial.printf("[Task] WiFi/Command task running on core %d\n", xPortGetCoreID());
  for (;;) {
    delay(1000);             // yield to other tasks
    ArduinoOTA.handle();
  }
}

void addWifiAP(const char* ssid, const char* password){
    wifiMulti.addAP(ssid, password);
}

void setupServers(const bool reset, bool enable_led, bool enable_buzz) {
    enable_led_flag = enable_led;
    enable_buzz_flag = enable_buzz;
    // after connect wifi the first time, you need to force disconnect to make new connection 
    WiFi.disconnect(true);
    // esp_wifi_disconnect();
    WiFi.mode(WIFI_OFF);
    // esp_wifi_stop();
    Serial.begin(115200);

    if(enable_led_flag)
        pinMode(LED_SIG, OUTPUT);
    if(enable_buzz_flag){
        pinMode(BUZZ_PIN,OUTPUT);
        ledcSetup(0, 2000, 8); // setup beeper
        ledcAttachPin(BUZZ_PIN, 0); // attach beeper
        tone();
    }

    WiFi.setSleep(false);
    WiFi.mode(WIFI_STA);
    unsigned long timer = millis();

    Serial.println("ConnectingWIFI..."); 
    unsigned long start = millis();
    while (wifiMulti.run() != WL_CONNECTED && millis() - start < WL_WAIT_CONNECT_TIME) {
        delay(200);
        Serial.print(".");
    }
    if(WiFi.status() != WL_CONNECTED && reset){
        Serial.println("Connection Failed! Skip");
        ESP.restart();
    }else{
        Serial.printf("Connected to: %s (RSSI %d dBm)\n",
            WiFi.SSID().c_str(),
            WiFi.RSSI());
        Serial.print("ip: ");
        Serial.println(WiFi.localIP());
    }

    if (!MDNS.begin("myesp32")) {
        Serial.println("Error starting mDNS!");
    } else {
        Serial.println("mDNS responder started");
    }

    ArduinoOTA.setHostname("myesp32");
    ArduinoOTA
        .onStart([]() { Serial.println("OTA Start");})
        .onEnd([]() { Serial.println("\nOTA End");})
        .onProgress([](unsigned int progress, unsigned int total) {
            if(enable_led_flag)
                digitalWrite(LED_SIG, HIGH);
            float cal_progress = progress / (total / 100);
            Serial.printf("Progress: %u%%\r", cal_progress);
            TelnetStream.printf("Progress: %u%%\r", cal_progress);
            if(enable_led_flag)
                digitalWrite(LED_SIG, LOW);
        })
        .onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
        });
    ArduinoOTA.begin();
    Serial.println("OTA Ready");
    TelnetStream.begin(); // starts TCP server on port 23

    

    // Launch command handler on Core 0
    xTaskCreatePinnedToCore(
        handleOTA,    // Task function
        "handleOTA",  // Name
        4096,               // Stack size
        NULL,               // Params
        1,                  // Priority (low) or 2
        &wifiTaskHandle,    // Task handle
        0                   // Core 0
    );
}

void tone(int freq, unsigned long duration) {
  ledcWriteTone(0, freq);
  if (duration != 0){
    delay(duration);
    ledcWriteTone(0, 0);
  }
}