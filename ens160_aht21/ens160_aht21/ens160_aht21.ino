// Každú sekundu prečítam kvalitu vzduchu zo senzora ENS160 (AQI 1–5, TVOC, eCO2) a teplotu s vlhkosťou z AHT21.
// Modul je na druhej I2C zbernici (piny 25/26). eCO2 je odhad z VOC, nie skutočné CO2.


#include <Wire.h>
#include <DFRobot_ENS160.h>
#include <Adafruit_AHTX0.h>

DFRobot_ENS160_I2C ens(&Wire1, 0x53);
Adafruit_AHTX0 aht;

void setup() {
  Serial.begin(115200);
  Wire1.begin(25, 26);

  if (!aht.begin(&Wire1)) Serial.println("AHT21 sa nenasiel");
  else Serial.println("AHT21 OK");

  if (ens.begin() != 0) Serial.println("ENS160 sa nenasiel");
  else Serial.println("ENS160 OK");

  ens.setPWRMode(ENS160_STANDARD_MODE);
}

void loop() {
  sensors_event_t vlhkost, teplota;
  aht.getEvent(&vlhkost, &teplota);
  ens.setTempAndHum(teplota.temperature, vlhkost.relative_humidity);

  Serial.printf("AQI: %u   TVOC: %4u ppb   eCO2: %4u ppm   |   AHT21: %.2f C  %.1f %%RH   stav: %u\n",
                ens.getAQI(), ens.getTVOC(), ens.getECO2(),
                teplota.temperature, vlhkost.relative_humidity, ens.getENS160Status());
  delay(1000);
}