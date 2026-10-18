// Program prehľadá I2C zbernicu a vypíše adresy všetkých pripojených senzorov. Opakuje to každých 5 sekúnd.

#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);
}

void loop() {
  Serial.println("--- I2C scan ---");
  int pocet = 0;
  for (uint8_t adresa = 8; adresa < 120; adresa++) {
    Wire.beginTransmission(adresa);
    if (Wire.endTransmission() == 0) {
      Serial.printf("Najdene zariadenie na 0x%02X\n", adresa);
      pocet++;
    }
  }
  Serial.printf("Spolu: %d\n\n", pocet);
  delay(5000);
}
