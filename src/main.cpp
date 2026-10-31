#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "SoilMoisture.h"
#include <math.h>

// =================================================================
// 1. CẤU HÌNH CHÂN GIAO TIẾP LORA (BÊ NGUYÊN TỪ PHẦN TEST)
// =================================================================
#define LORA_SS_PIN    5   // Chân CS / NSS
#define LORA_RST_PIN   14  // Chân Reset
#define LORA_DIO0_PIN  2   // Chân DIO0

#define LORA_SCK_PIN   18  // SPI Clock
#define LORA_MISO_PIN  19  // SPI MISO
#define LORA_MOSI_PIN  23  // SPI MOSI

// =================================================================
// 2. KHAI BÁO CẢM BIẾN ĐỘ ẨM VÀ THÔNG SỐ ĐỊA CHẤT (TỪ THUYET-MINH.PDF)
// =================================================================
SoilMoisture soilSensor(26); // Cảm biến độ ẩm nối chân G26

// Thông số địa chất Lớp 1 (1m - 5m): Sét pha
const float SOIL_COHESION = 13.0; // c' (kPa)
const float PHI_DEG       = 9.45; // Phi (độ)
const float GAMMA_DRY     = 11.04;// Dung trọng khô (kN/m3)
const float Z_DEPTH       = 2.0;  // Chiều sâu lớp đất trượt z (m)
const float SLOPE_BETA    = 30.0; // Góc dốc mái Beta (độ)

// Thông số van Genuchten (SWCC) cho đất sét pha
const float THETA_S = 0.400; // Ngưỡng độ ẩm bão hòa
const float THETA_R = 0.067; // Ngưỡng độ ẩm dư
const float VG_N    = 1.89;  // Tham số n
const float VG_M    = 1.0 - (1.0 / VG_N); // Tham số m = 1 - 1/n
const float ALPHA_KPA = 0.075 / 0.0981;   // Alpha quy đổi sang 1/kPa

// Biến lưu trữ chuỗi mưa API
float rain_API = 0.0;
const float K_DECAY = 0.90; // Ngưỡng suy giảm k_decay

// =================================================================
// 3. HÀM TÍNH TOÁN FOS_PHYSICS (MÔ HÌNH VẬT LÝ UNSSATURATED SLOPE)
// =================================================================
float calculateFoS(float theta) {
    // Kẹp theta trong dải an toàn [THETA_R, THETA_S]
    float theta_clamped = constrain(theta, THETA_R + 0.001f, THETA_S);

    // Tính độ bão hòa hiệu dụng Se
    float Se = (theta_clamped - THETA_R) / (THETA_S - THETA_R);
    Se = constrain(Se, 0.01f, 1.0f);

    // Tính Matric Suction (kPa) theo van Genuchten
    // Suction = (1/alpha) * (Se^(-1/m) - 1)^(1/n)
    float suction = (1.0f / ALPHA_KPA) * pow(pow(Se, -1.0f / VG_M) - 1.0f, 1.0f / VG_N);

    // Tính Suction Stress Suction_Stress = -Se * Suction
    float ss_mag = Se * suction; // Độ lớn Suction Stress tăng cường sức chống cắt

    // Dung trọng tự nhiên theo độ ẩm: gamma = gamma_d + gamma_w * theta
    float gamma = GAMMA_DRY + 9.81f * theta_clamped;

    // Đổi góc sang Radian
    float beta_rad = SLOPE_BETA * M_PI / 180.0f;
    float phi_rad  = PHI_DEG * M_PI / 180.0f;

    // Tính Ứng suất cắt gây trượt (tau) và Sức chống cắt (tau_f)
    float cos_b = cos(beta_rad);
    float sin_b = sin(beta_rad);

    float tau_stress = gamma * Z_DEPTH * sin_b * cos_b;
    float normal_stress = gamma * Z_DEPTH * cos_b * cos_b;

    // Sức chống cắt Mohr-Coulomb mở rộng cho đất không bão hòa
    float tau_strength = SOIL_COHESION + (normal_stress + ss_mag) * tan(phi_rad);

    if (tau_stress <= 0.0001f) return 99.0f; // Tránh chia cho 0
    return tau_strength / tau_stress;
}

// =================================================================
// 4. KHỞI TẠO THIẾT BỊ
// =================================================================
void setup() {
    Serial.begin(115200);
    delay(1500);

    Serial.println("\n==============================================");
    Serial.println("  ESP32 WSN NODE - LORA & FoS COMPUTATION");
    Serial.println("==============================================");

    // 1. Khởi tạo Cảm biến độ ẩm
    soilSensor.begin();
    Serial.println("[1] Khoi tao cam bien do am G26 ok!");

    // 2. Khởi tạo Bus SPI và Module LoRa
    Serial.println("[2] Khoi tao bus SPI & gan chan LoRa...");
    SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_SS_PIN);
    LoRa.setSPI(SPI);
    LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);

    Serial.println("[3] Dang ket noi LoRa 433MHz...");
    if (!LoRa.begin(433E6)) {
        Serial.println("[LOI FATAL] KHONG THE KET NOI MODULE LORA!");
        while (true) delay(1000);
    }

    // Cấu hình thông số phát LoRa
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.setTxPower(17);

    // Khởi tạo hạt giống ngẫu nhiên
    randomSeed(analogRead(34));

    Serial.println("[THANH CONG] He thong san sang truyen du lieu!");
}

// =================================================================
// 5. VÒNG LẶP CHÍNH
// =================================================================
void loop() {
    // 1. Đọc độ ẩm đất thực tế từ cảm biến (Soil_Water)
    float soil_water = soilSensor.readTheta();

    // 2. Tạo lượng mưa ngẫu nhiên Rain_1h (mm/h) mô phỏng chuỗi thời tiết
    // Tạo mưa giả định (ví dụ 0mm đến 25mm/h)
    float rain_1h = (random(0, 100) > 70) ? (random(10, 250) / 10.0f) : 0.0f;

    // 3. Tính lượng mưa tích lũy Rain_API với k_decay = 0.90
    rain_API = (K_DECAY * rain_API) + rain_1h;

    // 4. Tính toán hệ số an toàn FoS_Physics
    float fos_physics = calculateFoS(soil_water);

    // 5. Đóng gói dữ liệu dạng JSON gửi về Raspberry Pi
    String payload = "{";
    payload += "\"Rain_1h\":" + String(rain_1h, 2) + ",";
    payload += "\"Rain_API\":" + String(rain_API, 2) + ",";
    payload += "\"Soil_Water\":" + String(soil_water, 4) + ",";
    payload += "\"Soil_Cohesion\":" + String(SOIL_COHESION, 2) + ",";
    payload += "\"FoS_Physics\":" + String(fos_physics, 3);
    payload += "}";

    // 6. Gửi gói tin qua LoRa
    Serial.print("Dang gui goi tin LoRa: ");
    Serial.println(payload);

    LoRa.beginPacket();
    LoRa.print(payload);
    int status = LoRa.endPacket();

    if (status == 1) {
        Serial.println(" -> Gui thanh cong toi Pi!");
    } else {
        Serial.println(" -> Gui THAT BAI!");
    }

    // Đợi 5 giây cho chu kỳ tiếp theo
    delay(5000);
}