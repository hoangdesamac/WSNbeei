#ifndef SOIL_MOISTURE_H
#define SOIL_MOISTURE_H

#include <Arduino.h>

class SoilMoisture {
private:
  uint8_t _pinAO;
  uint8_t _pinDO;
  float _thetaSat;
  float _thetaDry;
  int _adcSat;
  int _adcDry;

public:
  // Constructor khởi tạo chân cắm
  SoilMoisture(uint8_t pinAO, uint8_t pinDO = 255);

  // Cấu hình thông số hiệu chuẩn
  void begin(float thetaSat, float thetaDry, int adcSat, int adcDry);

  // Đọc giá trị ADC
  int readRawADC();

  // Tính độ ẩm theta
  float readTheta();

  // Đọc trạng thái Digital (nếu có dùng)
  int readDigital();
};

#endif