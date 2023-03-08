#pragma once

#include "CommandExecutor.h"

class RelayController : public CommandExecutor{
	private:
        const uint8_t RELAY_PIN = 22;
        String ActionName;
        void (RelayController::*ActionMethod)(); //Pointer on method, command should call

		void ParseCommand(String* command) override; 

        void SelectActionViaName();

	public:
        void init();

        void ExecuteCommand(String* command) override;

		void turnOn();

		void turnOff();

};

extern RelayController Relay;