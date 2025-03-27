#pragma once

#include <Arduino.h>
#include "../config/config.hpp"
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>


void setupOTA();

void startAutomaticOTA();