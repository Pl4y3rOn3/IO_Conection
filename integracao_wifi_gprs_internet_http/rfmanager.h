#ifndef _RF_MANAGER_H_
#define _RF_MANAGER_H_

#include <WiFi.h>

//Rural
//String ssid = "iot";
//String pass = "ufrpe_iot";

//Casa
String ssid = "Luciana ^-^";
String pass = "2luciana2";

// Initialize WiFi
bool initWiFi() {
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid.c_str(), pass.c_str());
  #ifdef DEBUG_MODE
  Serial.println("[WIFI] Connecting to WiFi...");
  #endif

  int tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef DEBUG_MODE
    Serial.print(".");
    #endif

    if(tries++ >= 20) {
      Serial.println("\n[WIFI] Failed connection");
      return false;
      //ESP.restart();
    }
  }
  // Colocar verificação de RSSI
  
  #ifdef DEBUG_MODE
  Serial.println(WiFi.localIP());
  #endif
  
  return true;
}

#endif /*_RF_MANAGER_H_*/