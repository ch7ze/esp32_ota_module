#include <ESP32_OTA_Module.h>
#include <WiFi.h>

// WiFi Configuration - Adjust these values
const char* ESP32_OTA_SSID = "YourWiFiName";
const char* ESP32_OTA_PASSWORD = "YourWiFiPassword"; 

// UDP Configuration for remote logging
IPAddress ESP32_OTA_UDP_TARGET_IP(192, 168, 1, 100); // Your PC's IP address
const int ESP32_OTA_UDP_PORT = 60000;

// Hostname for OTA
const char* ESP32_OTA_HOSTNAME = "esp32-ota-basic";

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("ESP32 OTA Module - Basic Usage Example");
    
    // Connect to WiFi
    WiFi.begin(ESP32_OTA_SSID, ESP32_OTA_PASSWORD);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println();
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
    
    // Initialize OTA
    ota::setupOTA();
    Serial.println("OTA initialized");
    
    // Start TCP server for remote access
    ota::tcpServer.begin();
    Serial.println("TCP Server started on port 50000");
    
    // Add some example start options
    ota::tcpServer.addStartOption("test_function", []() {
        Serial.println("Test function executed!");
    });
    
    ota::tcpServer.addStartOption("restart", []() {
        Serial.println("Restarting ESP32...");
        ESP.restart();
    });
    
    Serial.println("Setup completed. You can now:");
    Serial.println("1. Use Arduino IDE OTA to upload new sketches");
    Serial.println("2. Connect to TCP port 50000 for remote control");
    Serial.println("3. Send JSON commands like: {\"startOption\":\"test_function\"}");
}

void loop() {
    // Handle OTA updates
    ArduinoOTA.handle();
    
    // Your main code here
    delay(1000);
}