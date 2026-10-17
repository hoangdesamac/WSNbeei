#include "SoilMoisture.h"

SoilMoisture::SoilMoisture(uint8_t pinAO, uint8_t pinDO) {
  _pinAO = pinAO;
  _pinDO = pinDO;
  _thetaSat = 0.4;
  _thetaDry = 0.05;
  _adcSat = 1100;
  _adcDry = 3200;
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

int SoilMoisture::readRawADC() { return analogRead(_pinAO); }

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