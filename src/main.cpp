#include <Arduino.h>

#define humSensorDO 25
#define humSensorAO 26

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}
void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Hello, World!");
  delay(1000);
}