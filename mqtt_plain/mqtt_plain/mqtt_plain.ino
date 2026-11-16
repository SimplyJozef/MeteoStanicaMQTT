// Každých 5 sekúnd prečítam všetky senzory a pošlem ich ako JSON na MQTT broker bez šifrovania (port 1883).
// Téma je meteo/meteo-01/telemetry; pri výpadku Wi-Fi alebo brokera sa program pripojí znova.

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <DFRobot_ENS160.h>
#include "secrets.h"

const char *DEVICE_ID = "meteo-01";
const char *TOPIC = "meteo/meteo-01/telemetry";
const uint8_t GY302 = 0x23;
const int PIN_AO = 34;
const int PIN_DO = 35;
const unsigned long INTERVAL_MS = 5000;

WiFiClient net;
PubSubClient mqtt(net);

Adafruit_AHTX0 aht20;
Adafruit_AHTX0 aht21;
Adafruit_BMP280 bmp(&Wire);
DFRobot_ENS160_I2C ens(&Wire1, 0x53);

bool okSvetlo, okAht20, okBmp, okAht21, okEns;
unsigned long seq = 0;
unsigned long posledne = 0;

void pripojWifi() {
  if (WiFi.status() == WL_CONNECTED) return;
  unsigned long start = millis();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) delay(100);
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("# Wi-Fi OK za %lu ms | IP %s | RSSI %d dBm\n", millis() - start,
                  WiFi.localIP().toString().c_str(), WiFi.RSSI());
  } else {
    Serial.println("# Wi-Fi sa nepodarilo pripojit");
  }
}

void pripojMqtt() {
  if (mqtt.connected() || WiFi.status() != WL_CONNECTED) return;
  unsigned long start = millis();
  if (mqtt.connect(DEVICE_ID)) {
    Serial.printf("# MQTT OK za %lu ms | broker %s:%d\n", millis() - start, MQTT_HOST, MQTT_PORT);
  } else {
    Serial.printf("# MQTT chyba, stav %d\n", mqtt.state());
  }
}

float citajSvetlo() {
  if (Wire.requestFrom(GY302, (uint8_t)2) != 2) return NAN;
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return ((hi << 8) | lo) / 1.2;
}

// NAN (chybajuci senzor) zapise ako null, aby JSON zostal platny
void pridajCislo(char *buf, size_t n, const char *kluc, float hodnota, int desatinne) {
  size_t d = strlen(buf);
  if (isnan(hodnota)) snprintf(buf + d, n - d, ",\"%s\":null", kluc);
  else snprintf(buf + d, n - d, ",\"%s\":%.*f", kluc, desatinne, hodnota);
}

void zostavJson(char *json, size_t n) {
  float teplota = NAN, vlhkost = NAN, tlak = NAN, svetlo = NAN;
  sensors_event_t h, t;

  if (okSvetlo) svetlo = citajSvetlo();
  if (okAht20 && aht20.getEvent(&h, &t)) {
    teplota = t.temperature;
    vlhkost = h.relative_humidity;
  }
  if (okBmp) tlak = bmp.readPressure() / 100.0;
  if (okAht21 && okEns && aht21.getEvent(&h, &t)) ens.setTempAndHum(t.temperature, h.relative_humidity);

  snprintf(json, n, "{\"dev\":\"%s\",\"seq\":%lu,\"ts_ms\":%lu", DEVICE_ID, seq, posledne);
  pridajCislo(json, n, "temp_c", teplota, 2);
  pridajCislo(json, n, "rh_pct", vlhkost, 1);
  pridajCislo(json, n, "press_hpa", tlak, 2);
  pridajCislo(json, n, "lux", svetlo, 1);

  size_t d = strlen(json);
  if (okEns) {
    snprintf(json + d, n - d, ",\"aqi\":%u,\"tvoc_ppb\":%u,\"eco2_ppm\":%u,\"ens_status\":%u",
             ens.getAQI(), ens.getTVOC(), ens.getECO2(), ens.getENS160Status());
  } else {
    snprintf(json + d, n - d, ",\"aqi\":null,\"tvoc_ppb\":null,\"eco2_ppm\":null,\"ens_status\":null");
  }

  d = strlen(json);
  snprintf(json + d, n - d, ",\"rain_raw\":%d,\"rain\":%s}", analogRead(PIN_AO),
           digitalRead(PIN_DO) == LOW ? "true" : "false");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Wire.begin(21, 22);
  Wire1.begin(25, 26);
  pinMode(PIN_DO, INPUT);

  Wire.beginTransmission(GY302);
  Wire.write(0x10);
  okSvetlo = Wire.endTransmission() == 0;
  okAht20 = aht20.begin(&Wire);
  okBmp = bmp.begin(0x77, 0x58);
  okAht21 = aht21.begin(&Wire1);
  okEns = ens.begin() == 0;
  if (okEns) ens.setPWRMode(ENS160_STANDARD_MODE);

  Serial.printf("# GY-302 %s | AHT20 %s | BMP280 %s | AHT21 %s | ENS160 %s\n",
                okSvetlo ? "OK" : "CHYBA", okAht20 ? "OK" : "CHYBA", okBmp ? "OK" : "CHYBA",
                okAht21 ? "OK" : "CHYBA", okEns ? "OK" : "CHYBA");

  WiFi.mode(WIFI_STA);
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setBufferSize(512);
  pripojWifi();
  pripojMqtt();
}

void loop() {
  pripojWifi();
  pripojMqtt();
  mqtt.loop();

  if (seq > 0 && millis() - posledne < INTERVAL_MS) return;
  posledne = millis();
  seq++;

  char json[256];
  zostavJson(json, sizeof(json));
  bool odoslane = mqtt.connected() && mqtt.publish(TOPIC, json);
  Serial.printf("%s %s\n", odoslane ? "PUB" : "NEODOSLANE", json);
}
