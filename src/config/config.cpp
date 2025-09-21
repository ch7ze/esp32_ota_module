#include "config.hpp"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>



void ota::setInitialConfig(initial_config config)
{
	ota_config = config;
}