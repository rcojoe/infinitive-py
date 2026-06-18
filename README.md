# Infinitive Python

A Python port of the Infinitive HVAC controller. This project impersonates a SAM (System Access Module) on a Carrier Infinity system management bus, allowing remote monitoring and control of HVAC systems via REST API and WebSocket connections.

**Original Project:** [acd/infinitive](https://github.com/acd/infinitive) (Go)

## 📋 Table of Contents

- [Features](#features)
- [Quick Start](#quick-start)
- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [API Endpoints](#api-endpoints)
- [Architecture](#architecture)
- [Hardware Setup](#hardware-setup)
- [Troubleshooting](#troubleshooting)
- [Advanced Topics](#advanced-topics)
- [Contributing](#contributing)

## ✨ Features

- **Serial Communication**: Direct communication with Carrier Infinity HVAC systems via serial port
- **REST API**: Full REST API for querying and controlling thermostat settings
- **WebSocket Support**: Real-time updates for connected clients
- **Multi-Zone Support**: Support for up to 8 HVAC zones per system
- **Zone Configuration**: Control temperature setpoints, fan modes, and hold settings per zone
- **Vacation Mode**: Set vacation/away mode parameters
- **Cache Management**: Efficient caching of device state
- **Protocol Monitoring**: Tools for capturing and analyzing Infinity protocol frames

## 🚀 Quick Start

### For the Impatient

```bash
# Clone and enter directory
git clone https://github.com/rcojoe/infinitive-py.git
cd infinitive-py

# Create and activate virtual environment
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt

# Run (adjust serial port as needed)
python infinitive.py --serial /dev/ttyUSB0 --httpport 8080

# Visit http://localhost:8080 in your browser
```

## 💻 System Requirements

### Minimum Requirements
- **Python**: 3.8 or higher
- **Operating System**: Windows, macOS, or Linux
- **RAM**: 256 MB minimum
- **Disk Space**: 100 MB for installation

### Hardware (for HVAC communication)
- **Serial Adapter**: USB-to-Serial or RS485-to-WiFi module
- **Connection**: TTL serial or RS485 bus to Infinity system
- **Baud Rate**: 38400 baud (or 19200 for some systems)

### Python Verification

Check your Python version:

```bash
python --version
# or
python3 --version
```

Should show Python 3.8 or higher. If you have multiple Python versions, use `python3` explicitly.

## 📦 Installation

### Step 1: Create Project Directory

Choose a location for your project and create it:

```bash
# Navigate to where you want the project (Windows example)
cd C:\Users\YourUsername\Documents

# Or on macOS/Linux
cd ~/Projects

# Create the project directory
mkdir infinitive-hvac
cd infinitive-hvac
```

### Step 2: Clone the Repository

```bash
git clone https://github.com/rcojoe/infinitive-py.git
cd infinitive-py
```

**Don't have Git?** 
- Download from: https://git-scm.com/download
- Or [download as ZIP](https://github.com/rcojoe/infinitive-py/archive/refs/heads/main.zip) and extract

### Step 3: Create Python Virtual Environment

A virtual environment isolates this project's dependencies from your system Python.

#### On macOS / Linux:

```bash
# Create virtual environment named 'venv'
python3 -m venv venv

# Activate it
source venv/bin/activate

# You should see (venv) in your terminal prompt
```

#### On Windows (Command Prompt):

```bash
# Create virtual environment
python -m venv venv

# Activate it
venv\Scripts\activate.bat

# You should see (venv) in your command prompt
```

#### On Windows (PowerShell):

```bash
# Create virtual environment
python -m venv venv

# Activate it
venv\Scripts\Activate.ps1

# If you get an execution policy error, run:
# Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser

# You should see (venv) in your PowerShell prompt
```

### Step 4: Install Required Packages

With the virtual environment activated, install dependencies:

```bash
# Upgrade pip (recommended)
pip install --upgrade pip

# Install required packages
pip install -r requirements.txt
```

**What gets installed?**

| Package | Version | Purpose |
|---------|---------|---------|
| `Flask` | 3.0.0+ | Web framework for REST API |
| `Flask-CORS` | 4.0.0+ | Cross-Origin Resource Sharing support |
| `Flask-Sock` | 0.2.9+ | WebSocket support for Flask |
| `pyserial` | 3.5+ | Serial port communication with HVAC system |

### Step 5: Verify Installation

Test that everything is working:

```bash
# Check Flask
python -c "import flask; print(f'Flask {flask.__version__}')"

# Check pyserial
python -c "import serial; print(f'pySerial version installed')"

# List all installed packages
pip list
```

Expected output:
```
Flask                     3.0.0
Flask-CORS                4.0.0
Flask-Sock                0.2.9
pyserial                  3.5
```

### Step 6: Deactivate Virtual Environment (Optional)

When you're done working:

```bash
deactivate
```

To reactivate later:
```bash
# macOS/Linux
source venv/bin/activate

# Windows
venv\Scripts\activate.bat  # or .ps1 for PowerShell
```

## ⚙️ Configuration

### Finding Your Serial Port

Depending on your hardware setup, you'll need to identify the serial port:

#### Windows
1. Connect USB-Serial adapter
2. Open Device Manager (Win+X → Device Manager)
3. Look under "Ports (COM & LPT)"
4. Your port will be something like `COM3` or `COM4`

Or use Python:
```bash
python -m serial.tools.list_ports
```

#### macOS
```bash
ls /dev/tty.*
# Look for /dev/tty.usbserial-XXXXX or similar
```

#### Linux
```bash
ls /dev/ttyUSB*
ls /dev/ttyACM*
# Usually /dev/ttyUSB0
```

### Checking Your Connection

Test serial communication:

```bash
# Windows
python -c "import serial; ser = serial.Serial('COM3', 38400); print('Connected!'); ser.close()"

# macOS/Linux
python -c "import serial; ser = serial.Serial('/dev/ttyUSB0', 38400); print('Connected!'); ser.close()"
```

## 🏃 Usage

### Basic Usage

```bash
# Activate virtual environment first
source venv/bin/activate  # macOS/Linux
# or
venv\Scripts\activate.bat  # Windows

# Run the server
python infinitive.py --serial /dev/ttyUSB0 --httpport 8080
```

### Command Line Arguments

| Argument | Required | Default | Description |
|----------|----------|---------|-------------|
| `--serial` | Yes | N/A | Serial port (e.g., `/dev/ttyUSB0`, `COM3`) |
| `--httpport` | No | 8080 | HTTP server port |

### Examples

```bash
# Linux with default port
python infinitive.py --serial /dev/ttyUSB0

# Windows with custom port
python infinitive.py --serial COM3 --httpport 9000

# macOS with non-default port
python infinitive.py --serial /dev/tty.usbserial-14140 --httpport 8080

# RS485-to-WiFi bridge (monitoring)
python rs485_wifi_infinity_monitor.py 192.168.1.100 10001
```

### Accessing the API

Once running, access the server at:

- **REST API**: http://localhost:8080/
- **Check Status**: http://localhost:8080/api/tstat/settings

You should see output like:
```
 * Serving Flask app 'webserver'
 * Debug mode: off
 * Running on http://127.0.0.1:8080
```

## 📡 API Endpoints

### Thermostat Settings

Get current thermostat settings:

```bash
curl http://localhost:8080/api/tstat/settings
```

Response:
```json
{
  "currentTemp": 72,
  "outdoorTemp": 65,
  "mode": "auto",
  "fanMode": "auto"
}
```

### Zone Configuration

#### Get Zone Config

```bash
curl http://localhost:8080/api/zone/1/config
```

Response:
```json
{
  "zone": 1,
  "currentTemp": 72,
  "currentHumidity": 45,
  "mode": "auto",
  "fanMode": "auto",
  "hold": false,
  "heatSetpoint": 68,
  "coolSetpoint": 78
}
```

#### Update Zone Config

```bash
curl -X PUT http://localhost:8080/api/zone/1/config \
  -H "Content-Type: application/json" \
  -d '{
    "mode": "auto",
    "fanMode": "auto",
    "hold": false,
    "heatSetpoint": 68,
    "coolSetpoint": 78
  }'
```

### Air Handler Status

```bash
curl http://localhost:8080/api/airhandler
```

Response:
```json
{
  "blowerRPM": 800,
  "airFlowCFM": 600,
  "elecHeat": false
}
```

### Heat Pump Status

```bash
curl http://localhost:8080/api/heatpump
```

Response:
```json
{
  "compressorOutput": 75,
  "supplyTemp": 105,
  "returnTemp": 72
}
```

### Vacation Mode

#### Get Vacation Settings

```bash
curl http://localhost:8080/api/zone/1/vacation
```

#### Update Vacation Settings

```bash
curl -X PUT http://localhost:8080/api/zone/1/vacation \
  -H "Content-Type: application/json" \
  -d '{
    "days": 7,
    "minTemperature": 60,
    "maxTemperature": 80,
    "minHumidity": 30,
    "maxHumidity": 60,
    "fanMode": "auto"
  }'
```

### Raw Data Access

```bash
GET /api/raw/{device}/{table}
```

Get raw protocol data directly.

- `device`: 4-character hex string (device ID)
- `table`: 6-character hex string (table address)

### WebSocket

Connect to real-time updates:

```javascript
const ws = new WebSocket('ws://localhost:8080/api/ws');

ws.onmessage = (event) => {
  const data = JSON.parse(event.data);
  console.log('Update:', data);
};

ws.onerror = (error) => {
  console.error('WebSocket error:', error);
};
```

## 🏗️ Architecture

### Project Structure

```
infinitive-py/
├── infinitive.py                    # Main entry point
├── webserver.py                     # Flask web server and API
├── requirements.txt                 # Python dependencies
│
├── infinity/                        # Core protocol package
│   ├── __init__.py
│   ├── api.py                       # High-level API
│   ├── bus.py                       # Serial bus communication
│   └── conversions.py               # Mode/fan mode conversions
│
├── internal/                        # Internal utilities
│   ├── __init__.py
│   ├── cache.py                     # State caching
│   └── dispatcher.py                # Event distribution
│
├── esp32/                           # Arduino/ESP32 implementation
│   ├── infinitive_esp32/
│   │   ├── infinitive_esp32.ino     # Main sketch
│   │   ├── infinitive_bus.h/cpp     # Serial protocol
│   │   ├── infinitive_api.h/cpp     # High-level API
│   │   └── infinitive_conversions.h/cpp
│   ├── README.md                    # ESP32 setup guide
│   └── HARDWARE.md                  # Wiring diagrams
│
├── README.md                        # This file
├── LICENSE                          # MIT License
└── .gitignore
```

### Core Components

| Component | File | Purpose |
|-----------|------|---------|
| **Bus** | `infinity/bus.py` | Manages serial communication with Infinity protocol |
| **API** | `infinity/api.py` | High-level interface for device operations |
| **Cache** | `internal/cache.py` | Maintains state of device readings |
| **Dispatcher** | `internal/dispatcher.py` | Broadcasts updates to WebSocket clients |
| **Web Server** | `webserver.py` | Flask REST API and WebSocket server |

### Protocol Details

The Infinity protocol uses:
- **Baud Rate**: 38400 (or 19200)
- **Data Format**: 8 bits, No parity, 1 stop bit
- **Framing**: 10-byte header + variable data + 2-byte CRC16
- **CRC Polynomial**: 0x8005 (standard RS485)

Frame structure:
```
[DST:2][SRC:2][LEN:1][RES:2][OP:1][DATA:N][CRC16:2]
 Destination    Source   Length Reserved Operation  Data  Checksum
```

## 🔌 Hardware Setup

### Serial Port Wiring

#### TTL Serial (3.3V)

```
Infinity HVAC System
├─ TX  ────→ USB Adapter RX  ────→ Computer
├─ RX  ←──── USB Adapter TX  ←──── Computer
└─ GND ──────→ USB Adapter GND ──→ Computer
```

#### RS485 Serial

```
Infinity HVAC System (or RS485-to-WiFi Module)
├─ A (or +)   ────→ RS485 Adapter A/+
├─ B (or -)   ────→ RS485 Adapter B/-
└─ GND        ────→ RS485 Adapter GND
```

### RS485-to-WiFi Module Setup

If using an RS485-to-WiFi module:

1. **Connect module to Infinity bus**:
   - RS485 A → Infinity TX+
   - RS485 B → Infinity TX-
   - GND → Infinity GND

2. **Configure module**:
   - Access web interface (check module documentation)
   - Set Baud Rate: 38400
   - Set Mode: Raw TCP/UDP (NOT Modbus gateway)
   - Set Server Port: 10001 (or your choice)

3. **Monitor frames**:
   ```bash
   # Make sure venv is activated first
   python rs485_wifi_infinity_monitor.py 192.168.1.100 10001
   ```

## 🔧 Troubleshooting

### Virtual Environment Issues

**Problem**: `python: command not found` or wrong version

```bash
# Use python3 explicitly
python3 infinitive.py --serial /dev/ttyUSB0

# Or check your Python path
which python
which python3
```

**Problem**: `source: command not found` on Windows

```bash
# Use Windows-specific activation
venv\Scripts\activate.bat
```

**Problem**: Virtual environment not activating

```bash
# Check if venv directory exists
ls venv          # macOS/Linux
dir venv         # Windows

# Recreate if needed
python3 -m venv venv
source venv/bin/activate  # or appropriate command for your OS
```

### Serial Port Issues

**Problem**: `Permission denied` on Linux/macOS

```bash
# Add your user to dialout group (Linux)
sudo usermod -a -G dialout $USER
# Log out and log back in

# Or use sudo
sudo python infinitive.py --serial /dev/ttyUSB0
```

**Problem**: `Port not found` or `SerialException`

```bash
# List available ports
python -m serial.tools.list_ports

# Verify connection
ls /dev/tty*        # macOS/Linux
# or Device Manager  # Windows
```

**Problem**: `OSError: [Errno 16] Device or resource busy`

```bash
# Another program is using the port
# Close any terminal emulators (PuTTY, CoolTerm, etc.)
# Or identify the process using the port
lsof /dev/ttyUSB0   # macOS/Linux
```

### Package Installation Issues

**Problem**: `ModuleNotFoundError: No module named 'flask'`

```bash
# Ensure virtual environment is activated
source venv/bin/activate  # or your system's command

# Reinstall requirements
pip install -r requirements.txt

# Or install specific package
pip install Flask==3.0.0
```

**Problem**: `pip: command not found`

```bash
# Use python -m pip
python -m pip install -r requirements.txt

# Or upgrade pip first
python -m pip install --upgrade pip
```

**Problem**: `requirements.txt not found`

```bash
# Make sure you're in the infinitive-py directory
pwd  # Show current directory
ls requirements.txt  # Check if file exists

# If in wrong directory
cd infinitive-py
pip install -r requirements.txt
```

### Connection Issues

**Problem**: `Connection refused` on API calls

```bash
# Check if server is running
# Look for output like "Running on http://127.0.0.1:8080"

# Check firewall
# Allow port 8080 (or your chosen port) through firewall

# Verify correct port
curl http://localhost:8080/
```

**Problem**: WebSocket connection fails

```bash
# WebSocket requires port 8080 accessible
# Check that Flask was started successfully
# Try simpler GET request first: curl http://localhost:8080/
```

**Problem**: Serial port stuck or unresponsive

```bash
# Restart the USB connection (unplug/replug adapter)
# Or on Linux, reset the port
sudo tio /dev/ttyUSB0  # Install with: sudo apt install tio

# Kill any hanging processes
pkill -f infinitive.py
```

## 🔍 Advanced Topics

### Monitoring the Infinity Protocol

Capture and analyze protocol frames:

```bash
# Basic monitoring (requires RS485-to-WiFi module)
python rs485_wifi_infinity_monitor.py 192.168.1.100 10001

# With custom baud rate
python -c "
import serial
ser = serial.Serial('/dev/ttyUSB0', 38400)
while True:
    if ser.in_waiting:
        data = ser.read(ser.in_waiting)
        print(data.hex())
"
```

### Extending the API

Add custom endpoints in `webserver.py`:

```python
@app.route("/api/custom", methods=["GET"])
def custom_endpoint():
    # Your custom logic
    return jsonify({"status": "ok"})
```

### Running as a Service

#### macOS (LaunchAgent)

Create `~/Library/LaunchAgents/com.hvac.infinitive.plist`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>Label</key>
  <string>com.hvac.infinitive</string>
  <key>ProgramArguments</key>
  <array>
    <string>/path/to/venv/bin/python</string>
    <string>/path/to/infinitive.py</string>
    <string>--serial</string>
    <string>/dev/ttyUSB0</string>
  </array>
  <key>RunAtLoad</key>
  <true/>
</dict>
</plist>
```

Load it:
```bash
launchctl load ~/Library/LaunchAgents/com.hvac.infinitive.plist
```

#### Linux (Systemd)

Create `/etc/systemd/system/infinitive.service`:

```ini
[Unit]
Description=Infinitive HVAC Controller
After=network.target

[Service]
Type=simple
User=your-username
WorkingDirectory=/path/to/infinitive-py
ExecStart=/path/to/venv/bin/python infinitive.py --serial /dev/ttyUSB0
Restart=always

[Install]
WantedBy=multi-user.target
```

Enable it:
```bash
sudo systemctl daemon-reload
sudo systemctl enable infinitive
sudo systemctl start infinitive
```

### Docker Deployment

Build and run in Docker:

```bash
# Create Dockerfile
cat > Dockerfile << 'EOF'
FROM python:3.11-slim
WORKDIR /app
COPY requirements.txt .
RUN pip install -r requirements.txt
COPY . .
CMD ["python", "infinitive.py", "--serial", "/dev/ttyUSB0", "--httpport", "8080"]
EOF

# Build image
docker build -t infinitive-hvac .

# Run container
docker run -d --device /dev/ttyUSB0 -p 8080:8080 infinitive-hvac
```

### Using GitHub Codespace with Local Network

If you want to access your local RS485-to-WiFi module from a GitHub Codespace:

1. **Set up local bridge** on your computer:
   ```bash
   # On your local machine (in infinitive-py directory)
   python local_bridge_server.py 8888
   ```

2. **Expose via CloudFlare Tunnel**:
   ```bash
   # Install CloudFlare Tunnel
   brew install cloudflared  # macOS
   
   # Create tunnel
   cloudflared tunnel create infinitive-bridge
   cloudflared tunnel route dns infinitive-bridge yourdomain.com
   cloudflared tunnel run infinitive-bridge --url http://localhost:8888
   ```

3. **Use in Codespace**:
   ```bash
   python rs485_wifi_infinity_monitor.py infinitive-bridge.yourdomain.com 443
   ```

## 📄 License

MIT License - See [LICENSE](LICENSE) file for details

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit changes (`git commit -m 'Add amazing feature'`)
4. Push to branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Testing

Please test your changes:

```bash
# Make sure venv is activated
source venv/bin/activate

# Run basic connectivity test
python -c "import infinity.bus; print('Protocol imports OK')"

# Test API endpoints
curl http://localhost:8080/

# Test WebSocket connection
python -c "from webserver import launch_webserver; print('Flask server loads OK')"
```

## ⚠️ Safety & Legal

**IMPORTANT**: 

- Use this software **ONLY** on HVAC systems you own or have explicit permission to modify
- Unauthorized changes to HVAC systems can cause:
  - Equipment damage
  - Safety hazards
  - Personal injury
  - Voided warranties

- **Always test** in a safe manner first
- **Keep** the system's original controls functional
- **Document** any changes you make

## 📚 References

- [acd/infinitive (Original Go Project)](https://github.com/acd/infinitive)
- [Carrier Infinity Documentation](https://www.carrier.com/)
- [Python Serial Documentation](https://pyserial.readthedocs.io/)
- [Flask Documentation](https://flask.palletsprojects.com/)
- [WebSocket RFC 6455](https://tools.ietf.org/html/rfc6455)

## 🆘 Getting Help

If you encounter issues:

1. **Check Troubleshooting** section above
2. **Review logs** for error messages
3. **Verify hardware** connections
4. **Test components** individually
5. **Open an Issue** on GitHub with:
   - Your OS and Python version (`python --version`)
   - Complete error messages
   - Steps to reproduce
   - Hardware configuration (serial adapter type, connection method)

## 🎯 Roadmap

Planned features:
- [ ] Scheduler for temperature setpoints
- [ ] Historical data logging
- [ ] Mobile app
- [ ] Integration with Home Assistant
- [ ] MQTT support
- [ ] Database backend for long-term storage

---

**Made with ❤️ for HVAC enthusiasts**

Last Updated: June 2024
