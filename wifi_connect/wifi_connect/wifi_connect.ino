// Program pripojí ESP32 k domácej 2,4 GHz Wi-Fi a vypíše IP adresu, silu signálu a čas pripojenia.
// Každých 5 sekúnd skontroluje spojenie a pri výpadku sa pripojí znova.

#include <WiFi.h>
#include "secrets.h"

void pripoj() {
  Serial.printf("Pripajam sa k \"%s\"...\n", WIFI_SSID);
  unsigned long start = millis();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) delay(100);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("Pripojene za %lu ms | IP %s | brana %s | RSSI %d dBm | kanal %d\n",
                  millis() - start, WiFi.localIP().toString().c_str(),
                  WiFi.gatewayIP().toString().c_str(), WiFi.RSSI(), WiFi.channel());
  } else {
    Serial.printf("Nepodarilo sa pripojit (stav %d)\n", WiFi.status());
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  WiFi.mode(WIFI_STA);
  pripoj();
}

void loop() {
  delay(5000);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Spojenie stratene");
    WiFi.disconnect();
    pripoj();
  } else {
    Serial.printf("%lu s | RSSI %d dBm\n", millis() / 1000, WiFi.RSSI());
  }
}
