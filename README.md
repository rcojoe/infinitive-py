# Infinitive Python

A Python port of the Infinitive HVAC controller. This project impersonates a SAM (System Access Module) on a Carrier Infinity system management bus, allowing remote monitoring and control of HVAC systems through a REST API and WebSocket interface.

**Original Project:** [acd/infinitive](https://github.com/acd/infinitive) (Go)

## 📋 Table of Contents

- [Features](#features)
- [Quick Start](#quick-start)
- [System Requirements](#system-requirements)
- [Installation](#installation)
- [Configuration](#configuration)
- [Serial Port Setup](#serial-port-setup)
  - [Waveshare WiFi RS485](#waveshare-wifi-rs485)
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
- **Network Serial Ports**: Support for WiFi and Ethernet serial adapters

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
- **Serial Adapter**: USB-to-Serial, RS485-to-WiFi module, or Ethernet adapter
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

Your virtual environment is still there; just activate it again next time.

## ⚙️ Configuration

### Command-Line Arguments

```bash
python infinitive.py [OPTIONS]
```

| Argument | Default | Description |
|----------|---------|-------------|
| `--serial PORT` | `/dev/ttyUSB0` | Serial port device path |
| `--httpport PORT` | `8080` | HTTP server port |
| `--loglevel LEVEL` | `INFO` | Logging level (DEBUG, INFO, WARNING, ERROR) |
| `--cache-ttl SECONDS` | `300` | Cache time-to-live in seconds |
| `--baudrate RATE` | `38400` | Serial port baud rate |

### Example Usage

```bash
# Use COM port on Windows
python infinitive.py --serial COM3 --httpport 8080 --baudrate 38400

# Use USB serial on macOS
python infinitive.py --serial /dev/tty.usbserial-14140 --httpport 8080

# Use TTY on Linux with debugging
python infinitive.py --serial /dev/ttyUSB0 --httpport 8080 --loglevel DEBUG

# Use network serial device (see below)
python infinitive.py --serial socket://192.168.1.100:2000 --httpport 8080
```

## 🔌 Serial Port Setup

### Finding Your Serial Port

#### Windows
```bash
# Method 1: Using Device Manager
# Device Manager → Ports (COM & LPT) → Look for USB Serial Device

# Method 2: Using Python
python -c "import serial.tools.list_ports; ports = serial.tools.list_ports.comports(); [print(f'{p.device}: {p.description}') for p in ports]"
```

#### macOS
```bash
# Using ls command
ls /dev/tty.*

# Using Python (recommended)
python -c "import serial.tools.list_ports; ports = serial.tools.list_ports.comports(); [print(f'{p.device}: {p.description}') for p in ports]"
```

#### Linux
```bash
# Using dmesg (after plugging in device)
dmesg | grep ttyUSB

# Using ls
ls /dev/ttyUSB*

# Using Python (recommended)
python -c "import serial.tools.list_ports; ports = serial.tools.list_ports.comports(); [print(f'{p.device}: {p.description}') for p in ports]"
```

### Waveshare WiFi RS485

The **Waveshare WiFi RS485** module is a network-to-serial converter that allows you to connect RS485 devices over WiFi.

#### Hardware Setup

**Waveshare WiFi RS485 Module Connections:**

```
Infinity Bus Connection:
┌─────────────────────────────┐
│  Waveshare WiFi RS485       │
├─────────────────────────────┤
│ A (RS485+)  → Infinity A+   │
│ B (RS485-)  → Infinity B-   │
│ GND         → Infinity GND  │
└─────────────────────────────┘

Power Connection:
├─ USB Power (5V)
└─ Can also be powered from Infinity bus if supported
```

#### Configuration Steps

##### Step 1: Connect to Waveshare WiFi Configuration

1. **Find the module's IP address:**
   - Look at the sticker on the device (factory IP: typically `192.168.1.100` or similar)
   - Or scan your network: `nmap -sn 192.168.1.0/24` (Linux/macOS)
   - Or use network discovery tools (Windows)

2. **Connect via web browser:**
   ```
   http://192.168.1.100:80
   ```
   (Adjust IP as needed)

##### Step 2: Configure WiFi Connection

1. Go to **Network → WiFi Settings**
2. Select your WiFi network (SSID)
3. Enter WiFi password
4. Save and wait for module to reconnect

##### Step 3: Configure Serial Parameters

1. Go to **Serial → Serial Port Settings**
2. Set the following parameters:
   - **Baud Rate**: `38400` (or `19200` for older systems)
   - **Data Bits**: `8`
   - **Stop Bits**: `1`
   - **Parity**: `None`
   - **Flow Control**: `None`
3. Save settings

##### Step 4: Configure Network Settings

1. Go to **Network → TCP/UDP Settings**
2. Set the following:
   - **Work Mode**: `TCP Server` or `TCP Client` (usually `TCP Server`)
   - **Local Port**: `2000` (default) or any unused port
   - **Save settings**

##### Step 5: Find the Module's IP Address

After WiFi connection:

```bash
# Method 1: Check router admin panel
# Log into your router and look for connected devices named "WiFi RS485"

# Method 2: Network scan
# On Linux/macOS:
nmap -p 2000 192.168.1.0/24

# On Windows (PowerShell):
Test-NetConnection -ComputerName 192.168.1.100 -Port 2000

# Method 3: Use the module's web interface
# Still accessible at http://192.168.1.100 or the new IP shown on the device
```

##### Step 6: Test Connection to Module

```bash
# Using Python (from within your project directory with venv activated):
python -c "
import socket
import sys

try:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(2)
    result = sock.connect_ex(('192.168.1.100', 2000))
    sock.close()
    if result == 0:
        print('✓ Connection successful on 192.168.1.100:2000')
    else:
        print('✗ Connection failed - check IP and port')
except Exception as e:
    print(f'✗ Error: {e}')
"
```

#### Using with Infinitive Python

**Option 1: Using pyserial's socket URL scheme**

```bash
# Activate virtual environment first
source venv/bin/activate  # or: venv\Scripts\activate on Windows

# Run with network serial device
python infinitive.py --serial socket://192.168.1.100:2000 --httpport 8080
```

**Option 2: Create a serial port tunnel on Linux/macOS**

```bash
# Install socat if not present
sudo apt-get install socat  # Linux
brew install socat           # macOS

# Create virtual serial port connected to WiFi device
socat -d -d PTY,link=/tmp/ttyWiFi,raw,echo=0 TCP:192.168.1.100:2000

# In another terminal, run infinitive
python infinitive.py --serial /tmp/ttyWiFi --httpport 8080
```

**Option 3: Create a serial port tunnel on Windows (PowerShell)**

```powershell
# Using COM port redirection with socat (Windows with WSL)
# Or use third-party tools like:
# - Serial-to-TCP Bridge (com0com)
# - HW Virtual Serial Port
# - Network Serial Port Control/Control RTS

# For Windows, the simplest method is Option 1 (socket://)
```

#### Troubleshooting Waveshare Connection

**Problem: Module not found on network**

```bash
# 1. Check module is powered and has WiFi indicator light
# 2. Reset module to factory settings (hold reset button 5-10 seconds)
# 3. Scan network with default IP:
ping 192.168.1.100

# 4. If not found, try factory reset and connect directly via USB for configuration
```

**Problem: Connection times out**

```bash
# 1. Verify module is connected to WiFi:
# Check the web interface: http://192.168.1.100

# 2. Verify serial parameters are correct:
# - Baud Rate should be 38400 (or 19200)
# - Stop bits should be 1
# - No flow control

# 3. Check firewall allows port 2000:
# Windows Firewall → Inbound Rules → Allow port 2000
# Linux/macOS: sudo ufw allow 2000/tcp

# 4. Verify module's local port setting (check web interface)
```

**Problem: Intermittent connection drops**

```bash
# Common causes:
# 1. WiFi signal too weak - move module closer to router
# 2. RF interference - avoid microwave, cordless phones
# 3. Buffer overflow - reduce logging level to reduce data flow

python infinitive.py --serial socket://192.168.1.100:2000 --loglevel ERROR
```

**Problem: Data corruption or garbled messages**

```bash
# 1. Verify baud rate matches: 38400 (or 19200)
# 2. Check RS485 wiring - ensure proper A and B connections
# 3. Try resetting module and reconnecting
# 4. Check for long cable runs (> 100 feet may need shielding)
```

#### Advanced: Static IP Assignment

To ensure stable connection, assign a static IP to the Waveshare module:

1. **In module web interface:**
   - Go to **Network → IPv4 Settings**
   - Set **DHCP** to `Off`
   - Configure static IP (example: `192.168.1.200`)
   - Set gateway and DNS

2. **Or in your router:**
   - Access router admin panel
   - Find Waveshare device (MAC address on sticker)
   - Assign static DHCP lease

3. **Use the static IP in infinitive:**
   ```bash
   python infinitive.py --serial socket://192.168.1.200:2000 --httpport 8080
   ```

#### Performance Tuning for Waveshare

```bash
# For stable, reliable operation:
python infinitive.py \
  --serial socket://192.168.1.100:2000 \
  --httpport 8080 \
  --baudrate 38400 \
  --cache-ttl 300 \
  --loglevel WARNING
```

- **`--cache-ttl 300`**: Reduces serial port traffic by caching responses
- **`--loglevel WARNING`**: Reduces network overhead from logging

---

## 📡 Usage

### Starting the Server

```bash
# Activate virtual environment
source venv/bin/activate  # or: venv\Scripts\activate

# Run the application
python infinitive.py --serial /dev/ttyUSB0 --httpport 8080

# Expected output:
# INFO:root:Connecting to serial port /dev/ttyUSB0 at 38400 baud
# INFO:root:Starting HTTP server on http://0.0.0.0:8080
# INFO:root:WebSocket support enabled
```

### Accessing the Interface

- **Web Interface**: http://localhost:8080
- **REST API**: http://localhost:8080/api/...
- **WebSocket**: ws://localhost:8080/ws

### Command Examples

```bash
# Query current thermostat status
curl http://localhost:8080/api/tstat/settings

# Set zone 1 to 72°F heating
curl -X POST http://localhost:8080/api/zone/1/config \
  -H "Content-Type: application/json" \
  -d '{"heatSetpoint": 72}'

# Enable vacation mode for 7 days
curl -X POST http://localhost:8080/api/vacation \
  -H "Content-Type: application/json" \
  -d '{"days": 7, "minTemperature": 60, "maxTemperature": 80}'
```

## 🌐 API Endpoints

### System Status
- `GET /api/tstat` - Full thermostat status
- `GET /api/tstat/settings` - Thermostat settings only

### Zones (1-8)
- `GET /api/zone/{zone}` - Get zone status
- `POST /api/zone/{zone}/config` - Update zone configuration
- `GET /api/zone/{zone}/humidity` - Get zone humidity

### Equipment Status
- `GET /api/airhandler` - Air handler status
- `GET /api/heatpump` - Heat pump status
- `GET /api/furnace` - Furnace status

### Vacation Mode
- `GET /api/vacation` - Get vacation settings
- `POST /api/vacation` - Set vacation mode

### Raw Protocol Access
- `GET /api/raw` - Send raw protocol commands
- `GET /api/protocol` - Protocol statistics

## 🏗️ Architecture

```
┌─────────────────────────────────────┐
│  Web Browser / API Client           │
└────────────────┬────────────────────┘
                 │ HTTP/WebSocket
                 │
┌────────────────▼────────────────────┐
│  Flask Web Server                   │
│  ├─ REST API handlers               │
│  └─ WebSocket manager               │
└────────────────┬────────────────────┘
                 │
┌────────────────▼────────────────────┐
│  Infinitive Protocol Engine         │
│  ├─ Message codec                   │
│  ├─ Device communication            │
│  └─ State management                │
└────────────────┬────────────────────┘
                 │ Serial / Network
                 │
┌────────────────▼────────────────────┐
│  Serial Port Interface              │
│  ├─ USB-Serial adapter              │
│  ├─ WiFi RS485 (Waveshare)         │
│  └─ Direct serial connection        │
└────────────────┬────────────────────┘
                 │ RS485/TTL serial
                 │
┌────────────────▼────────────────────┐
│  Carrier Infinity HVAC System       │
│  ├─ Thermostat                      │
│  ├─ Air Handler                     │
│  ├─ Heat Pump/Furnace               │
│  └─ Zone Dampers                    │
└─────────────────────────────────────┘
```

## 🔧 Hardware Setup

### Serial Adapter Wiring

The Infinity bus requires a serial adapter to communicate:

```
┌─ Infinity Connector (3-pin or 5-pin)
│  ├─ Pin 1: +12V or +24V (depends on system)
│  ├─ Pin 2: Serial TX
│  ├─ Pin 3: Serial RX  
│  ├─ Pin 4: GND (if 5-pin)
│  └─ Pin 5: (reserved)
│
└─ USB-to-Serial Adapter
   ├─ GND ──────────────→ GND
   ├─ RX (from system) → TX
   ├─ TX (to system)   → RX
   └─ May need level shifting (3.3V ↔ TTL)
```

### Carrier SAM Module Emulation

This application emulates a Carrier SAM (System Access Module), which appears to the system as:

```
Device ID: 0x4000 (Infinity Control Module)
Memory Table: 0x00B0 (SAM control table)
Capabilities:
  - Full read/write to thermostat settings
  - Multi-zone control
  - Vacation mode management
  - System status monitoring
  - Protocol version: 1.0-1.4
```

## 🐛 Troubleshooting

### Serial Port Issues

**Problem: "Permission Denied" on /dev/ttyUSB0**

```bash
# Linux/macOS: Add user to dialout group
sudo usermod -a -G dialout $USER
newgrp dialout

# Or run with sudo (not recommended for production)
sudo python infinitive.py --serial /dev/ttyUSB0
```

**Problem: "Serial port not found"**

```bash
# List available ports
python -c "
import serial.tools.list_ports
for port in serial.tools.list_ports.comports():
    print(f'{port.device}: {port.description}')
"

# Use correct port
python infinitive.py --serial [port-from-list]
```

**Problem: "Port already in use"**

```bash
# Another application is using the port
# Kill the process:

# macOS/Linux:
sudo lsof -i :8080  # Check what's using port 8080
kill -9 [PID]

# Windows PowerShell:
Get-Process | Where-Object {$_.Handles -match 'COMx'}
```

### Communication Problems

**Problem: No response from thermostat**

```bash
# 1. Check wiring is correct
# 2. Verify baud rate: 38400 or 19200
# 3. Enable debug logging:
python infinitive.py --serial /dev/ttyUSB0 --loglevel DEBUG

# 4. Check signal with protocol analyzer:
python infinitive.py --serial /dev/ttyUSB0 --loglevel DEBUG 2>&1 | grep "Frame"
```

**Problem: Intermittent "CRC errors"**

```bash
# Usually indicates wiring issues:
# 1. Check RS485 termination resistors (120Ω between A and B)
# 2. Verify twisted-pair cable for RS485
# 3. Ensure common ground connection
# 4. Reduce cable length or add shielding
```

### Network Issues (WiFi RS485)

**Problem: "Connection refused" on socket://**

```bash
# 1. Verify module IP address:
ping 192.168.1.100  # or correct IP

# 2. Verify port is correct (usually 2000):
nc -zv 192.168.1.100 2000  # Linux/macOS
Test-NetConnection -ComputerName 192.168.1.100 -Port 2000  # Windows

# 3. Check module web interface is accessible:
# Open http://192.168.1.100 in browser

# 4. Check module serial parameters match (38400 baud)
```

### Virtual Environment Issues

**Problem: "ModuleNotFoundError: No module named 'flask'"**

```bash
# Ensure virtual environment is activated
source venv/bin/activate  # Linux/macOS
venv\Scripts\activate     # Windows

# Reinstall packages
pip install -r requirements.txt
```

**Problem: "python: command not found"**

```bash
# Use python3 explicitly
python3 infinitive.py --serial /dev/ttyUSB0

# Or create an alias
alias python=python3
python infinitive.py --serial /dev/ttyUSB0
```

### Debugging Tips

**Enable verbose logging:**

```bash
python infinitive.py --serial /dev/ttyUSB0 --loglevel DEBUG
```

**Capture serial traffic:**

```bash
# macOS/Linux using minicom
minicom -D /dev/ttyUSB0 -b 38400

# Or using screen
screen /dev/ttyUSB0 38400
# (Press Ctrl+A then Ctrl+\ to exit)
```

**Test REST API connectivity:**

```bash
# Check if server is running
curl http://localhost:8080/api/tstat

# With verbose output
curl -v http://localhost:8080/api/tstat

# With timeout (useful for debugging freezes)
curl --max-time 5 http://localhost:8080/api/tstat
```

## 🚀 Advanced Topics

### Local Bridge for GitHub Codespace

Run infinitive-py from GitHub Codespaces with local hardware:

```bash
# On your local machine with serial connection:
ssh-keygen -t ed25519  # Generate key pair

# Share key with Codespace
# On Codespace:
ssh -R 2000:localhost:2000 your-local-machine

# On local machine, set up serial bridge:
socat TCP-LISTEN:2000,reuseaddr FILE:/dev/ttyUSB0,b38400,raw,echo=0

# On Codespace:
python infinitive.py --serial socket://127.0.0.1:2000
```

### CloudFlare Tunnel Integration

Expose infinitive-py remotely without port forwarding:

```bash
# Install cloudflared
# https://developers.cloudflare.com/cloudflare-one/connections/connect-applications/

# Create tunnel
cloudflared tunnel create infinitive-hvac
cloudflared tunnel route dns infinitive-hvac yourdomain.com

# Configure tunnel
cat > ~/.cloudflared/config.yml << EOF
tunnel: infinitive-hvac
credentials-file: ~/.cloudflared/infinitive-hvac.json
ingress:
  - hostname: infinitive-hvac.yourdomain.com
    service: http://localhost:8080
  - service: http_status:404
EOF

# Run tunnel
cloudflared tunnel run infinitive-hvac

# Access remotely
# https://infinitive-hvac.yourdomain.com
```

### Running as a Service

#### macOS (LaunchAgent)

```bash
# Create plist file
cat > ~/Library/LaunchAgents/com.infinitive.hvac.plist << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>Label</key>
    <string>com.infinitive.hvac</string>
    <key>ProgramArguments</key>
    <array>
        <string>/Users/USERNAME/infinitive-py/venv/bin/python</string>
        <string>/Users/USERNAME/infinitive-py/infinitive.py</string>
        <string>--serial</string>
        <string>/dev/ttyUSB0</string>
        <string>--httpport</string>
        <string>8080</string>
    </array>
    <key>RunAtLoad</key>
    <true/>
    <key>StandardOutPath</key>
    <string>/tmp/infinitive.log</string>
    <key>StandardErrorPath</key>
    <string>/tmp/infinitive-error.log</string>
</dict>
</plist>
EOF

# Load service
launchctl load ~/Library/LaunchAgents/com.infinitive.hvac.plist

# Check status
launchctl list | grep infinitive
```

#### Linux (Systemd)

```bash
# Create service file
sudo tee /etc/systemd/system/infinitive.service > /dev/null << EOF
[Unit]
Description=Infinitive HVAC Controller
After=network.target

[Service]
Type=simple
User=YOUR_USERNAME
WorkingDirectory=/home/YOUR_USERNAME/infinitive-py
ExecStart=/home/YOUR_USERNAME/infinitive-py/venv/bin/python infinitive.py --serial /dev/ttyUSB0 --httpport 8080
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
EOF

# Enable and start
sudo systemctl enable infinitive
sudo systemctl start infinitive

# Check status
sudo systemctl status infinitive
```

### Docker Deployment

```dockerfile
# Dockerfile
FROM python:3.11-slim

WORKDIR /app

# Install dependencies
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# Copy application
COPY infinitive.py .
COPY src/ src/

# Create non-root user
RUN useradd -m -u 1000 infinitive && chown -R infinitive:infinitive /app
USER infinitive

# Default command (can be overridden)
CMD ["python", "infinitive.py", "--serial", "socket://host.docker.internal:2000", "--httpport", "8080"]
```

```bash
# Build image
docker build -t infinitive-hvac .

# Run container with serial port forwarding
docker run -p 8080:8080 \
  --device=/dev/ttyUSB0 \
  -e SERIAL_PORT=/dev/ttyUSB0 \
  infinitive-hvac

# Or with network serial device
docker run -p 8080:8080 \
  --network host \
  infinitive-hvac python infinitive.py \
    --serial socket://192.168.1.100:2000 \
    --httpport 8080
```

### Database Persistence

Store historical data (future enhancement):

```bash
# Install SQLite support
pip install flask-sqlalchemy

# Modify infinitive.py to add database logging
# Example: Log all temperature changes, HVAC events, etc.
```

## ⚠️ Safety & Legal Warnings

**IMPORTANT**: Before using this software, understand:

1. **Warranty Disclaimer**: This software is provided "AS IS" without warranty
2. **System Damage Risk**: Improper serial communication may damage your HVAC system
3. **Thermostat Control Risk**: Incorrect settings may create comfort/safety issues
4. **Manufacturer Support**: Carrier may refuse support if they detect third-party control
5. **Local Regulations**: Some jurisdictions may have restrictions on modifying HVAC systems
6. **Insurance**: Check if modifications affect your HVAC warranty or home insurance
7. **Network Security**: This exposes HVAC control - use only on trusted networks

**Use at your own risk!**

## 🤝 Contributing

We welcome contributions! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/your-feature`)
3. Test thoroughly (especially serial communication!)
4. Commit with clear messages
5. Push to your fork
6. Open a Pull Request

### Development Setup

```bash
# Clone and create virtual environment
git clone https://github.com/rcojoe/infinitive-py.git
cd infinitive-py
python3 -m venv venv
source venv/bin/activate

# Install dev dependencies
pip install -r requirements.txt
pip install -r requirements-dev.txt  # pytest, black, etc.

# Run tests
pytest

# Code formatting
black src/

# Linting
pylint src/
```

## 📄 License

This project is licensed under the MIT License. See [LICENSE](LICENSE) file for details.

## 🔗 References & Resources

- **Original Go Implementation**: [acd/infinitive](https://github.com/acd/infinitive)
- **Carrier Infinity Docs**: [Carrier Equipment Documentation](https://www.carrier.com/)
- **Waveshare WiFi RS485**: [Waveshare Module Documentation](https://www.waveshare.com/wiki/WiFi_RS485)
- **Python Serial**: [PySerial Documentation](https://pyserial.readthedocs.io/)
- **Flask Documentation**: [Flask Guide](https://flask.palletsprojects.com/)
- **RS485 Protocol**: [RS485 Standard](https://en.wikipedia.org/wiki/RS-485)

## 💬 Support & Community

- **Issues**: [GitHub Issues](https://github.com/rcojoe/infinitive-py/issues)
- **Discussions**: [GitHub Discussions](https://github.com/rcojoe/infinitive-py/discussions)
- **Original Project**: [acd/infinitive](https://github.com/acd/infinitive)

---

**Last Updated**: 2024
