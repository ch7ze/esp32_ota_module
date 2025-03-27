#pragma once

#include <Arduino.h>
#include <ota/ota.hpp>
#include <config/config.hpp>
#include <remote_access/remote_access.h>
#include "map"



class OTA_EXAMPLE
{

public:
	
	void init();

	void run();

	private:
		std::function<void()> selectedStartOption;

};

inline OTA_EXAMPLE otaExample;


inline std::map<std::string, std::function<void()>> startOptions = {
    {"Option 1", []() { cout << "Option 1 ausgeführt" << endl; }},
	{"Option 2", []() { cout << "Option 2 ausgeführt" << endl; }},
	{"Option 3", []() { cout << "Option 3 ausgeführt" << endl; }}
};


inline uint32_t variable1 = 43;
inline uint32_t variable2 = 555;
inline uint32_t variable3 = 6666;


inline std::map<std::string, uint32_t&> changeableVariables = 
{
	{"Variable 1", variable1},
	{"Variable 2", variable2},
	{"Variable 3", variable3}
};