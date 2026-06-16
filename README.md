# Infinitive Python

A Python port of the Infinitive HVAC controller. This project impersonates a SAM (System Access Module) on a Carrier Infinity system management bus, allowing remote monitoring and control of HVAC systems.

## Original Go Project

Based on: [acd/infinitive](https://github.com/acd/infinitive)

## Features

- **Serial Communication**: Direct communication with Carrier Infinity HVAC systems via serial port
- **REST API**: Full REST API for querying and controlling thermostat settings
- **WebSocket Support**: Real-time updates via WebSocket connections
- **Multi-Zone Support**: Support for up to 8 HVAC zones
- **Zone Configuration**: Control temperature setpoints, fan modes, and hold settings per zone
- **Vacation Mode**: Set vacation/away mode parameters
- **Cache Management**: Efficient caching of device state

## Installation

### Requirements

- Python 3.8+
- pip

### Setup

```bash
git clone https://github.com/rcojoe/infinitive-py.git
cd infinitive-py
pip install -r requirements.txt
```

## Usage

```bash
python infinitive.py --serial /dev/ttyUSB0 --httpport 8080
```

### Arguments

- `--serial` (required): Path to the serial port device (e.g., `/dev/ttyUSB0` on Linux, `COM3` on Windows)
- `--httpport` (optional): HTTP port to listen on (default: 8080)

## API Endpoints

### Thermostat Status

```
GET /api/tstat/settings
```

Returns current thermostat settings.

### Zone Configuration

```
GET /api/zone/{zone}/config
PUT /api/zone/{zone}/config
```

Get or update zone configuration (temperature setpoints, fan mode, hold status).

**Zones**: 1-8

**PUT Body Example**:
```json
{
  "mode": "auto",
  "fanMode": "auto",
  "hold": false,
  "heatSetpoint": 68,
  "coolSetpoint": 78
}
```

### Air Handler

```
GET /api/airhandler
```

Returns air handler status (blower RPM, airflow CFM).

### Heat Pump

```
GET /api/heatpump
```

Returns heat pump status (temperatures, stage).

### Vacation Mode

```
GET /api/zone/1/vacation
PUT /api/zone/1/vacation
```

Get or update vacation mode settings.

**PUT Body Example**:
```json
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
GET /api/raw/{device}/{table}
```

Get raw protocol data directly.

- `device`: 4-character hex string (device ID)
- `table`: 6-character hex string (table address)

### WebSocket

```
WS /api/ws
```

WebSocket connection for real-time updates.

## Architecture

### Core Components

- **Bus** (`infinity/bus.py`): Handles serial communication with the Infinity protocol
- **Frame** (`infinity/bus.py`): Encodes/decodes protocol frames
- **API** (`infinity/api.py`): High-level API for device interaction
- **Cache** (`internal/cache.py`): Caches device state
- **Dispatcher** (`internal/dispatcher.py`): Event distribution system

### Protocol

The Infinity protocol is a custom binary protocol that uses:
- 19200 baud serial communication
- CRC16 checksums for frame validation
- Request/response patterns for device communication

## Differences from Go Version

1. **Web Framework**: Flask instead of Gin
2. **Serial Library**: `pyserial` instead of `github.com/tarm/serial`
3. **Concurrency**: Threading instead of goroutines
4. **Type System**: Duck typing instead of Go structs

## Hardware Requirements

- Serial port adapter compatible with Carrier Infinity systems
- Access to the SAM connector or equivalent serial interface on your HVAC system

## Warning

Unauthorized changes to HVAC systems can cause equipment damage or injury. Use this software only on systems you own or have explicit permission to modify.

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.
