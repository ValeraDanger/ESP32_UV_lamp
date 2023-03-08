#include "RelayController.h"
#include "CommandExecutor.h"
#include "BTMessangerClass.h"
#include <Arduino.h>


void RelayController::ParseCommand(String* command) { /*Write action name in class field*/
    this->ActionName = command->substring(0, command->indexOf(':')); /*Selecting action part of the command*/
    *command = command->substring(command->indexOf(':') + 1); 
}	

void RelayController::SelectActionViaName() {
    if (this->ActionName == "on") {
        this->ActionMethod = &RelayController::turnOn;                  //parse command to select method
    }

    else if (this->ActionName == "off") {
        this->ActionMethod = &RelayController::turnOff;
    }
}



void RelayController::init() {
    pinMode(RELAY_PIN, OUTPUT);
}

void RelayController::ExecuteCommand(String* command) {
    this->ParseCommand(command);
    delete command;
    this->SelectActionViaName();
    (this->*ActionMethod)();  //executing method, selected in SelectActionViaName
}	

void RelayController::turnOn() {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("\tOn");
    BTMessanger.sendResponse(BTMessanger.RELAY_ON);
}

void RelayController::turnOff() {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("\tOff");
    BTMessanger.sendResponse(BTMessanger.RELAY_OFF);
}

RelayController Relay;
