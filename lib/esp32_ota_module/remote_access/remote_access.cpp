#include "remote_access.h"


/****************************************************************
 * ConfigurableStreambuf
 ****************************************************************/

// Implementierung der ConfigurableStreambuf-Klasse
ConfigurableStreambuf::ConfigurableStreambuf() {
    // Initialisiere den Ausgabepuffer (put area)
    setp(buffer, buffer + bufferSize - 1);
}

int ConfigurableStreambuf::addOutputFunction(std::function<void(const char*, size_t)> outputFunction) {
    int id = nextId++;
    outputFunctions.push_back({id, outputFunction});
    return id;
}

bool ConfigurableStreambuf::removeOutputFunction(int id) {
    auto it = std::find_if(outputFunctions.begin(), outputFunctions.end(),
        [id](const OutputEntry& entry) { return entry.id == id; });
        
    if (it != outputFunctions.end()) {
        outputFunctions.erase(it);
        return true;
    }
    return false;
}

ConfigurableStreambuf::int_type ConfigurableStreambuf::overflow(int_type c) {
    if (c != traits_type::eof()) {
        *pptr() = traits_type::to_char_type(c);
        pbump(1);
        sync();
    }
    return c != traits_type::eof() ? traits_type::to_int_type(c) : traits_type::eof();
}

int ConfigurableStreambuf::sync() {
    size_t len = pptr() - pbase();
    
    if (len > 0) {
        for (const auto& entry : outputFunctions) {
            entry.func(pbase(), len);
        }
        
        // Setze den Puffer zurück
        pbump(-static_cast<int>(len));
    }
    
    return 0;
}






/******************************************************************************************
 * AsyncTCPServer
 ******************************************************************************************/


AsyncTCPServer::AsyncTCPServer(uint16_t port) : _server(port), _port(port) 
{
    // Initialize mutex
    selectedStartOptionMutex = xSemaphoreCreateMutex();

    // Verwende Lambda-Funktionen, die 'this' erfassen
    _onClientConnect = [this](AsyncClient* client) {
        this->onConnectInternal(client);
    };
    
    _onClientDisconnect = [this](AsyncClient* client) {
        this->onDisconnectInternal(client);
    };
    
    _onData = [this](AsyncClient* client, void* data, size_t len) {
        this->onDataInternal(client, data, len);
    };
}

void AsyncTCPServer::begin() {
    _server.onClient([this](void* arg, AsyncClient* client) 
    {
        Serial.print("New client connected: ");
        Serial.println(client->remoteIP().toString());
        
        // Client zur Liste hinzufügen
        _clients.push_back(client);
        
        // Callbacks für den Client einrichten
        client->onData([this, client](void* arg, AsyncClient* c, void* data, size_t len) 
        {
            _onData(client, data, len);
        }, nullptr);
        
        client->onDisconnect([this, client](void* arg, AsyncClient* c) 
        {
            Serial.print("Client disconnected: ");
            Serial.println(client->remoteIP().toString());
            
            // Client aus der Liste entfernen
            _clients.erase(std::remove(_clients.begin(), _clients.end(), client), _clients.end());
            
            _onClientDisconnect(client);
        }, nullptr);
        
        client->onError([this, client](void* arg, AsyncClient* c, int8_t error) 
        {
            Serial.printf("Client error %d: %s\n", error, client->remoteIP().toString().c_str());
        }, nullptr);
        
        // Callback für neue Verbindung aufrufen
        _onClientConnect(client);
    }, nullptr);
    
    _server.begin();
    Serial.printf("TCP Server gestartet auf Port %d\n", _port);
}

size_t AsyncTCPServer::getClientCount() const 
{
    return _clients.size();
}

bool AsyncTCPServer::sendToAll(const char* data, size_t len) 
{
    if (_clients.empty()) {
        Serial.println("Keine Clients verbunden");
        return false;
    }
    
    bool success = true;
    for (AsyncClient* client : _clients) {
        if (client && client->connected()) {
            if (client->write(data, len) <= 0) {
                success = false;
            }
        }
    }
    return success;
}

bool AsyncTCPServer::sendToAll(const char* data) 
{
    return sendToAll(data, strlen(data));
}

bool AsyncTCPServer::sendStartOptions(const std::map<std::string, std::function<void()>>& options) 
{
    if (_clients.empty()) {
        Serial.println("Keine Clients verbunden");
        return false;
    }
    
    // JSON erstellen
    _jsonDocFunctions.clear();
    JsonArray optionsArray = _jsonDocFunctions.createNestedArray("startOptions");
    
    // Optionen hinzufügen
    for (const auto& option : options) {
        optionsArray.add(option.first);
    }
    
    // In Buffer serialisieren
    size_t len = serializeJson(_jsonDocFunctions, _jsonBufferFunctions, sizeof(_jsonBufferFunctions));
    
    // Senden
    std::clog << "Sende Startoptionen: " << _jsonBufferFunctions << std::endl;
    return sendToAll(_jsonBufferFunctions, len);
}

bool AsyncTCPServer::sendChangeableVariables(const std::map<std::string, std::reference_wrapper<uint32_t>>& variables)
{
    if (_clients.empty()) {
        Serial.println("Keine Clients verbunden");
        return false;
    }
    
    // JSON erstellen
    _jsonDocVariables.clear();
    JsonArray varsArray = _jsonDocVariables.createNestedArray("changeableVariables");
    
    // Variablen mit Werten hinzufügen
    for (const auto& variable : variables) {
        JsonObject varObj = varsArray.createNestedObject();
        varObj["name"] = variable.first;
        varObj["value"] = variable.second.get(); // .get() für Zugriff auf den Wert
    }
    
    // In Buffer serialisieren
    size_t len = serializeJson(_jsonDocVariables, _jsonBufferVariables, sizeof(_jsonBufferVariables));
    
    // Senden
    std::clog << "Sende veränderbare Variablen: " << _jsonBufferVariables << std::endl;
    return sendToAll(_jsonBufferVariables, len);
}

void AsyncTCPServer::onClientConnect(ClientConnectCallback cb) 
{
    if (cb) {
        _onClientConnect = cb;
    } else {
        _onClientConnect = [this](AsyncClient* client) {
            this->onConnectInternal(client);
        };
    }
}

void AsyncTCPServer::onClientDisconnect(ClientDisconnectCallback cb) 
{
    if (cb) {
        _onClientDisconnect = cb;
    } else {
        _onClientDisconnect = [this](AsyncClient* client) {
            this->onDisconnectInternal(client);
        };
    }
}

void AsyncTCPServer::onData(DataCallback cb)
{
    if (cb) {
        _onData = cb;
    } else {
        _onData = [this](AsyncClient* client, void* data, size_t len) {
            this->onDataInternal(client, data, len);
        };
    }
}

void AsyncTCPServer::setStartOptions(std::map<std::string, std::function<void()>> startOptions) 
{
    this->startOptions = startOptions;
}

void AsyncTCPServer::setChangeableVariables(std::map<std::string, uint32_t&> vars) 
{
    // Clear the current map
    this->changeableVariables.clear();
    
    // Add each variable individually, explicitly creating a reference_wrapper
    for (auto& pair : vars) {
        // Use insert() instead of operator[] to avoid default constructor issues
        this->changeableVariables.insert(
            std::make_pair(pair.first, std::reference_wrapper<uint32_t>(pair.second))
        );
    }
}

void AsyncTCPServer::addStartOption(std::string name, std::function<void()> function) 
{
    startOptions[name] = function;
}

void AsyncTCPServer::addChangeableVariable(std::string name, uint32_t& variable) 
{
    // Again, use insert() instead of operator[]
    changeableVariables.insert(
        std::make_pair(name, std::reference_wrapper<uint32_t>(variable))
    );
}

bool AsyncTCPServer::sendStartOptions()
{
    return sendStartOptions(startOptions);
}

bool AsyncTCPServer::sendChangeableVariables()
{
    return sendChangeableVariables(changeableVariables);
}

void AsyncTCPServer::onConnectInternal(AsyncClient* client) 
{
    Serial.print("Client verbunden von IP: ");
    Serial.println(client->remoteIP().toString());
    
    // Sende automatisch die verfügbaren Optionen und Variablen an den neuen Client
    sendStartOptions();
    sendChangeableVariables();
}

void AsyncTCPServer::onDisconnectInternal(AsyncClient* client) 
{
    Serial.print("Client getrennt von IP: ");
    Serial.println(client->remoteIP().toString());
}

void AsyncTCPServer::onDataInternal(AsyncClient* client, void* data, size_t len) 
{
    // Puffer für die eingehenden Daten mit Nullterminierung
    char* str = new char[len + 1];
    memcpy(str, data, len);
    str[len] = '\0';
    
    Serial.printf("Daten vom Client %s empfangen: %s\n", 
                 client->remoteIP().toString().c_str(), str);
    
    // Versuche die Daten als JSON zu parsen
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, str);
    
    if (!error) 
    {
        // JSON erfolgreich geparst
        if (doc.containsKey("startOption")) 
        {
            const char* funcName = doc["startOption"];
            std::clog << "selected startoption: " << funcName << std::endl;
        
            auto it = startOptions.find(funcName);
        
            // Take mutex before modifying
            if (xSemaphoreTake(selectedStartOptionMutex, portMAX_DELAY)) {
                if (it != startOptions.end()) 
                {
                    selectedStartOption = it->second;
                    validStartOption = true;
                } 
                else 
                {
                    validStartOption = false;
                    std::clog << "Warning: Function '" << funcName << "' not found in startOptions" << std::endl;
                    selectedStartOption = []() { std::cout << "Function for start option not found" << std::endl; };
                }
                // Release mutex after modifying
                xSemaphoreGive(selectedStartOptionMutex);
            }
        }
        
        else if (doc.containsKey("setVariable")) 
        {
            const char* varName = doc["setVariable"]["name"];
            
            // Versuche verschiedene Methoden zur Extraktion des Werts
            uint32_t value = 0;
            
            // 1. Versuche, den Wert als String zu bekommen und dann zu konvertieren
            char valueStr[32];
            if (serializeJson(doc["setVariable"]["value"], valueStr, sizeof(valueStr)) > 0) 
            {
                // Konvertiere den serialisierten String zurück zu einer Zahl
                value = atoi(valueStr);
                Serial.printf("Konvertierter Wert aus String: %d\n", value);
            }
            
            std::cout << "Setze Variable " << varName << " auf " << value << std::endl;
            
            // Variable in changeableVariables suchen und setzen
            auto it = changeableVariables.find(varName);
            if (it != changeableVariables.end()) 
            {
                it->second.get() = value; // .get() für den Zugriff auf die eigentliche Referenz
                
                // Sende die aktualisierten Variablenwerte an alle Clients
                sendChangeableVariables();
            }
            else 
            {
                Serial.printf("Variable %s nicht gefunden\n", varName);
            }
        }
        else if (doc.containsKey("reset"))
        {
            std::cout << "restart esp" << std::endl;
            ESP.restart();
        }
    } 
    else 
    {
        // Kein gültiges JSON, versuche als direkten Funktionsnamen zu interpretieren
        for (const auto& option : startOptions) {
            if (strcmp(str, option.first.c_str()) == 0) {
                Serial.printf("Starte Option: %s\n", option.first.c_str());
                option.second(); // Funktion ausführen
                break;
            }
        }
    }
    
    delete[] str;
}

bool AsyncTCPServer::getSelectedStartOption(std::function<void()>& option) 
{
    bool success = false;
    
    if (xSemaphoreTake(selectedStartOptionMutex, portMAX_DELAY)) 
    {
        if (validStartOption) 
        {
            option = selectedStartOption; 
            success = true;
        }
        else 
        {
            option = []() { std::cout << "Function for start option not found" << std::endl; };
            success = false;
        }
        xSemaphoreGive(selectedStartOptionMutex);
    }
    
    return success;
}