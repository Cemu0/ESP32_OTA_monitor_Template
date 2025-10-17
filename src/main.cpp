#include "utils.h"

void setup() {
  addWifiAP("ssid1","12345678");
  addWifiAP("ssid2","12345678");
  // ...
  setupServers(true);
  tone();
}

void loop() {
  // your code goes here 
}