#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define HEATING_LED 13
#define AIRCO_LED 12
#define POTM 34

Adafruit_BME280 bme;
const int rs = 15, en = 23, d4 = 5, d5 = 18, d6 = 19, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

float actual_t;
float desired_t;

float desired_temp() {
  float foo = map(analogRead(POTM), 0, 4096, 000, 3000);
  return foo / 100;
}

void setup() {
  pinMode(AIRCO_LED, OUTPUT);
  pinMode(HEATING_LED, OUTPUT);

  lcd.begin(16, 2);
  lcd.clear();

  bool status;
  status = bme.begin(0x76);

  if (!status) {
    Serial.println("Couldn't find valid BME");
    while (1);
  }
}

void loop() {
  desired_t = desired_temp();
  actual_t = bme.readTemperature();
  
  lcd.setCursor(0, 1);
  lcd.print("Act= ");
  lcd.print(actual_t);
  lcd.print(" *C");

  lcd.setCursor(0, 0);
  lcd.print("Des= ");
  lcd.print(desired_t);
  lcd.print(" *C");

  if (actual_t < desired_t - 0.5) {
    digitalWrite(AIRCO_LED, LOW);
    digitalWrite(HEATING_LED, HIGH);
  } else if (actual_t > desired_t + 1.5) {
    digitalWrite(AIRCO_LED, HIGH);
    digitalWrite(HEATING_LED, LOW);
  } else {
    digitalWrite(AIRCO_LED, LOW);
    digitalWrite(HEATING_LED, LOW);
  }
}
