#ifndef _HTTP_H_
#define _HTTP_H_

#include <WiFi.h>
#include <HTTPClient.h>

#define JSON_BUFFER_SIZE (300)
char json[JSON_BUFFER_SIZE];
const char* hostname = "http://devtb.smartrural.com.br";
String accessToken = "pvuufKL2uWP64stCKJNP";

// return true of http code is 200, false if not
bool sendHttpToServer(float current, float power) {
  #ifdef DEBUG_MODE
  Serial.println("-----------------------------------------------------------");
  #endif
  if(WiFi.status()== WL_CONNECTED){
    snprintf (json, JSON_BUFFER_SIZE, \
      "{" \
        "\"current\":%3.3f," \
        "\"power\":%3.3f" \
      "}", \
      current, power);

    // requisição http post
    HTTPClient http;
    String uri = String(hostname) + "/api/v1/" + String(accessToken) + "/telemetry";
    http.begin(uri);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(json);

    #ifdef DEBUG_MODE
    Serial.print("[HTTP] Response code: ");
    Serial.println(httpResponseCode);
    #endif
    
    http.end();
    
    return (httpResponseCode == 200) ? true : false;
  }
  else {
    #ifdef DEBUG_MODE
    Serial.println("[HTTP] WiFi Disconnected");
    #endif
  }
  return false;
}

#endif /*_HTTP_H_*/