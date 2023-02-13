#define SIM800L_AXP192_VERSION_20200327

#include <Arduino.h>
#include "utilities.h"

//#define TEST_HTTP_GET
#define TEST_HTTP_POST

// Your GPRS credentials (leave empty, if not needed)
const char apn[]      = "zap.vivo.com.br";
const char gprsUser[] = "vivo"; // GPRS User
const char gprsPass[] = "vivo"; // GPRS Password

// SIM card PIN (leave empty, if not defined)
const char simPIN[]   = ""; 

// Server details
const char server[] = "devtb.smartrural.com.br"; // domain name: example.com
const char resource[] = "/api/v1/pvuufKL2uWP64stCKJNP/telemetry"; // resource path, for example: /post-data.php
const int  port = 80;


// Set serial for debug console (to Serial Monitor, default speed 115200)
#define Serial Serial
// Set serial for AT commands (to SIM800 module)
#define SerialAT Serial1

// Configure TinyGSM library
#define TINY_GSM_MODEM_SIM800      // Modem is SIM800
#define TINY_GSM_RX_BUFFER   1024  // Set RX buffer to 1Kb

// Define the serial console for debug prints, if needed
//#define DUMP_AT_COMMANDS

#include <Wire.h>
#include <TinyGsmClient.h>

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, Serial);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

// TinyGSM Client for Internet connection
TinyGsmClient client(modem);

#define uS_TO_S_FACTOR 1000000UL   /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) 3600 seconds = 1 hour */

#define IP5306_ADDR          0x75
#define IP5306_REG_SYS_CTL0  0x00

void setup() {
  // Set serial monitor debugging window baud rate to 115200
  Serial.begin(115200);

  // Set modem reset, enable, power pins
  pinMode(MODEM_PWRKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);
  digitalWrite(MODEM_PWRKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);

  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  //delay(3000);

  // Restart SIM800 module, it takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  //modem.restart();
  modem.init(); // if you don't need the complete restart

  // Unlock your SIM card with a PIN if needed
  if (strlen(simPIN) && modem.getSimStatus() != 3 ) {
    modem.simUnlock(simPIN);
  }
  
  // Configure the wake up source as timer wake up  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
}

void loop() {
  uint64_t start = esp_timer_get_time(); //Pega tempo inicial
  Serial.print("Connecting to APN: ");
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println(" fail");
    return;
  }
  
  Serial.println(" OK");
  
  Serial.print("Connecting to ");
  Serial.print(server);
  if (!client.connect(server, port)) {
    Serial.println(" fail");
    esp_deep_sleep_start();
  }

  Serial.println(" OK");

  checkSignall();

  #ifdef TEST_HTTP_GET
    Serial.println("Performing HTTP GET request...");
    client.print(String("GET ") + resource + " HTTP/1.1\r\n");
    client.print(String("Host: ") + server + "\r\n\r\n");
  #endif

  #ifdef TEST_HTTP_POST
    Serial.println("Performing HTTP POST request...");
    String httpRequestData = "{\"voltage\":5}";
    client.print(String("POST ") + resource + " HTTP/1.1\r\n");
    client.print(String("Host: ") + server + "\r\n");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(httpRequestData.length());
    client.println();
    client.print(httpRequestData + "\r\n\r\n");
  #endif

  unsigned long timeout = millis();
  while (client.connected() && millis() - timeout < 10000L) {
    // Print available data (HTTP response from server)
    while (client.available()) {
      char c = client.read();
      Serial.print(c);
      timeout = millis();
    }
  }
  Serial.println();

  uint64_t end = esp_timer_get_time(); //Pega tempo final

  Serial.printf("Tempo de execucao: %llu milisegundos", (end - start)/1000); // Realiza o calculo do ms
  Serial.println("");

  // Close client and disconnect
  client.stop();
  Serial.println(F("Server disconnected"));
  modem.gprsDisconnect();
  Serial.println(F("GPRS disconnected"));
  
  // Put ESP32 into deep sleep mode (with timer wake up)
  esp_deep_sleep_start();
}

void checkSignall() 
{
  Serial.println("Signall Quality: " + String( modem.getSignalQuality( ) ));
}