#include "config.hpp"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>





extern "C" 
{
	int _write(int fd, char *ptr, int len) 
	{
		(void) fd;
		return Serial.write(ptr, len);
	}
}

