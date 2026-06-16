#ifndef INFINITIVE_API_H
#define INFINITIVE_API_H

#include <ArduinoJson.h>
#include "infinitive_bus.h"

class InfinitivApi {
  public:
    InfinitivApi(InfinitiveBus* bus);
    
    // Get thermostat settings
    void getTstatSettings(JsonDocument& doc);
    
    // Get zone configuration
    bool getConfig(int zone, JsonDocument& doc);
    
    // Update zone configuration
    void updateZoneConfig(int zone, const JsonDocument& config);
    
    // Get air handler status
    bool getAirHandler(JsonDocument& doc);
    
    // Get heat pump status
    bool getHeatPump(JsonDocument& doc);
    
    // Get vacation settings
    bool getVacation(JsonDocument& doc);
    
    // Update vacation settings
    void updateVacation(const JsonDocument& config);
    
    // Get raw table data
    bool getTableRaw(uint16_t device, const uint8_t* tableAddr, JsonDocument& doc);
    
    // Update cache from device
    void updateCache();
    
    // Dump cache
    void dumpCache(JsonDocument& doc);
    
  private:
    InfinitiveBus* _bus;
    JsonDocument _cache;
};

#endif
