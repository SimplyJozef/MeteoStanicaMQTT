// Program každú pol sekundu prečíta dažďový senzor MH-RD: analógovú hodnotu (mieru zvlhnutia platničky) a digitálny stav mokro/sucho.
// Hodnota nie je v mm zrážok, je to len surové číslo 0–4095 (čím menšie, tým mokrejšie).

const int PIN_AO = 34;
const int PIN_DO = 35;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_DO, INPUT);
}

void loop() {
  int raw = analogRead(PIN_AO);
  bool mokro = digitalRead(PIN_DO) == LOW;

  Serial.printf("Dazd raw: %4d   Stav: %s\n", raw, mokro ? "MOKRO" : "sucho");
  delay(500);
}