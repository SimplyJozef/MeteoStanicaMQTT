// Každých 5 sekúnd prečítam všetky senzory stanice z oboch I2C zberníc a dažďový senzor a vypíšem ich do jedného bloku.
// Zbernica 1 (21/22): GY-302, AHT20, BMP280. Zbernica 2 (25/26): ENS160 + AHT21 (iba kompenzácia). Dážď: piny 34/35.

#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <DFRobot_ENS160.h>

const uint8_t GY302 = 0x23;
const int PIN_AO = 34;
const int PIN_DO = 35;

Adafruit_AHTX0 aht20;
Adafruit_AHTX0 aht21;
Adafruit_BMP280 bmp(&Wire);
DFRobot_ENS160_I2C ens(&Wire1, 0x53);

bool okSvetlo, okAht20, okBmp, okAht21, okEns;
unsigned long meranie = 0;

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

  Serial.printf("GY-302 %s | AHT20 %s | BMP280 %s | AHT21 %s | ENS160 %s\n",
                okSvetlo ? "OK" : "CHYBA", okAht20 ? "OK" : "CHYBA", okBmp ? "OK" : "CHYBA",
                okAht21 ? "OK" : "CHYBA", okEns ? "OK" : "CHYBA");
}

void loop() {
  meranie++;
  Serial.printf("--- meranie %lu (%lu s) ---\n", meranie, millis() / 1000);

  Wire.requestFrom(GY302, (uint8_t)2);
  if (Wire.available() == 2) {
    uint8_t hi = Wire.read();
    uint8_t lo = Wire.read();
    uint16_t raw = (hi << 8) | lo;
    Serial.printf("Svetlo:   %.1f lx\n", raw / 1.2);
  } else {
    Serial.println("Svetlo:   senzor neodpoveda");
  }

  sensors_event_t vlhkost, teplota;
  if (okAht20 && aht20.getEvent(&vlhkost, &teplota)) {
    Serial.printf("AHT20:    %.2f C   %.1f %%RH\n", teplota.temperature, vlhkost.relative_humidity);
  } else {
    Serial.println("AHT20:    senzor neodpoveda");
  }

  if (okBmp) Serial.printf("BMP280:   %.2f C   %.2f hPa\n", bmp.readTemperature(), bmp.readPressure() / 100.0);
  else Serial.println("BMP280:   senzor neodpoveda");

  if (okAht21 && aht21.getEvent(&vlhkost, &teplota)) {
    if (okEns) ens.setTempAndHum(teplota.temperature, vlhkost.relative_humidity);
    Serial.printf("AHT21:    %.2f C   %.1f %%RH (kompenzacia)\n", teplota.temperature, vlhkost.relative_humidity);
  } else {
    Serial.println("AHT21:    senzor neodpoveda");
  }

  if (okEns) {
    Serial.printf("ENS160:   AQI %u   TVOC %u ppb   eCO2 %u ppm   stav %u (0 = OK, 1 = zahrieva sa, 2 = prvy start)\n",
                  ens.getAQI(), ens.getTVOC(), ens.getECO2(), ens.getENS160Status());
  } else {
    Serial.println("ENS160:   senzor neodpoveda");
  }

  int raw = analogRead(PIN_AO);
  bool mokro = digitalRead(PIN_DO) == LOW;
  Serial.printf("Dazd:     raw %d   %s\n", raw, mokro ? "MOKRO" : "sucho");

  delay(5000);
}
