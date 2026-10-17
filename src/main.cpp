#include <Arduino.h>
#include <soilmoisture.h>
#define humSensorDO 25
#define humSensorAO 26

SoilMoisture humSensor(humSensorAO, humSensorDO);

void setup() {
  Serial.begin(115200);

  pinMode(humSensorDO, INPUT);
  pinMode(humSensorAO, INPUT);

  // ESP32 ADC: do phan giai 12 bit (0-4095)
  analogReadResolution(12);
  humSensor.begin(0.40, 0.05, 1100, 3200);
}

void loop() {
  Serial.print("ADC Raw: ");
  Serial.println(humSensor.readRawADC());

  Serial.print("Theta: ");
  Serial.println(humSensor.readTheta());

  Serial.print("DO State: ");
  Serial.println(humSensor.readDigital());

  Serial.println("--------------------");
  delay(1000);
}
