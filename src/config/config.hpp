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


namespace ota
{
    struct initial_config
    {
        std::string ssid;
        std::string password;
        IPAddress targetIP;
    };


    void setInitialConfig(initial_config config);
}



#define HOSTNAME "test"

using std::cout;
using std::clog;
using std::endl;




#include <string>
#include <sstream>
#include <ArduinoJson.h>

namespace ota
{
    constexpr uint16_t udpPort = 3232; // Port für UDP-Ausgabe

    inline initial_config ota_config;


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

}