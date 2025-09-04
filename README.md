# ESP32 OTA Module

A comprehensive Arduino library for ESP32 Over-The-Air (OTA) updates with advanced remote access capabilities, TCP server communication, and variable management.

## Features

- **OTA Updates**: Wireless firmware updates using Arduino OTA
- **Remote Access**: TCP server with JSON-based command interface
- **Variable Management**: Runtime configuration of variables via network
- **Structured Logging**: JSON-formatted output with UDP forwarding
- **Background Tasks**: Automatic OTA handling in separate FreeRTOS task
- **Multi-Client Support**: Handle multiple simultaneous TCP connections

## Installation

### PlatformIO (Recommended)

Add to your `platformio.ini`:

```ini
lib_deps = 
    https://github.com/yourusername/esp32_ota_module.git
    bblanchon/ArduinoJson@^6.21.3
    me-no-dev/AsyncTCP@^1.1.1
```

### Arduino IDE

1. **Library Manager Method:**
   - Open Arduino IDE
   - Go to `Tools` > `Manage Libraries`
   - Search for "ESP32 OTA Module"
   - Click Install

2. **Manual Installation:**
   - Download or clone this repository
   - Extract to your Arduino libraries folder
   - Restart Arduino IDE

3. **Git URL Installation:**
   - In Arduino IDE, go to `Sketch` > `Include Library` > `Add .ZIP Library`
   - Enter: `https://github.com/yourusername/esp32_ota_module/archive/main.zip`

## Quick Start

### Basic Usage

```cpp
#include <ESP32_OTA_Module.h>
#include <WiFi.h>

// Configuration
const char* ESP32_OTA_SSID = "YourWiFiName";
const char* ESP32_OTA_PASSWORD = "YourWiFiPassword";
IPAddress ESP32_OTA_UDP_TARGET_IP(192, 168, 1, 100);
const int ESP32_OTA_UDP_PORT = 60000;
const char* ESP32_OTA_HOSTNAME = "esp32-device";

void setup() {
    Serial.begin(115200);
    
    // Connect WiFi
    WiFi.begin(ESP32_OTA_SSID, ESP32_OTA_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    
    // Initialize OTA
    ota::setupOTA();
    
    // Start TCP server
    ota::tcpServer.begin();
    
    // Add remote functions
    ota::tcpServer.addStartOption("restart", []() {
        ESP.restart();
    });
}

void loop() {
    ArduinoOTA.handle();
    delay(1000);
}
```

## API Reference

### OTA Functions

#### `ota::setupOTA()`
Initializes OTA functionality with hostname and error handling.

#### `ota::startAutomaticOTA()` 
Starts OTA handling in a separate FreeRTOS task (recommended).

### TCP Server

#### `ota::tcpServer`
Global AsyncTCPServer instance for remote communication.

#### `tcpServer.begin()`
Starts TCP server on port 50000.

#### `tcpServer.addStartOption(name, function)`
Adds a remotely callable function.
```cpp
ota::tcpServer.addStartOption("calibrate", []() {
    // Your calibration code
});
```

#### `tcpServer.addChangeableVariable(name, variable)`  
Adds a variable that can be modified remotely.
```cpp
uint32_t delayTime = 1000;
ota::tcpServer.addChangeableVariable("delayTime", delayTime);
```

#### `tcpServer.getSelectedStartOption(function)`
Retrieves and executes pending remote function calls.
```cpp
std::function<void()> func;
if (ota::tcpServer.getSelectedStartOption(func)) {
    func(); // Execute the function
}
```

### Utility Functions

#### `ota::makeVarInfo(name, value)`
Creates JSON-formatted variable output.
```cpp
std::cout << ota::makeVarInfo("temperature", 25.6) << std::endl;
// Output: {"temperature":"25.6"}
```

#### `vInfo(variable)` (Macro)
Shorthand for variable info with automatic naming.
```cpp
int sensor = 123;
std::cout << vInfo(sensor) << std::endl;
// Output: {"sensor":"123"}
```

## Remote Control Protocol

### Connection
Connect to ESP32 on TCP port 50000:
```bash
telnet ESP32_IP_ADDRESS 50000
```

### Commands

#### Execute Functions
```json
{"startOption":"function_name"}
```

#### Set Variables
```json
{"setVariable":{"name":"variableName","value":newValue}}
```

#### Reset Device
```json
{"reset":true}
```

### Automatic Responses
- Upon connection, available functions and variables are sent
- Variable changes are broadcast to all connected clients

## Examples

### [BasicUsage](examples/BasicUsage/)
Simple Arduino IDE example showing core OTA and remote access features.

### [PlatformIO_Example](examples/PlatformIO_Example/)  
Advanced PlatformIO example with:
- Automatic OTA background task
- Multiple remote functions
- Variable management
- Structured logging
- Periodic tasks

## Configuration Variables

These must be defined in your sketch:

```cpp
const char* ESP32_OTA_SSID;           // WiFi network name
const char* ESP32_OTA_PASSWORD;       // WiFi password  
IPAddress ESP32_OTA_UDP_TARGET_IP;    // Target IP for UDP logging
const int ESP32_OTA_UDP_PORT;         // UDP port for logging
const char* ESP32_OTA_HOSTNAME;       // OTA hostname
```

## Dependencies

- **ArduinoJson** (^6.21.3): JSON parsing and generation
- **AsyncTCP** (^1.1.1): Asynchronous TCP connections
- **ESP32 Arduino Core**: ESP32 framework support

## Hardware Requirements

- ESP32 development board (any variant)
- WiFi connection
- Minimum 4MB flash recommended

## Troubleshooting

### OTA Issues
- Ensure ESP32 and computer are on same network
- Check firewall settings
- Verify hostname resolution

### TCP Connection Issues  
- Confirm ESP32 IP address from Serial Monitor
- Test with telnet or netcat first
- Check port 50000 availability

### Compilation Errors
- Verify all dependencies are installed
- Check ESP32 Arduino Core version compatibility
- Ensure correct board selection

## Development

### Project Structure
```
esp32_ota_module/
├── src/
│   ├── ESP32_OTA_Module.h      # Main header
│   ├── config/
│   │   ├── config.hpp          # Configuration
│   │   └── config.cpp          # Configuration implementation
│   ├── ota/
│   │   ├── ota.hpp             # OTA header
│   │   └── ota.cpp             # OTA implementation
│   └── remote_access/
│       ├── remote_access.h     # TCP server header
│       └── remote_access.cpp   # TCP server implementation
├── examples/
│   ├── BasicUsage/             # Arduino IDE example
│   └── PlatformIO_Example/     # PlatformIO example
└── library.properties         # Arduino library metadata
```

### Contributing
1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Changelog

### v1.0.0
- Initial release
- Basic OTA functionality
- TCP server with JSON protocol
- Variable management system
- Arduino IDE and PlatformIO support