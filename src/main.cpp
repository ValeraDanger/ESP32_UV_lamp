#include "BluetoothSerial.h"
#include <EEPROM.h>
#include "WiFi.h"
#include "AsyncUDP.h"
#include <NTPClient.h>
#include <WiFiUdp.h>

#define EEPROM_SIZE 512

// Проверка, что Bluetooth доступен на плате
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;
AsyncUDP udp;

int RELAY = 22; // пин реле
int USE_WIFI = 0;
String WIFI_SSID = "";
String WIFI_PSWD = "";
bool isWIFIConnected = false;
long int end_time = 0;
bool isTimerActive = false;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

String PacketToStr(uint8_t *packet_data, int packet_length)
{
  char str[(packet_length) + 1];
  memcpy(str, packet_data, packet_length);
  str[packet_length] = 0; // Null termination.
  return String(str);
}

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

String BTReadLine()
{
  String string = "";
  while (true)
  {
    if (SerialBT.available())
    {
      char incomingChar = SerialBT.read(); //чтение спец.строки по БТ (начинается с !, заканчивается #)
      if (incomingChar == '#')
      {
        break;
      }
      string += incomingChar;
    }
  }
  return (string);
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
  EEPROM.begin(EEPROM_SIZE);
  SerialBT.begin("Солнышко ОУФК-01М");
  Serial.println("The device started, now you can pair it with bluetooth!");
  pinMode(RELAY, OUTPUT);
  USE_WIFI = CheckWifiUse_Wifi();
  if (USE_WIFI == 1)
  {
    Serial.println("Найдены настройки WIFI! Подключаюсь...");
    WIFI_SSID = CheckWifiSsid();
    WIFI_PSWD = CheckWifiPswd();
    WiFi.begin(WIFI_SSID.c_str(), WIFI_PSWD.c_str());

    int timer = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
      if (millis() - timer > 10000)
      {
        Serial.println("Превышено время ожидания подключения!");
        break;
      }
      if (WiFi.status() == WL_CONNECTED)
      {
        isWIFIConnected = true;
      }
    }
    if (isWIFIConnected)
    {
      Serial.println("WIFI подключен!");
      timeClient.begin();
      timeClient.setTimeOffset(10800);
    }

    else
    {
      Serial.println("Ошибка подключения к WIFI! Проверьте введенные данные!");
    }
  }
}

void loop()
{
  if (isWIFIConnected)
  {
    timeClient.update();
  }
  if (Serial.available())
  {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available())
  {
    char incomingChar = SerialBT.read();

    if (incomingChar == '1')
    {
      digitalWrite(RELAY, HIGH);
      Serial.println("\t On");
      isTimerActive = false;
    }

    if (incomingChar == '0')
    {
      digitalWrite(RELAY, LOW);
      Serial.println("\t Off");
      isTimerActive = false;
    }

    if (incomingChar == '!')
    {
      String BTReaded = BTReadLine();
      Serial.println(BTReaded);
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
        end_time = millis() + getValue(BTReaded, ':', 1).toInt() * 1000;
        isTimerActive = true;
        digitalWrite(RELAY, HIGH);
        Serial.println("\t On. Таймер запущен");
      }
    }

    delay(20);
  }
  if (isTimerActive && (millis() >= end_time))
  {
    digitalWrite(RELAY, LOW);
    Serial.println("\t Off. Таймер остановлен");
    isTimerActive = false;
  }
}