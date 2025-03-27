#include "example.h"



using std::cout;
using std::clog;

void OTA_EXAMPLE::init()
{
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) 
	{
		delay(20);
		Serial.println("Connecting to WiFi...");
	}

	clog << "Connected to WiFi\n";
	clog << "IP address: " << WiFi.localIP() << '\n';
	clog << "\n**********************************\n";
	clog << "Init OTA\n";
	clog << "**********************************\n\n";
	clog << "Init mDNS\n";

	clog << vInfo(WiFi.status()) << std::endl;

	if (!MDNS.begin(HOSTNAME)) 
	{
		Serial.println("Error setting up MDNS responder!");
		while (1) 
		{
		  delay(1000);
		}
	}
	clog << "mDNS responder started\n";
	clog << "Start automatic OTA\n";
	startAutomaticOTA();

	clog << "set start options\n";
	tcpServer.setStartOptions(startOptions);

	clog << "set start options\n";                    
	tcpServer.setChangeableVariables(changeableVariables);

	clog << "begin tcp server\n";
	tcpServer.begin();
	cout << vInfo(WiFi.localIP().toString()) << std::endl;
	std::cout << vInfoNamed("IP", WiFi.localIP().toString()) << std::endl;

}

void OTA_EXAMPLE::run()
{
	clog << "wait for start option\n";
	while(!tcpServer.getSelectedStartOption(selectedStartOption)) { delay(100); }
	selectedStartOption();

	clog << "called selected start option\n";

}