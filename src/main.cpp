#include "HardwareSerial.h"
#include <Arduino.h>

// 1. Thư viện nâng cao cho ADXL345
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_Sensor.h>

// 2. Thư viện nâng cao cho GY-271 (QMC5883L)
#include <QMC5883LCompass.h>

// 3. Thư viện LoRa
#include <LoRa.h>
#include <PinConfig.h>
#include <cstdint>

// 4. Thư viện Cảm biến độ ẩm đất (Do bạn tự viết trong lib/SoilMoisture)
#include <soilmoisture.h>

SoilMoisture humSensor(humSensorAO, humSensorDO);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
QMC5883LCompass compass;

typedef struct __attribute__((packed)) {
  uint16_t packetId;
  float soilTheta;
  float accelX;
  float accelY;
  float accelZ;
  int compassAzimuth;
} SensorPayload;

SensorPayload payload;
uint16_t packetCounter = 0;

void setup() {
  Serial.begin(115200);

  // ESP32 ADC: do phan giai 12 bit (0-4095)
  analogReadResolution(12);

  pinMode(humSensorDO, INPUT);
  pinMode(humSensorAO, INPUT);

  // Cấu hình cảm biến độ ẩm đất
  humSensor.begin(0.40, 0.05, 1100, 3200);

  // Tao ra I2C bus
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  if (!accel.begin()) {
    /* code */
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
  } else {
    accel.setRange(ADXL345_RANGE_2_G);
  }
  compass.init();

  // Cấu hình LoRa
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);

  // Cấu hình soil moisture sensor
  if (!LoRa.begin(433E6)) {
    /* code */
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }

  // Cấu hình các thông số LoRa
  LoRa.setSpreadingFactor(7);
  LoRa.setTxPower(20);
  Serial.println("LoRa Initial OK!");
}

void loop() {
  packetCounter++;

  // Đọc giá trị độ ẩm đất
  payload.soilTheta = humSensor.readTheta();

  // Giá trị compass trả về là độ (0-360)
  sensors_event_t event;
  accel.getEvent(&event);
  payload.accelX = event.acceleration.x;
  payload.accelY = event.acceleration.y;
  payload.accelZ = event.acceleration.z;

  // Lấy giá trị Azimuth từ cảm biến la bàn QMC5883L
  compass.read();
  payload.compassAzimuth = compass.getAzimuth();

  payload.packetId = packetCounter;
  Serial.printf("Gửi Packet #%d | Theta: %.2f | X: %.2f | Azimuth: %d\n",
                payload.packetId, payload.soilTheta, payload.accelX,
                payload.compassAzimuth);
  LoRa.beginPacket();
  LoRa.write((uint8_t *)&payload, sizeof(SensorPayload));
  LoRa.endPacket();

  delay(1000);
}
