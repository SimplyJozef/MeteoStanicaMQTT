// Program číta kvalitu vzduchu zo senzora CCS811: TVOC (prchavé organické látky) a eCO2 (odhad CO2 vypočítaný z TVOC, nie skutočné meranie CO2).

#include <Wire.h>
#include <Adafruit_CCS811.h>

Adafruit_CCS811 ccs;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!ccs.begin(0x5A)) {
    Serial.println("CCS811 sa nenasiel)");
    while (true) delay(1000);
  }
  Serial.println("CCS811 OK");
}

void loop() {
  if (ccs.available()) {
    if (ccs.readData() == 0) {
      Serial.printf("eCO2: %4u ppm   TVOC: %4u ppb\n", ccs.geteCO2(), ccs.getTVOC());
    } else {
      Serial.println("Chyba citania");
    }
  }
  delay(1000);
}
