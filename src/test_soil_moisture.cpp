#include <Arduino.h>
#include "SoilMoisture.h"

// Khai báo cảm biến nối chân G26
SoilMoisture soilSensor(26);

void setup() {
    Serial.begin(115200);
    
    // Gọi begin() không tham số để tự động nhận các giá trị mặc định (0.400, 0.067, 1050, 4095)
    soilSensor.begin();
    
    Serial.println("--- DEMO DO THETA DO AM DAT (DA LOC NHIEU) ---");
}

void loop() {
    int rawADC = soilSensor.readRawADC();
    float theta = soilSensor.readTheta();
    
    Serial.print("ADC Lọc: ");
    Serial.print(rawADC);
    Serial.print(" | VWC (Theta): ");
    Serial.println(theta, 4);
    
    delay(1000);
}