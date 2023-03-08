#include <EEPROM.h>
#include "WiFi.h"
#include "AsyncUDP.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#include "CommandExecutor.h"
#include "RelayController.h"
#include "TimerClass.h"
#include "CommandDistributorClass.h"
#include "BTMessangerClass.h"

#define EEPROM_SIZE 512




AsyncUDP udp;



int USE_WIFI = 0;
String WIFI_SSID = "";
String WIFI_PSWD = "";
bool isWIFIConnected = false;
long int end_time = 0;
bool isTimerActive = false;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);


String getValue(String data, char separator, int index)
{
	int found = 0;
	int strIndex[] = {0, -1};
	int maxIndex = data.length() - 1;

	for (int i = 0; i <= maxIndex && found <= index; i++)
	{
	if (data.charAt(i) == separator || i == maxIndex)
	{
		found++;
		strIndex[0] = strIndex[1] + 1;
		strIndex[1] = (i == maxIndex) ? i + 1 : i;			
	}
	}
	return found > index ? data.substring(strIndex[0], strIndex[1]) : "";		
}

String CheckWifiSsid()
{
	String ssid = "";
	EEPROM.get(0, ssid);
	return ssid;
}

String CheckWifiPswd()
{
	String pswd = "";
	EEPROM.get(100, pswd);
	return pswd;
}

int CheckWifiUse_Wifi()
{
	int use_wifi = EEPROM.read(200);
	return use_wifi;
}

void SetWifi(String ssid, String pswd, int use_wifi)
{
	Serial.print(ssid);
	Serial.print("\t");
	Serial.print(pswd);
	Serial.print("\t");
	Serial.println(use_wifi);

	EEPROM.put(0, ssid);
	EEPROM.put(100, pswd);
	EEPROM.put(200, use_wifi);
	EEPROM.commit();

	USE_WIFI = CheckWifiUse_Wifi();
	WIFI_SSID = CheckWifiSsid();
	WIFI_PSWD = CheckWifiPswd();
	Serial.println("WIFI Включен! Данные сохранены");
}



void setup()
{
    Serial.begin(115200);
    BTMessanger.init();
    Relay.init();
    BTMessanger.startWaitingCommand();
}

void loop() {
    Serial.println(ESP.getFreeHeap());
    vTaskDelay(100);
}