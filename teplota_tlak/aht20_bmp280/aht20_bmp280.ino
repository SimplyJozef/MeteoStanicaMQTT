// Každé 2 sekundy prečítam teplotu a vlhkosť z AHT20 a teplotu a tlak z BMP280 a vypíše ich.

#include <Wire.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (!aht.begin()) Serial.println("AHT20 sa nenasiel na 0x38");
  else Serial.println("AHT20 OK");

  Wire.beginTransmission(0x77);
  Wire.write(0xD0);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)0x77, (uint8_t)1);
  uint8_t chipId = Wire.read();
  Serial.printf("Chip ID na 0x77: 0x%02X (0x58 = BMP280, 0x60 = BME280)\n", chipId);

  if (!bmp.begin(0x77, chipId)) Serial.println("BMP280 sa nepodarilo spustit");
  else Serial.println("BMP280 OK");
}

void loop() {
  sensors_event_t vlhkost, teplota;
  aht.getEvent(&vlhkost, &teplota);

  Serial.printf("AHT20:  %.2f C  %.1f %%RH   |   BMP280:  %.2f C  %.2f hPa\n",
                teplota.temperature, vlhkost.relative_humidity,
                bmp.readTemperature(), bmp.readPressure() / 100.0);
  delay(2000);
}
