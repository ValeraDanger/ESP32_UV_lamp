#include "BluetoothSerial.h"
#include <EEPROM.h>
#include "WiFi.h"
#include "AsyncUDP.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define EEPROM_SIZE 512
//#define RELAY 22 // пин реле


class BTMessangerClass {
	private:
		// Проверка, что Bluetooth доступен на плате
		#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
		#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
		#endif

		BluetoothSerial SerialBT;
		const char commandTerminateChar = '#';
		

	public:
        typedef enum {
            OK = 0,
        } ReturnCode;

		void init() {
			SerialBT.begin("Солнышко ОУФК-01М");
			Serial.println("The device started, now you can pair it with bluetooth!");
		}

		String getRequest() {
			sendResponse(ReturnCode::OK);
			return SerialBT.readStringUntil(commandTerminateChar);
		}

		void sendResponse(int resp) {
			SerialBT.println(resp);
		}

		bool isAvailable() {
			return SerialBT.available();
		}

} BTMessanger;





class CommandExecutor {
	protected:
		virtual void SplitCommand(String command) = 0;
};



class RelayController : public CommandExecutor {
	private:
        const uint8_t RELAY_PIN = 22;

		void SplitCommand(String command) override {
			
		}	

	public:
        void init() {
            pinMode(RELAY_PIN, OUTPUT);
        }

		void turnOn() {
			digitalWrite(RELAY_PIN, HIGH);
            Serial.println("\tOn");
		}

		void turnOff() {
			digitalWrite(RELAY_PIN, LOW);
            Serial.println("\tOff");
		}

} Relay;



class TimerClass : public CommandExecutor {
    private:
        int time = 0;

		void SplitCommand(String command) override {
			
		}

        static void KeepingRelayOnTask(void *pvParameters) {
            TimerClass* self = (TimerClass*)pvParameters;       /*<self> is pointing on class object*/
            Relay.turnOn();
            self->isActive = true;
            vTaskDelay(self->time * 1000 / portTICK_RATE_MS);
            Relay.turnOff();
            self->isActive = false;
            Serial.println("Таймер остановлен");
            vTaskDelete(NULL);      /*Delete this task*/
        }

    public:
        bool isActive = false;

        void setRelayOnTimer(int time) {
            this->time = time;

            xTaskCreate(
                this->KeepingRelayOnTask,   /* Указатель на функцию, которая реализует задачу. */
                "Relay on keeper",          /* Текстовое имя задачи. Этот параметр нужен только для упрощения отладки. */
                1000,                       /* Глубина стека - самые маленькие микроконтроллеры будут использовать значение намного меньше, чем здесь указано. */
                (void*) this,               /* Pointer on class object itself */
                1,                          /* Задача будет запущена с приоритетом 1. */
                NULL                        /* Мы не будем использовать хендл задачи. */
            );  
            Serial.println("Таймер запущен");
        }
};


AsyncUDP udp;


TimerClass Timer;


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

}

void loop() {
	if (BTMessanger.isAvailable()) {
		String incStr = BTMessanger.getRequest();

		if (incStr == "1") {
			Relay.turnOn();
			isTimerActive = false;
		}

		if (incStr == "0") {
			Relay.turnOff();
			isTimerActive = false;
		}

		if (incStr[0] == '!') {
			String BTReaded = incStr.substring(1);
			Serial.println(BTReaded);
            Serial.println(getValue(BTReaded, ':', 0));
			if (getValue(BTReaded, ':', 0) == String("setwifi"))
			{
				SetWifi(getValue(BTReaded, ':', 1), getValue(BTReaded, ':', 2), getValue(BTReaded, ':', 3).toInt());
			}

			if (getValue(BTReaded, ':', 0) == String("checkssid"))
			{
				Serial.println("Checking SSID...");
				Serial.println(CheckWifiSsid());
			}

			if (getValue(BTReaded, ':', 0) == String("checkpswd"))
			{
				Serial.println("Checking password...");
				Serial.println(CheckWifiPswd());
			}

			if (getValue(BTReaded, ':', 0) == String("checkuse_wifi"))
			{
				Serial.println("Checking use_wifi...");
				Serial.println(CheckWifiUse_Wifi());
			}

			if (getValue(BTReaded, ':', 0) == String("checktime"))
			{
				if (isWIFIConnected)
				{
				Serial.println(timeClient.getFormattedTime());
				}
			}

			if (getValue(BTReaded, ':', 0) == String("settimer"))
			{
                Timer.setRelayOnTimer(5);
				// end_time = millis() + getValue(BTReaded, ':', 1).toInt() * 1000;
				// isTimerActive = true;
				// digitalWrite(RELAY, HIGH);
				// Serial.println("\t On. Таймер запущен");
			}
		}

		delay(20);
  }
}