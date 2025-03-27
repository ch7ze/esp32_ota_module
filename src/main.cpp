#include <ota/ota.hpp>
#include <config/config.hpp>
#include <remote_access/remote_access.h>

#include "example.h"




void setup() 
{
// Start serial communication
	Serial.begin(115200);

	configurableStreambuf.addOutputFunction(serialOut);
	configurableStreambuf.addOutputFunction(udpOut);

	std::clog.rdbuf(&configurableStreambuf);
	std::cout.rdbuf(&configurableStreambuf);
    
    // Advertise the UDP service
    //MDNS.addService("esp32log", "udp", 59999);
	delay(5);

	otaExample.init();

	
	xTaskCreate(
		[](void *pvParameters)
		{	uint16_t count = 0;
			for(;;)
			{
				std::clog << "\0" ;
				fflush(stdout);
				//tcpClient.sendString("Hello from tcp!\n", count);
				//tcpServer.sendToAll("Hello from tcp!\n");
				delay(1000);
			}
		},
		"Heartbeat",
		10000,
		NULL,
		1,
		NULL);
	

	delay(100);
	otaExample.run();

	xTaskCreate(
		[](void *pvParameters)
		{	uint16_t count = 0;
			for(;;)
			{
				std::clog << "Hello World!: " << count++  << std::endl;
				//tcpClient.sendString("Hello from tcp!\n", count);
				tcpServer.sendToAll("Hello from tcp!\n");
				delay(1000);
				std::clog << vInfo(count) << std::endl;
				delay(1000);
			}
		},
		"OTA",
		10000,
		NULL,
		1,
		NULL);



}

void loop() 
{

}