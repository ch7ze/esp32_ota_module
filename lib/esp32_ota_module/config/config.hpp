#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <AsyncUDP.h>

#include <iostream>
#include <unordered_map>
#include <functional>
#include <atomic>

static constexpr char *ssid = "Raspberry";
static constexpr char *password = "55555555";


inline IPAddress udpTargetIP(192, 168, 43, 23); // IP-Adresse des Zielcomputers (dein PC)
inline const int udpPort = 60000;
#define HOSTNAME "test"

using std::cout;
using std::clog;
using std::endl;




#include <string>
#include <sstream>
#include <ArduinoJson.h>

// Template-Klasse für Variableninfo mit einheitlichem JSON-Format
template<typename T>
class VarInfo {
private:
    const std::string name;
    const T value; // Als Kopie speichern, nicht als Referenz!
    
    // Konvertiert den Wert in einen String
    std::string valueToString() const {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
    
public:
    VarInfo(const std::string& n, const T& v) : name(n), value(v) {}
    
    friend std::ostream& operator<<(std::ostream& os, const VarInfo<T>& info) {
        StaticJsonDocument<128> doc;
        doc[info.name] = info.valueToString();
        String jsonString;
        serializeJson(doc, jsonString);
        os << jsonString.c_str();
        return os;
    }
    
    // Direkte Umwandlung in String
    operator std::string() const {
        StaticJsonDocument<128> doc;
        doc[name] = valueToString();
        String jsonString;
        serializeJson(doc, jsonString);
        return jsonString.c_str();
    }
};

// Spezialisierung für std::string
template<>
inline std::string VarInfo<std::string>::valueToString() const {
    return value;
}

// Spezialisierung für const char*
template<>
inline std::string VarInfo<const char*>::valueToString() const {
    return std::string(value);
}

// Spezialisierung für Arduino String
template<>
inline std::string VarInfo<String>::valueToString() const {
    return std::string(value.c_str());
}

// Helfer-Funktion
template<typename T>
VarInfo<T> makeVarInfo(const std::string& name, const T& value) {
    return VarInfo<T>(name, value);
}

// Makro für die einfache Verwendung
#define vInfo(var) makeVarInfo(#var, var)

template<typename T>
VarInfo<T> makeNamedVarInfo(const std::string& name, const T& value) {
    return VarInfo<T>(name, value);
}

// Zusätzliches Makro für benannte Variablen
#define vInfoNamed(name, value) makeNamedVarInfo(name, value)


//P I N   D E C L A R A T I O N

//I2C
static constexpr uint8_t PIN_SCL 					= 1;
static constexpr uint8_t PIN_SDA 					= 2;

//ultra-sonic sensor
static constexpr uint8_t PIN_TRIGGER 				= 3;
static constexpr uint8_t PIN_ECHO 					= 4;

//line-follow sensor
static constexpr uint8_t PIN_REFLECTION_SENSOR_1 	= 5;
static constexpr uint8_t PIN_REFLECTION_SENSOR_2 	= 6;
static constexpr uint8_t PIN_REFLECTION_SENSOR_3 	= 7;
static constexpr uint8_t PIN_REFLECTION_SENSOR_4 	= 8;
static constexpr uint8_t PIN_REFLECTION_SENSOR_5 	= 9;
static constexpr uint8_t PIN_REFLECTION_SENSOR_6 	= 10;
static constexpr uint8_t PIN_REFLECTION_SENSOR_7 	= 11;
static constexpr uint8_t PIN_REFLECTION_SENSOR_8 	= 12;

//headlights
static constexpr uint8_t PIN_LED_WHITE_LEFT 		= 13;
static constexpr uint8_t PIN_LED_WHITE_RIGHT 		= 18;

//backlights
static constexpr uint8_t PIN_LED_RED_LEFT 			= 37;
static constexpr uint8_t PIN_LED_RED_RIGHT 			= 38;

//battery-voltage
static constexpr uint8_t PIN_VOLTAGE_MEASURE 		= 14;

//motor driver
static constexpr uint8_t PIN_FAULT 					= 15;
static constexpr uint8_t PIN_SLEEP 					= 21;
static constexpr uint8_t PIN_MOTOR_RIGHT_1 			= 34;
static constexpr uint8_t PIN_MOTOR_RIGHT_2 			= 33;
static constexpr uint8_t PIN_MOTOR_LEFT_1 			= 36;
static constexpr uint8_t PIN_MOTOR_LEFT_2 			= 35;

//servo
static constexpr uint8_t PIN_SERVO 					= 17;

//user buttons
static constexpr uint8_t PIN_USER_BUTTON_2 			= 39;
static constexpr uint8_t PIN_USER_BUTTON_1 			= 40;

/*******************************************************/


// pwm channels

static constexpr uint8_t LEDC_CH_MOTOR_LEFT = 6;
static constexpr uint8_t LEDC_CH_MOTOR_RIGHT = 7;

static constexpr uint16_t MOTOR_PWM_FREQ = 1000;
static constexpr uint8_t MOTOR_PWM_RESOLUTION = 8;
static constexpr uint8_t MOTOR_PWM_MAX = 255;


//servo limits
static constexpr uint8_t SERVO_MIN = 60;
static constexpr uint8_t SERVO_MAX = 120;

static constexpr uint16_t SERVO_MID = 1309;


// reflection sensor
static constexpr uint16_t NUMBER_OF_REFLECTION_SENSORS = 8;


// battery voltage
static constexpr double BATTERY_VOLTAGE_MULTIPLIER = 2.7;
static constexpr uint16_t MINIMUM_BATTERY_VOLTAGE = 6000;        // Voltage in milivolt

static constexpr uint16_t REFLECTION_SENSOR_THRESHOLD = 7000;


//std::unordered_map<std::string, std::function<void()>> startOptions;


extern "C" 
{
	int _write(int fd, char *ptr, int len);
}

void otaUpdate();

/* // UDP-Konfiguration
inline WiFiUDP udp;
inline const char* udpAddress = "192.168.43.23"; // IP-Adresse des Zielcomputers (dein PC)
inline const int udpPort = 60000;

class SerialStreambuf : public std::streambuf {
private:
    static constexpr size_t bufferSize = 128; // Größe des internen Puffers
    char buffer[bufferSize];
    size_t bufferIndex = 0;

protected:
    virtual int overflow(int c) override {
        if (c != EOF) {
            buffer[bufferIndex++] = c;
            if (bufferIndex >= bufferSize || c == '\n') {
                sync();
            }
        }
        return c;
    }

    virtual int sync() override {
        if (bufferIndex > 0) {
            Serial.write(buffer, bufferIndex);
            // Sende den gesamten Puffer über UDP
            udp.beginPacket(udpAddress, udpPort);
            udp.write(reinterpret_cast<const uint8_t*>(buffer), bufferIndex);
            udp.endPacket();
            bufferIndex = 0; // Puffer zurücksetzen
        }
        return 0;
    }
};

inline SerialStreambuf serial_streambuf; */

/* // UDP-Konfiguration
inline AsyncUDP asyncUdp;
// Replace string with IPAddress object
inline const IPAddress udpAddress(192, 168, 43, 23);
inline const int udpPort = 60000;

class SerialStreambuf : public std::streambuf {
private:
    static constexpr size_t bufferSize = 128;
    char buffer[bufferSize];
    size_t bufferIndex = 0;

protected:
    virtual int overflow(int c) override {
        if (c != EOF) {
            buffer[bufferIndex++] = c;
            if (bufferIndex >= bufferSize || c == '\n') {
                sync();
            }
        }
        return c;
    }

    virtual int sync() override {
        if (bufferIndex > 0) {
            Serial.write(buffer, bufferIndex);
            // Asynchrones Senden über UDP
            asyncUdp.writeTo(reinterpret_cast<const uint8_t*>(buffer), 
                            bufferIndex, 
                            udpAddress,  // Now using IPAddress object
                            udpPort);
            bufferIndex = 0;
        }
        return 0;
    }
}; 
inline SerialStreambuf serial_streambuf;*/