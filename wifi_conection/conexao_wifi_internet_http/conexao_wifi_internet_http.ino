#define DEBUG_MODE

#include "rfmanager.h"
#include "http.h"
#include "esp_timer.h"

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define DEEP_SLEEP_TIME  5        /* Time ESP32 will go to sleep (in seconds) */


void setup() {

  #ifdef DEBUG_MODE
  Serial.begin(115200);
  #endif

  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_TIME * uS_TO_S_FACTOR);

  uint64_t start = esp_timer_get_time(); //Pega tempo inicial

  //Execução do programa
  initWiFi();
  Serial.print("RSSI: ");
  Serial.println(WiFi.RSSI());

  //delay(1000);

  int current = 25;
  int power = 20;
  bool httpSuccess = sendHttpToServer(current, power);


  uint64_t end = esp_timer_get_time(); //Pega tempo final

  Serial.printf("Tempo de execucao: %llu milisegundos", (end - start)/1000); // Realiza o calculo do ms
  Serial.println("");

  #ifdef DEBUG_MODE
  Serial.println("[DEEP SLEEP] Going to sleep!");
  #endif
  
  esp_deep_sleep_start();

}

void loop() {
}