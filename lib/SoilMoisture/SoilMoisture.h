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

  // Cấu hình thông số hiệu chuẩn (Gán mặc định theo bài báo & mốc đo thực tế)
  void begin(float thetaSat = 0.400, float thetaDry = 0.067, int adcSat = 1050, int adcDry = 4095);

  // Đọc giá trị ADC có lọc lấy trung bình (Mặc định 15 mẫu)
  int readRawADC(uint8_t samples = 15);

  // Tính độ ẩm theta (giữ nguyên logic công thức gốc của bạn)
  float readTheta();

  // Đọc trạng thái Digital
  int readDigital();
};

#endif