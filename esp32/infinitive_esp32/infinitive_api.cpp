#include "infinitive_api.h"
#include "infinitive_conversions.h"

InfinitivApi::InfinitivApi(InfinitiveBus* bus) : _bus(bus) {}

void InfinitivApi::getTstatSettings(JsonDocument& doc) {
  // Return cached thermostat settings
  if (_cache.containsKey("tstat")) {
    doc = _cache["tstat"];
  }
}

bool InfinitivApi::getConfig(int zone, JsonDocument& doc) {
  if (zone < 1 || zone > 8) {
    return false;
  }
  
  // Read zone parameters from device
  uint8_t tstatAddr[3] = {0x00, 0x27, 0x01};  // TStatZoneParams table address
  uint8_t response[256];
  uint16_t respLen = 0;
  
  if (_bus->readTable(DEV_TSTAT, tstatAddr, response, respLen)) {
    doc["zone"] = zone;
    doc["currentTemp"] = response[zone * 2];  // Example parsing
    doc["mode"] = "auto";
    doc["fanMode"] = "auto";
    doc["hold"] = false;
    doc["heatSetpoint"] = 68;
    doc["coolSetpoint"] = 78;
    return true;
  }
  
  return false;
}

void InfinitivApi::updateZoneConfig(int zone, const JsonDocument& config) {
  if (zone < 1 || zone > 8) {
    return;
  }
  
  // Update zone settings
  if (config.containsKey("fanMode")) {
    const char* fanMode = config["fanMode"];
    uint8_t mode = stringFanModeToRaw(fanMode);
    Serial.printf("Setting zone %d fan mode to %d\n", zone, mode);
  }
  
  if (config.containsKey("heatSetpoint")) {
    uint8_t setpoint = config["heatSetpoint"];
    Serial.printf("Setting zone %d heat setpoint to %d\n", zone, setpoint);
  }
  
  if (config.containsKey("coolSetpoint")) {
    uint8_t setpoint = config["coolSetpoint"];
    Serial.printf("Setting zone %d cool setpoint to %d\n", zone, setpoint);
  }
}

bool InfinitivApi::getAirHandler(JsonDocument& doc) {
  if (_cache.containsKey("airHandler")) {
    doc = _cache["airHandler"];
    return true;
  }
  return false;
}

bool InfinitivApi::getHeatPump(JsonDocument& doc) {
  if (_cache.containsKey("heatPump")) {
    doc = _cache["heatPump"];
    return true;
  }
  return false;
}

bool InfinitivApi::getVacation(JsonDocument& doc) {
  uint8_t vacationAddr[3] = {0x00, 0x3B, 0x04};  // Vacation params address
  uint8_t response[256];
  uint16_t respLen = 0;
  
  if (_bus->readTable(DEV_TSTAT, vacationAddr, response, respLen)) {
    doc["active"] = response[0] == 1;
    doc["days"] = response[1];
    doc["minTemperature"] = response[2];
    doc["maxTemperature"] = response[3];
    doc["minHumidity"] = response[4];
    doc["maxHumidity"] = response[5];
    doc["fanMode"] = rawFanModeToString(response[6]);
    return true;
  }
  
  return false;
}

void InfinitivApi::updateVacation(const JsonDocument& config) {
  Serial.println("Updating vacation settings...");
  
  if (config.containsKey("days")) {
    Serial.printf("Vacation days: %d\n", (int)config["days"]);
  }
}

bool InfinitivApi::getTableRaw(uint16_t device, const uint8_t* tableAddr, JsonDocument& doc) {
  uint8_t response[256];
  uint16_t respLen = 0;
  
  if (_bus->readTable(device, tableAddr, response, respLen)) {
    // Convert response to hex string
    char hexStr[512] = {0};
    for (uint16_t i = 0; i < respLen && i < 256; i++) {
      sprintf(&hexStr[i*2], "%02x", response[i]);
    }
    
    doc["response"] = hexStr;
    return true;
  }
  
  return false;
}

void InfinitivApi::updateCache() {
  // Periodically update cache from device
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate < 5000) {
    return;  // Update every 5 seconds
  }
  
  lastUpdate = millis();
  
  // Update air handler
  DynamicJsonDocument ahDoc(256);
  uint8_t ahAddr[3] = {0x00, 0x04, 0x00};  // Air handler address
  uint8_t response[256];
  uint16_t respLen = 0;
  
  if (_bus->readTable(DEV_TSTAT, ahAddr, response, respLen)) {
    ahDoc["blowerRPM"] = (response[1] << 8) | response[2];
    ahDoc["airFlowCFM"] = (response[4] << 8) | response[5];
    ahDoc["elecHeat"] = (response[0] & 0x03) != 0;
    _cache["airHandler"] = ahDoc;
  }
}

void InfinitivApi::dumpCache(JsonDocument& doc) {
  doc = _cache;
}
