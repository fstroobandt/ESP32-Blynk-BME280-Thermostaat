#include <LiquidCrystal.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#define HEATING_LED 13
#define AIRCO_LED 12
#define POTM 34

char auth[] = " ";// zet hier uw Blynk authenticatiecode
char ssid[] = " "; // zet hier het SSID van uw WiFi netwerk
char psw[] = " "; // zet hier het wachtwoord van uw WiFi netwerk
Adafruit_BME280 bme;
BlynkTimer timer;
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
  WiFi.begin(ssid, psw);

  lcd.begin(16, 2);
  lcd.clear();


  Serial.begin(9600);
  bool status;
  status = bme.begin(0x76);

  if (!status) {
    Serial.println("Couldn't find valid BME");
    while (1);
  }

  while (WiFi.status() != WL_CONNECTED) {
    // delay(500);
    Serial.print(".");
  }
  Blynk.begin(auth, ssid, psw);
  timer.setInterval(500L, sensorDataSend);
  timer.setInterval(600L, BME280Send);
}

float blynk_temp = 0;

BLYNK_WRITE(V0) {
  blynk_temp = (param.asFloat());
}

bool device_connected = false;

BLYNK_APP_CONNECTED() {
  device_connected = true;
}

BLYNK_APP_DISCONNECTED() {
  device_connected = false;
}

void BME280Send() {
  actual_t = bme.readTemperature();
  lcd.setCursor(0, 1);
  lcd.print("Act= ");
  lcd.print(actual_t);
  lcd.print(" *C");

  Blynk.virtualWrite(V2, actual_t);
}

void sensorDataSend() {
  if (device_connected) {
    desired_t = blynk_temp;
  } else {
    desired_t = desired_temp();
  }
  lcd.setCursor(0, 0);
  lcd.print("Des= ");
  lcd.print(desired_t);
  lcd.print(" *C");

  if (actual_t < desired_t - 0.5) {
    digitalWrite(AIRCO_LED, LOW);
    digitalWrite(HEATING_LED, HIGH);
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 1023);
  } else if (actual_t > desired_t + 1.5) {

    digitalWrite(AIRCO_LED, HIGH);
    digitalWrite(HEATING_LED, LOW);
    Blynk.virtualWrite(V3, 1023);
    Blynk.virtualWrite(V4, 0);
  } else {
    digitalWrite(AIRCO_LED, LOW);
    digitalWrite(HEATING_LED, LOW);
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 0);
  }
  Blynk.virtualWrite(V1, desired_t);
}

void loop() {
  Blynk.run();
  timer.run();
}
