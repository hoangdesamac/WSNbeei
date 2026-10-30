#pragma once
#include <Arduino.h>
#include <stdint.h>

static constexpr uint16_t WSN_MAGIC = 0xA51C;

enum PacketType : uint8_t {
  PACKET_DATA = 1,
  PACKET_ACK  = 2
};

struct __attribute__((packed)) WSNPacket {
  uint16_t magic;
  uint8_t type;
  uint32_t sessionId;
  uint32_t packetId;

  float soilTheta;
  float accelX;
  float accelY;
  float accelZ;
  int16_t compassAzimuth;

  float distanceM;
};
