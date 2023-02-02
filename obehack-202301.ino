// IKEA Obegraensad sketch 2023-01

// ESP8266 Feather pinout
#define LEDARRAY_CLA 4
#define LEDARRAY_CLK 5
#define LEDARRAY_DI 2
#define LEDARRAY_EN 16

void setup() {
  pinMode(LEDARRAY_CLA, OUTPUT);
  pinMode(LEDARRAY_CLK, OUTPUT);
  pinMode(LEDARRAY_DI, OUTPUT);
  pinMode(LEDARRAY_EN, OUTPUT);

  // turn off display
  digitalWrite(LEDARRAY_EN, HIGH);
}

void loop() {
  delay(100);
}
