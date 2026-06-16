#ifndef INFINITIVE_BUS_H
#define INFINITIVE_BUS_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include <queue>

// Protocol constants
#define DEV_SAM 0x4000
#define DEV_TSTAT 0x4000

#define READ_TABLE_BLOCK 0x60
#define WRITE_TABLE_BLOCK 0x61
#define ACK_06 0x06
#define ACK_02 0x02

#define RESPONSE_TIMEOUT 3000  // milliseconds
#define RESPONSE_RETRIES 3
#define SERIAL_BUFFER_SIZE 2048

struct Frame {
  uint16_t dst;
  uint16_t src;
  uint8_t op;
  uint8_t dataLen;
  uint8_t data[256];
  
  Frame() : dst(0), src(0), op(0), dataLen(0) {}
  
  // Encode frame to bytes
  void encode(uint8_t* buf, uint16_t& len);
  
  // Decode frame from bytes
  bool decode(const uint8_t* buf, uint16_t bufLen);
  
  // Calculate CRC16 checksum
  static void calcChecksum(const uint8_t* data, uint16_t len, uint8_t* checksum);
};

struct Action {
  Frame requestFrame;
  Frame responseFrame;
  bool ok;
  unsigned long timeout;
};

class InfinitiveBus {
  public:
    InfinitiveBus();
    
    // Initialize the bus
    void begin(HardwareSerial* port);
    
    // Read table from device
    bool readTable(uint16_t device, const uint8_t* tableAddr, uint8_t* response, uint16_t& respLen);
    
    // Write table to device
    bool writeTable(uint16_t device, const uint8_t* tableAddr, const uint8_t* data, uint16_t len);
    
    // Send frame and wait for response
    bool sendFrame(const Frame& frame, Frame& response);
    
  private:
    HardwareSerial* _port;
    uint8_t _serialBuffer[SERIAL_BUFFER_SIZE];
    uint16_t _bufferLen;
    
    // Frame processing
    void _processFrame(const Frame& frame);
    bool _readFromSerial();
    void _sendFrame(const Frame& frame);
};

#endif
