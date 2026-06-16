# Infinitive ESP32 - Arduino Implementation

Arduino/ESP32 implementation of the Infinitive HVAC controller for Carrier Infinity systems.

## Features

- **WiFi REST API** - Control and monitor your HVAC system over WiFi
- **WebSocket Support** - Real-time updates for connected clients
- **Serial Protocol** - Direct communication with Carrier Infinity systems
- **Multi-Zone Support** - Up to 8 zones per system
- **JSON API** - Easy integration with other systems

## Hardware Requirements

- **ESP32 Development Board** - Any standard ESP32 board
- **Serial Adapter** - TTL to Infinity bus adapter (19200 baud)
- **Power Supply** - 5V for ESP32
- **USB Cable** - For programming and debugging

### Pin Configuration

```
ESP32 Pin Mapping:
- GPIO16 (RX) → Serial RX from Infinity bus
- GPIO17 (TX) → Serial TX to Infinity bus
- GND → Common ground with Infinity system
```

## Installation

### 1. Install Arduino IDE
Download from https://www.arduino.cc/en/software

### 2. Install ESP32 Board Support
In Arduino IDE:
- Go to **File → Preferences**
- Add to "Additional Board Manager URLs":
  ```
  https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
  ```
- Go to **Tools → Board → Board Manager**
- Search for "ESP32" and install "ESP32 by Espressif Systems"

### 3. Install Required Libraries
In Arduino IDE, go to **Sketch → Include Library → Manage Libraries**:
- Install "ArduinoJson" (by Benoit Blanchon)
- Install "WebSockets" (by Markus Sattler)

### 4. Configure WiFi Credentials
Edit `infinitive_esp32.ino` and update:
```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
```

### 5. Select Board and Port
- Go to **Tools → Board** and select "ESP32 Dev Module" (or your specific board)
- Go to **Tools → Port** and select the COM port for your ESP32
- Go to **Tools → Upload Speed** and set to 921600

### 6. Upload
Click the Upload button or press `Ctrl+U`

## Usage

After uploading, the ESP32 will:
1. Connect to your WiFi network
2. Display the IP address in the Serial Monitor (baud rate: 115200)
3. Start the REST API server on port 80
4. Start the WebSocket server on port 81

### Serial Monitor
Open **Tools → Serial Monitor** (115200 baud) to see debug output.

## API Endpoints

### Thermostat Status
```
GET http://<esp32-ip>/api/tstat/settings
```

### Zone Configuration
```
GET http://<esp32-ip>/api/zone/config?zone=1
PUT http://<esp32-ip>/api/zone/config?zone=1
Content-Type: application/json

{
  "fanMode": "auto",
  "heatSetpoint": 68,
  "coolSetpoint": 78,
  "hold": false,
  "mode": "auto"
}
```

### Air Handler
```
GET http://<esp32-ip>/api/airhandler
```

Response:
```json
{
  "blowerRPM": 800,
  "airFlowCFM": 600,
  "elecHeat": false
}
```

### Heat Pump
```
GET http://<esp32-ip>/api/heatpump
```

### Vacation Settings
```
GET http://<esp32-ip>/api/vacation
PUT http://<esp32-ip>/api/vacation
Content-Type: application/json

{
  "days": 7,
  "minTemperature": 60,
  "maxTemperature": 80,
  "minHumidity": 30,
  "maxHumidity": 60,
  "fanMode": "auto"
}
```

### Raw Data Access
```
GET http://<esp32-ip>/api/raw?device=4000&table=002701
```

### WebSocket Connection
```
ws://<esp32-ip>:81
```

## Serial Adapter Wiring

The Infinity bus uses a custom connector. You'll need a TTL serial adapter:

```
Infinity Bus Connector → TTL Serial Adapter → ESP32

TX → RX (GPIO 16)
RX → TX (GPIO 17)
GND → GND
```

**Note:** The Infinity protocol uses 19200 baud, 8 data bits, no parity, 1 stop bit (8N1).

## Troubleshooting

### Serial Connection Issues
- Check baud rate is 19200
- Verify wiring connections
- Test with another serial terminal (PuTTY, CoolTerm)
- Check for signal inversion if needed

### WiFi Connection Issues
- Verify SSID and password are correct
- Check ESP32 is in range of WiFi network
- Look at Serial Monitor output for errors

### API Not Responding
- Ensure ESP32 is connected to WiFi
- Check firewall is not blocking port 80
- Verify the IP address from Serial Monitor

## Project Structure

```
infinitive_esp32/
├── infinitive_esp32.ino       # Main entry point
├── infinitive_bus.h/cpp       # Serial protocol implementation
├── infinitive_api.h/cpp       # High-level API
└── infinitive_conversions.h/cpp # Mode/fan mode conversions
```

## Memory Usage

The ESP32 has plenty of memory for this project:
- Flash: 4MB (plenty for code and assets)
- RAM: 520KB SRAM + 8MB PSRAM
- Typical usage: ~200KB Flash, ~100KB RAM

## Performance

- REST API response time: <100ms
- WebSocket update frequency: 1000ms (configurable)
- Serial protocol latency: 50-200ms per message
- WebSocket connections: Up to 10 simultaneous clients

## Security Considerations

⚠️ **Important**: This implementation provides NO authentication or encryption:
- Run on trusted networks only
- Use behind a firewall
- Consider adding authentication if exposed to untrusted networks

For production use, consider:
- Adding HTTP Basic Authentication
- Using SSL/TLS certificates
- Rate limiting on API endpoints
- Input validation on all parameters

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please test thoroughly before submitting pull requests.

## References

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [Arduino IDE](https://www.arduino.cc/)
- [ArduinoJson Library](https://arduinojson.org/)
- [WebSockets for Arduino](https://github.com/Links2004/arduinoWebSockets)
