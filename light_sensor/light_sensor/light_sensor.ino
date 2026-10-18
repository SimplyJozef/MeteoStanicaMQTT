// Každú pol sekundu prečítam intenzitu svetla zo senzora GY-302 a vypíše ju v luxoch.

#include <Wire.h>

const uint8_t ADRESA = 0x23;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  Wire.beginTransmission(ADRESA);
  Wire.write(0x10);
  Wire.endTransmission();
}

void loop() {
  Wire.requestFrom(ADRESA, (uint8_t)2);
  if (Wire.available() == 2) {
    uint16_t raw = (Wire.read() << 8) | Wire.read();
    Serial.printf("Svetlo: %.1f lx\n", raw / 1.2);
  } else {
    Serial.println("Senzor neodpoveda");
  }
  delay(500);
}