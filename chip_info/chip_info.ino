// Zistíme aký presne čip je na ESP32, základné údaje. Údaje priamo ovplyvňujú výkon TLS, rýhlosť handshaku a spotreba pamäte

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== ESP32 info ===");
  Serial.printf("Model:     %s\n", ESP.getChipModel());
  Serial.printf("Revizia:   %d\n", ESP.getChipRevision());
  Serial.printf("Jadra:     %d\n", ESP.getChipCores());
  Serial.printf("CPU:       %lu MHz\n", ESP.getCpuFreqMHz());
  Serial.printf("Flash:     %lu B\n", ESP.getFlashChipSize());
  Serial.printf("Volna RAM: %lu B\n", ESP.getFreeHeap());
  Serial.printf("SDK:       %s\n", ESP.getSdkVersion());
}

void loop() {
}