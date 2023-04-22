#pragma once 
#include <Arduino.h>
#include "BluetoothSerial.h"

class BTMessangerClass {
	private:
        // Проверка, что Bluetooth доступен на плате
		#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
		#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
		#endif

		BluetoothSerial SerialBT;
		const char commandTerminateChar = '#';

        static void WaitForCommandTask(void *pvParameters);
		

	public:
        typedef enum {
            OK = 0,
            RELAY_ON,
            RELAY_OFF,
            TIMER_ON,
            TIMER_OFF,
            TIMER_PAUSED,
        } ReturnCode;

		void init();

        void startWaitingCommand();

		String getRequest();

		void sendResponse(int resp);

		bool isAvailable();

};

extern BTMessangerClass BTMessanger;