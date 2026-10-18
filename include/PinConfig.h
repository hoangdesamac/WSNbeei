#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H
// I2C Pins
#define I2C_SDA_PIN    21   // Chân dữ liệu I2C
#define I2C_SCL_PIN    22   // Chân xung clock I2C
// Định nghĩa chân kết nối cảm biến độ ẩm đất
#define humSensorDO 25
#define humSensorAO 26
// Địa chỉ I2C của các cảm biến
#define ADXL345_I2C_ADDR 0x53 // Địa chỉ I2C mặc định ADXL345
#define GY271_I2C_ADDR 0x0D   // Địa chỉ I2C mặc định GY-271 (QMC5883L)
// Định nghĩa chân kết nối LoRa
#define LORA_SS_PIN 5   // Chân CS / NSS (Chip Select)
#define LORA_RST_PIN 14 // Chân Reset
#define LORA_DIO0_PIN 2 // Chân ngắt DIO0 (Interrupt)

#define LORA_SCK_PIN 18  // SPI Clock
#define LORA_MISO_PIN 19 // SPI Master In Slave Out
#define LORA_MOSI_PIN 23 // SPI Master Out Slave In

#endif