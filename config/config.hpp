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

namespace ota
{



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
}