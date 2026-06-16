/*
 * Infinitive ESP32 - HVAC Controller for Carrier Infinity Systems
 * Python conversion to Arduino/ESP32
 * 
 * Features:
 * - Serial communication with Carrier Infinity HVAC systems
 * - WiFi REST API endpoints
 * - WebSocket support for real-time updates
 * - Multi-zone thermostat control
 * - Vacation mode settings
 */

#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <HardwareSerial.h>
#include <ArduinoJson.h>

#include "infinitive_bus.h"
#include "infinitive_api.h"
#include "infinitive_conversions.h"

// WiFi Configuration
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// Server Configuration
WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// Serial Configuration
HardwareSerial &serialPort = Serial1;  // Serial1 for ESP32
const int SERIAL_RX_PIN = 16;           // RX pin
const int SERIAL_TX_PIN = 17;           // TX pin
const uint32_t SERIAL_BAUD = 19200;     // Infinity bus speed

// Global instances
InfinitiveBus bus;
InfinitivApi api(&bus);

// Forward declarations
void setupWiFi();
void setupServer();
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n\nInfinitive ESP32 Controller Starting...");
  
  // Initialize Infinity bus serial (RX, TX)
  serialPort.begin(SERIAL_BAUD, SERIAL_8N1, SERIAL_RX_PIN, SERIAL_TX_PIN);
  
  // Initialize bus
  bus.begin(&serialPort);
  
  // Setup WiFi
  setupWiFi();
  
  // Setup web server
  setupServer();
  
  // Start WebSocket
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  Serial.println("Infinitive ESP32 Controller Ready!");
  Serial.print("Connect to WebSocket at: ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81");
}

void loop() {
  server.handleClient();
  webSocket.loop();
  
  // Update API cache periodically
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 1000) {
    api.updateCache();
    lastUpdate = millis();
  }
}

void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("WiFi connected! IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi");
  }
}

void setupServer() {
  // Thermostat settings
  server.on("/api/tstat/settings", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    api.getTstatSettings(doc);
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
  });
  
  // Zone configuration GET
  server.on("/api/zone/config", HTTP_GET, []() {
    String zoneStr = server.arg("zone");
    if (zoneStr == "") {
      server.send(400, "application/json", "{\"error\":\"zone parameter required\"}");
      return;
    }
    
    int zone = zoneStr.toInt();
    if (zone < 1 || zone > 8) {
      server.send(400, "application/json", "{\"error\":\"Invalid zone (1-8)\"}");
      return;
    }
    
    DynamicJsonDocument doc(512);
    if (api.getConfig(zone, doc)) {
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    } else {
      server.send(204, "application/json", "");
    }
  });
  
  // Zone configuration PUT
  server.on("/api/zone/config", HTTP_PUT, []() {
    String zoneStr = server.arg("zone");
    if (zoneStr == "") {
      server.send(400, "application/json", "{\"error\":\"zone parameter required\"}");
      return;
    }
    
    int zone = zoneStr.toInt();
    if (zone < 1 || zone > 8) {
      server.send(400, "application/json", "{\"error\":\"Invalid zone (1-8)\"}");
      return;
    }
    
    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }
    
    api.updateZoneConfig(zone, doc);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });
  
  // Air handler
  server.on("/api/airhandler", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    if (api.getAirHandler(doc)) {
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    } else {
      server.send(204, "application/json", "");
    }
  });
  
  // Heat pump
  server.on("/api/heatpump", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    if (api.getHeatPump(doc)) {
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    } else {
      server.send(204, "application/json", "");
    }
  });
  
  // Vacation settings GET
  server.on("/api/vacation", HTTP_GET, []() {
    DynamicJsonDocument doc(256);
    if (api.getVacation(doc)) {
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    } else {
      server.send(204, "application/json", "");
    }
  });
  
  // Vacation settings PUT
  server.on("/api/vacation", HTTP_PUT, []() {
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, server.arg("plain"));
    
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }
    
    api.updateVacation(doc);
    server.send(200, "application/json", "{\"status\":\"ok\"}");
  });
  
  // Raw data access
  server.on("/api/raw", HTTP_GET, []() {
    String deviceStr = server.arg("device");
    String tableStr = server.arg("table");
    
    if (deviceStr == "" || tableStr == "") {
      server.send(400, "application/json", "{\"error\":\"device and table parameters required\"}");
      return;
    }
    
    uint16_t device = (uint16_t)strtol(deviceStr.c_str(), NULL, 16);
    uint8_t tableAddr[3];
    
    // Parse hex table address
    for (int i = 0; i < 3; i++) {
      char hex[3] = {tableStr[i*2], tableStr[i*2+1], '\0'};
      tableAddr[i] = (uint8_t)strtol(hex, NULL, 16);
    }
    
    DynamicJsonDocument doc(512);
    if (api.getTableRaw(device, tableAddr, doc)) {
      String response;
      serializeJson(doc, response);
      server.send(200, "application/json", response);
    } else {
      server.send(504, "application/json", "{\"error\":\"Timeout waiting for response\"}");
    }
  });
  
  // Root endpoint
  server.on("/", HTTP_GET, []() {
    server.send(200, "application/json", "{\"message\":\"Infinitive ESP32 HVAC Controller\", \"version\":\"1.0\"}");
  });
  
  server.begin();
  Serial.println("Web server started on port 80");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("WebSocket client #%u connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
        
        // Send current cache on connection
        DynamicJsonDocument doc(512);
        api.dumpCache(doc);
        String response;
        serializeJson(doc, response);
        webSocket.sendTXT(num, response);
      }
      break;
      
    case WStype_DISCONNECTED:
      Serial.printf("WebSocket client #%u disconnected\n", num);
      break;
      
    case WStype_TEXT:
      Serial.printf("WebSocket client #%u sent: %s\n", num, payload);
      // Handle incoming WebSocket commands if needed
      break;
      
    case WStype_BIN:
      Serial.printf("WebSocket client #%u sent binary data\n", num);
      break;
  }
}
