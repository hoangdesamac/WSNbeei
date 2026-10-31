#include "SoilMoisture.h"

SoilMoisture::SoilMoisture(uint8_t pinAO, uint8_t pinDO) {
  _pinAO = pinAO;
  _pinDO = pinDO;
}

void SoilMoisture::begin(float thetaSat, float thetaDry, int adcSat,
                         int adcDry) {
  _thetaSat = thetaSat;
  _thetaDry = thetaDry;
  _adcSat = adcSat;
  _adcDry = adcDry;

  pinMode(_pinAO, INPUT);
  if (_pinDO != 255) {
    pinMode(_pinDO, INPUT);
  }
}

// Bổ sung đọc lọc trung bình nhiều mẫu để chống trôi/nhiễu
int SoilMoisture::readRawADC(uint8_t samples) {
  long sum = 0;
  for (uint8_t i = 0; i < samples; i++) {
    sum += analogRead(_pinAO);
    delay(2);
  }
  return (int)(sum / samples);
}

// Giữ nguyên 100% logic công thức tính theta gốc của bạn
float SoilMoisture::readTheta() {
  int adc = readRawADC();
  float theta =
      ((_thetaSat - _thetaDry) / (float)(_adcSat - _adcDry)) * (adc - _adcDry) +
      _thetaDry;
  return theta;
}

int SoilMoisture::readDigital() {
  if (_pinDO != 255) {
    return digitalRead(_pinDO);
  }
  return -1;
}