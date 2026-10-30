#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>

// Bê nguyên định nghĩa chân từ file PinConfig của bạn sang đây để chạy độc lập
#define LORA_SS_PIN    5   // Chân CS / NSS
#define LORA_RST_PIN   14  // Chân Reset
#define LORA_DIO0_PIN  2   // Chân DIO0

#define LORA_SCK_PIN   18  // SPI Clock
#define LORA_MISO_PIN  19  // SPI MISO
#define LORA_MOSI_PIN  23  // SPI MOSI

void setup() {
  Serial.begin(115200);
  delay(1500); // Chờ Serial Monitor mở hẳn
  
  Serial.println("\n==================================");
  Serial.println("    TEST DOC LAP MODULE LORA");
  Serial.println("==================================");

  // 1. Cấu hình và khởi tạo chân SPI cho ESP32
  Serial.println("[1] Dang khoi tao bus SPI...");
  SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_SS_PIN);
  LoRa.setSPI(SPI);

  // 2. Gán các chân điều khiển cho thư viện LoRa
  Serial.println("[2] Dang gan chan LoRa (CS=5, RST=14, DIO0=2)...");
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_DIO0_PIN);

  // 3. Khởi tạo tần số 433MHz
  Serial.println("[3] Dang giao tiep voi chip LoRa (begin 433E6)...");
  if (!LoRa.begin(433E6)) {
    Serial.println("---------------------------------------------");
    Serial.println("[LOI FATAL] KHONG THE GIAO TIEP VOI LORA!");
    Serial.println("Cac nguyen nhan thuong gap:");
    Serial.println(" - Nguon: Module chua duoc cap nguon 3.3V (Tuyet doi khong cam 5V).");
    Serial.println(" - Đau day: MISO va MOSI bi cam nguoc vao nhau.");
    Serial.println(" - Day lỏng: Mot trong cac day SCK, MISO, MOSI, CS bi đut ngam hoac lỏng.");
    Serial.println(" - Module hu hỏng: IC LoRa da bi chet.");
    Serial.println("---------------------------------------------");
    while (true) {
      delay(1000); // Treo vĩnh viễn ở đây nếu lỗi
    }
  }

  Serial.println("[THANH CONG] Khoi tao LoRa hoan tat!");
  
  // Tùy chỉnh thông số phát (giống code chính của bạn)
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setTxPower(17);
}

void loop() {
  Serial.print("Dang gui goi tin test... ");
  
  // Mở gói tin
  LoRa.beginPacket();
  LoRa.print("Hello LoRa - Uptime: ");
  LoRa.print(millis());
  
  // endPacket() trả về 1 nếu gửi thành công, 0 nếu thất bại
  int result = LoRa.endPacket();

  if (result == 1) {
    Serial.println(" -> Thanh cong!");
  } else {
    Serial.println(" -> THAT BAI (Loi TX)!");
  }

  delay(3000);
}
