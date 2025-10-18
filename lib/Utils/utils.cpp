#include "utils.h"

bool enable_led_flag = true;
bool enable_buzz_flag = true;

int buzz_flag = 0;
int freq_flag = 0;

uint32_t led_blink_frequence = 100;
bool led_status = false;
bool is_uploading = false;

WiFiMulti wifiMulti;

TaskHandle_t wifiTaskHandle;
TaskHandle_t buzzTaskHandle;
TaskHandle_t ledTaskHandle;

void handleOTA(void *parameter) {
  // This runs on Core 0
  for (;;) {
    delay(50);             // yield to other tasks
    ArduinoOTA.handle();
  }
}

void handleBuzz(void *parameter) {
  // This runs on Core 0
  for (;;) {
    delay(100);             // yield to other tasks
    if(buzz_flag && enable_buzz_flag){
        ledcWriteTone(0, freq_flag);
        if (buzz_flag != 0){
            delay(buzz_flag); //this also prevent the 
            ledcWriteTone(0, 0);
        }
        buzz_flag = 0;
    }
  }
}

void handleLed(void *parameter) {
  // This runs on Core 0
  for (;;) {
    if(led_blink_frequence){
        digitalWrite(LED_SIG, led_status);
        led_status = !led_status;
        delay(led_blink_frequence);
    }else{
        digitalWrite(LED_SIG, 1);
        delay(100);             // yield to other tasks
    }
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

    if(enable_led_flag){
        pinMode(LED_SIG, OUTPUT);
    }
    if(enable_buzz_flag){
        pinMode(BUZZ_PIN,OUTPUT);
        ledcSetup(0, 2000, 8); // setup beeper
        ledcAttachPin(BUZZ_PIN, 0); // attach beeper
        digitalWrite(LED_SIG, 1);
    }
    xTaskCreatePinnedToCore(
        handleBuzz,    // Task function
        "handleBuzz",  // Name
        MINIMUM_RTOS_STACK_SIZE,               // Stack size
        NULL,               // Params
        2,                  // Priority (low)
        &buzzTaskHandle,    // Task handle
        0                   // Core 0
    );
    xTaskCreatePinnedToCore(
        handleLed,    // Task function
        "handleLed",  // Name
        MINIMUM_RTOS_STACK_SIZE,               // Stack size
        NULL,               // Params
        2,                  // Priority (low)
        &ledTaskHandle,    // Task handle
        0                   // Core 0
    );

    tone(1000);
    delay(100); //wait for wifi reset!

    WiFi.setSleep(false);
    WiFi.mode(WIFI_STA);
    unsigned long timer = millis();

    Serial.println("ConnectingWIFI"); 
    unsigned long start = millis();
    while (wifiMulti.run() != WL_CONNECTED && millis() - start < WL_WAIT_CONNECT_TIME) {
        delay(500);
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

    if (!MDNS.begin(DEVICE_NAME)) {
        Serial.println("Error starting mDNS!");
    } else {
        Serial.println("mDNS responder started");
    }

    ArduinoOTA.setHostname(DEVICE_NAME);
    ArduinoOTA
        .onStart([]() { 
            is_uploading = true;
            setBlink(0); 
            Serial.println("OTA Start");
        })
        .onEnd([]() { 
            //will reset automatically!
            Serial.println("\nOTA End");
        })
        .onProgress([](unsigned int progress, unsigned int total) {
            uint32_t cal_progress = progress / (total / 100);
            if(enable_led_flag)
                digitalWrite(LED_SIG, progress % 3); //cool effect
            Serial.printf("Progress: %u%%\r", cal_progress);
            TelnetStream.printf("Progress: %u%%\r", cal_progress);
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
    
    setBlink(1000);
}

void tone(int freq, unsigned long duration) {
    buzz_flag = duration;
    freq_flag = freq;
}

void setBlink(uint16_t freq){
    //lower the freq will led to unstable system!
    if(led_blink_frequence < 50)
        led_blink_frequence = 50;
    led_blink_frequence = freq;
}

bool isUploading(){
    return is_uploading;
}