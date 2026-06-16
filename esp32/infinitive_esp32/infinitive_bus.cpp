#include "infinitive_bus.h"

// CRC16 polynomial for Infinity protocol
#define CRC_POLY 0x1021

void Frame::calcChecksum(const uint8_t* data, uint16_t len, uint8_t* checksum) {
  uint16_t crc = 0x0000;
  
  for (uint16_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (int j = 0; j < 8; j++) {
      crc <<= 1;
      if (crc & 0x10000) {
        crc ^= CRC_POLY;
      }
      crc &= 0xFFFF;
    }
  }
  
  checksum[0] = (uint8_t)(crc >> 8);
  checksum[1] = (uint8_t)(crc & 0xFF);
}

void Frame::encode(uint8_t* buf, uint16_t& len) {
  uint16_t pos = 0;
  
  // Destination
  buf[pos++] = (uint8_t)(dst >> 8);
  buf[pos++] = (uint8_t)dst;
  
  // Source
  buf[pos++] = (uint8_t)(src >> 8);
  buf[pos++] = (uint8_t)src;
  
  // Data length
  buf[pos++] = dataLen;
  
  // Reserved bytes
  buf[pos++] = 0x00;
  buf[pos++] = 0x00;
  
  // Operation
  buf[pos++] = op;
  
  // Data
  memcpy(&buf[pos], data, dataLen);
  pos += dataLen;
  
  // Checksum
  uint8_t checksum[2];
  calcChecksum(buf, pos, checksum);
  buf[pos++] = checksum[0];
  buf[pos++] = checksum[1];
  
  len = pos;
}

bool Frame::decode(const uint8_t* buf, uint16_t bufLen) {
  if (bufLen < 10) {
    return false;
  }
  
  // Check if all zeros
  bool allZeros = true;
  for (uint16_t i = 0; i < bufLen; i++) {
    if (buf[i] != 0) {
      allZeros = false;
      break;
    }
  }
  if (allZeros) {
    return false;
  }
  
  // Verify checksum
  uint8_t expectedChecksum[2];
  calcChecksum(buf, bufLen - 2, expectedChecksum);
  if (expectedChecksum[0] != buf[bufLen - 2] || expectedChecksum[1] != buf[bufLen - 1]) {
    return false;
  }
  
  // Parse frame
  dst = ((uint16_t)buf[0] << 8) | buf[1];
  src = ((uint16_t)buf[2] << 8) | buf[3];
  dataLen = buf[4];
  op = buf[7];
  
  // Extract data
  uint16_t dataStart = 8;
  uint16_t dataEnd = bufLen - 2;
  if (dataEnd > dataStart) {
    memcpy(data, &buf[dataStart], dataEnd - dataStart);
  }
  
  return true;
}

InfinitiveBus::InfinitiveBus() : _port(nullptr), _bufferLen(0) {}

void InfinitiveBus::begin(HardwareSerial* port) {
  _port = port;
  _bufferLen = 0;
}

bool InfinitiveBus::sendFrame(const Frame& frame, Frame& response) {
  // Encode and send frame
  uint8_t encoded[512];
  uint16_t len;
  
  Frame sendFrame = frame;
  sendFrame.src = DEV_SAM;
  sendFrame.encode(encoded, len);
  
  _sendFrame(sendFrame);
  
  // Wait for response
  unsigned long startTime = millis();
  int retries = 0;
  
  while (retries < RESPONSE_RETRIES) {
    // Try to read frame
    if (_readFromSerial()) {
      // Parse frame from buffer
      Frame tempFrame;
      if (tempFrame.decode(_serialBuffer, _bufferLen)) {
        // Check if this is the response we're looking for
        if (tempFrame.src == frame.dst) {
          response = tempFrame;
          _bufferLen = 0;  // Clear buffer
          return true;
        }
      }
    }
    
    if (millis() - startTime > RESPONSE_TIMEOUT) {
      Serial.println("Bus: Timeout waiting for response, retransmitting");
      _sendFrame(sendFrame);
      retries++;
      startTime = millis();
    }
    
    delay(10);
  }
  
  Serial.println("Bus: Action timed out");
  return false;
}

void InfinitiveBus::_sendFrame(const Frame& frame) {
  uint8_t encoded[512];
  uint16_t len;
  
  Frame f = frame;
  f.encode(encoded, len);
  
  if (_port) {
    _port->write(encoded, len);
  }
}

bool InfinitiveBus::_readFromSerial() {
  if (!_port) {
    return false;
  }
  
  while (_port->available()) {
    if (_bufferLen >= SERIAL_BUFFER_SIZE) {
      // Buffer overflow, clear it
      _bufferLen = 0;
    }
    
    uint8_t byte = _port->read();
    _serialBuffer[_bufferLen++] = byte;
    
    // Try to parse frame
    if (_bufferLen >= 10) {
      Frame f;
      if (f.decode(_serialBuffer, _bufferLen)) {
        return true;
      }
    }
  }
  
  return false;
}

bool InfinitiveBus::readTable(uint16_t device, const uint8_t* tableAddr, uint8_t* response, uint16_t& respLen) {
  Frame req;
  req.dst = device;
  req.op = READ_TABLE_BLOCK;
  req.dataLen = 3;
  memcpy(req.data, tableAddr, 3);
  
  Frame resp;
  if (sendFrame(req, resp)) {
    // Copy response data (skip table address bytes)
    if (resp.dataLen > 3) {
      respLen = resp.dataLen - 3;
      memcpy(response, &resp.data[3], respLen);
    } else {
      respLen = 0;
    }
    return true;
  }
  
  return false;
}

bool InfinitiveBus::writeTable(uint16_t device, const uint8_t* tableAddr, const uint8_t* data, uint16_t len) {
  Frame req;
  req.dst = device;
  req.op = WRITE_TABLE_BLOCK;
  req.dataLen = 3 + len;
  
  memcpy(req.data, tableAddr, 3);
  memcpy(&req.data[3], data, len);
  
  Frame resp;
  return sendFrame(req, resp);
}
