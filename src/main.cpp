#include <secret.h>
#include "utils.h"

void setup() {
  addWifiAP(SECRET_SSID1, SECRET_PASS1);
  addWifiAP("ssid2", "12345678");
  // ...
  setupServers(true);
  tone();
  setBlink(2000);
}

void loop() {
  // your code goes here 
  if(!isUploading()){
    TelnetStream.println("ping");
    delay(1000);  
  }
}