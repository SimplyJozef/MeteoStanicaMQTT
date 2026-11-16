// Každých 5 sekúnd prečítam všetky senzory a pošlem ich na sériovú linku ako jeden riadok JSON.
// Tento riadok bude neskôr obsahom MQTT správy; seq slúži na zistenie stratených správ.

#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <DFRobot_ENS160.h>

const char *DEVICE_ID = "meteo-01";
const uint8_t GY302 = 0x23;
const int PIN_AO = 34;
const int PIN_DO = 35;
const unsigned long INTERVAL_MS = 5000;

Adafruit_AHTX0 aht20;
Adafruit_AHTX0 aht21;
Adafruit_BMP280 bmp(&Wire);
DFRobot_ENS160_I2C ens(&Wire1, 0x53);

bool okSvetlo, okAht20, okBmp, okAht21, okEns;
unsigned long seq = 0;
unsigned long posledne = 0;

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
}

void loop() {
  if (seq > 0 && millis() - posledne < INTERVAL_MS) return;
  posledne = millis();
  seq++;

  float teplota = NAN, vlhkost = NAN, tlak = NAN, svetlo = NAN;
  sensors_event_t h, t;

  if (okSvetlo) svetlo = citajSvetlo();
  if (okAht20 && aht20.getEvent(&h, &t)) {
    teplota = t.temperature;
    vlhkost = h.relative_humidity;
  }
  if (okBmp) tlak = bmp.readPressure() / 100.0;

  // ENS160 kompenzuje AHT21, ktorý je na tom istom module hneď pri ňom
  if (okAht21 && okEns && aht21.getEvent(&h, &t)) ens.setTempAndHum(t.temperature, h.relative_humidity);

  int rainRaw = analogRead(PIN_AO);
  bool prsi = digitalRead(PIN_DO) == LOW;

  char json[256];
  snprintf(json, sizeof(json), "{\"dev\":\"%s\",\"seq\":%lu,\"ts_ms\":%lu", DEVICE_ID, seq, posledne);
  pridajCislo(json, sizeof(json), "temp_c", teplota, 2);
  pridajCislo(json, sizeof(json), "rh_pct", vlhkost, 1);
  pridajCislo(json, sizeof(json), "press_hpa", tlak, 2);
  pridajCislo(json, sizeof(json), "lux", svetlo, 1);

  size_t d = strlen(json);
  if (okEns) {
    snprintf(json + d, sizeof(json) - d, ",\"aqi\":%u,\"tvoc_ppb\":%u,\"eco2_ppm\":%u,\"ens_status\":%u",
             ens.getAQI(), ens.getTVOC(), ens.getECO2(), ens.getENS160Status());
  } else {
    snprintf(json + d, sizeof(json) - d, ",\"aqi\":null,\"tvoc_ppb\":null,\"eco2_ppm\":null,\"ens_status\":null");
  }

  d = strlen(json);
  snprintf(json + d, sizeof(json) - d, ",\"rain_raw\":%d,\"rain\":%s}", rainRaw, prsi ? "true" : "false");

  Serial.println(json);
}
