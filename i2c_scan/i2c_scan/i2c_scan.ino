// Program prehľadá obe I2C zbernice (piny 21/22 a 25/26) a vypíše adresy senzorov na každej z nich. Opakuje to každých 5 sekúnd.

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
  Wire1.begin(25, 26);
}

void loop() {
  Serial.println("--- Zbernica 1 (21/22) ---");
  for (uint8_t adresa = 8; adresa < 120; adresa++) {
    Wire.beginTransmission(adresa);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Najdene zariadenie na 0x%02X\n", adresa);
    }
  }

  Serial.println("--- Zbernica 2 (25/26) ---");
  for (uint8_t adresa = 8; adresa < 120; adresa++) {
    Wire1.beginTransmission(adresa);
    if (Wire1.endTransmission() == 0) {
      Serial.printf("Najdene zariadenie na 0x%02X\n", adresa);
    }
  }

  Serial.println();
  delay(5000);
}
