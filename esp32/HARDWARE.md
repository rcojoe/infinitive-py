# Hardware Setup for Infinitive ESP32

## ESP32 Board Selection

### Recommended Boards
1. **ESP32 Dev Kit C** (Most popular)
   - 30 GPIO pins
   - Built-in USB-UART for programming
   - Good availability and documentation
   - ~$10-15 USD

2. **ESP32-S3** (More modern)
   - 45 GPIO pins
   - Native USB support (no separate UART)
   - More RAM (8MB)
   - ~$12-18 USD

3. **ESP32-WROOM** (Generic module)
   - 25 GPIO pins
   - Requires development board
   - Very affordable

## Serial Adapter for Infinity Bus

### Option 1: Simple TTL Adapter (Recommended)
Use a USB-to-TTL adapter:
- **CH340**, **CP2102**, or **FT232RL** based adapters
- ~$5-10 USD
- Available on Amazon, eBay, AliExpress

### Connections
```
USB-TTL Adapter    ESP32
─────────────────────────
GND        ────→  GND
RX         ────→  GPIO 17 (TX)
TX         ────→  GPIO 16 (RX)
+5V (optional) → 5V (if powered from USB)
```

### Option 2: Custom Serial Adapter
If you have access to the Infinity bus connector:
1. Identify the bus connector on your HVAC system
2. Use breakout adapter or carefully connect wires
3. Add series resistors (10k ohm) for protection
4. Ensure common ground

## Power Supply

### Option 1: USB Power (Recommended for development)
- Connect ESP32 USB port to computer or 5V USB power supply
- Provides both power and serial programming
- Simple and safe

### Option 2: Dedicated 5V Supply
- 5V 1A power supply minimum
- Connect to: ESP32 5V and GND pins
- Add 100µF capacitor near power input for stability

### Option 3: Battery (for portable testing)
- 5V USB power bank
- Provides ~5-10 hours of operation
- No mains power required

## Complete Wiring Diagram

```
                    ┌─────────────────┐
                    │     ESP32       │
                    │                 │
                    │  ┌───────────┐  │
                    │  │ USB PORT  │  │ ← USB Power + Serial Programming
                    │  └───────────┘  │
                    │                 │
     GPIO16 (RX) ◄─┤                 │
     GPIO17 (TX) ─►┤                 │
     GND        ┌─►┤                 │
                │  │                 │
                │  └─────────────────┘
                │
                │  ┌──────────────────┐
                │  │  TTL-UART Adapter│
                │  │  (CH340/CP2102)  │
                │  │                  │
                ├─►│ GND              │
                   │                  │
                   │ RX  ─────┐       │
                   │          │       │
                   │ TX  ─────┤       │
                   │          │       │
                   │ +5V      │       │
                   └──────────┼───────┘
                              │
                    ┌─────────┴────────┐
                    │  Infinity Bus    │
                    │   (19200 baud)   │
                    └──────────────────┘
```

## Testing the Setup

### Step 1: Verify USB Connection
1. Connect ESP32 to computer via USB
2. Open Arduino IDE
3. Go to **Tools → Port**
4. You should see a COM port listed (Windows) or /dev/ttyUSB0 (Linux)

### Step 2: Test Serial Monitor
1. Upload the provided sketch
2. Open Serial Monitor (Tools → Serial Monitor)
3. Set baud rate to 115200
4. You should see startup messages

### Step 3: Test WiFi Connection
1. Update WiFi credentials in sketch
2. Upload
3. Check Serial Monitor for IP address
4. Try pinging the ESP32: `ping <ip-address>`

### Step 4: Test API
1. Open browser and go to: `http://<esp32-ip>/`
2. You should see: `{"message":"Infinitive ESP32 HVAC Controller"}`

### Step 5: Test Serial Communication
1. Connect TTL adapter to Infinity bus
2. Check Serial Monitor for debug output
3. Try API calls to read device data

## Troubleshooting

### USB Port Not Recognized
- Install CH340 drivers (if using CH340 adapter)
- Try different USB cable
- Try different USB port
- Restart Arduino IDE

### Upload Fails
- Check if correct board is selected
- Try pressing BOOT button while uploading
- Check USB cable quality
- Reduce upload speed to 115200

### Serial Data Corrupted
- Check baud rate matches (19200 for Infinity bus)
- Add ferrite beads to serial lines
- Keep serial wires short and shielded
- Check for signal reflections

### No WiFi Connection
- Check SSID and password spelling
- Verify WiFi network is 2.4GHz (ESP32 doesn't support 5GHz by default)
- Check WiFi signal strength
- Try manually setting channel in code

## Advanced Configuration

### Custom Pin Assignment
To use different pins, edit in `infinitive_esp32.ino`:
```cpp
const int SERIAL_RX_PIN = 16;  // Change to desired RX pin
const int SERIAL_TX_PIN = 17;  // Change to desired TX pin
```

### Changing Baud Rates
To test different serial speeds, edit:
```cpp
const uint32_t SERIAL_BAUD = 19200;  // Infinity bus standard
```

### PSRAM Support (ESP32-S3)
The ESP32-S3 includes 8MB of PSRAM for large buffers:
```cpp
// Enable in Tools → PSRAM: "Enabled"
```

## Safety Considerations

⚠️ **Important Safety Notes:**

1. **Voltage Levels**: Ensure serial adapter output is 3.3V logic (not 5V)
2. **Power Supply**: Use stable, regulated power supply
3. **Grounding**: Ensure common ground between ESP32 and Infinity bus
4. **Surge Protection**: Consider adding surge protection diodes to serial lines
5. **Heat**: Monitor ESP32 temperature - add heatsink if needed
6. **Interference**: Keep away from high-power switching supplies

## Parts List

For a complete setup, you'll need:

| Part | Cost | Notes |
|------|------|-------|
| ESP32 Dev Kit | $10-15 | Check compatibility |
| USB-TTL Adapter | $5-10 | CH340 or FT232RL |
| USB Cable (Micro-B) | $2-5 | Preferably high quality |
| Jumper Wires | $2-5 | 22 AWG recommended |
| Breadboard | $5 | Optional for prototyping |
| 5V Power Supply | $10-20 | 1A minimum |
| Ferrite Beads | $1 | Optional for EMI filtering |
| **Total** | **~$35-70** | Depends on extras |

## Resources

- [ESP32 Pinout Reference](https://www.google.com/search?q=esp32+pinout)
- [TTL UART Adapter Setup](https://arduino.stackexchange.com/questions/54240/how-to-connect-ttl-serial-adapter-to-esp32)
- [ESP32 Power Supply Design](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32_devkitc.html)
