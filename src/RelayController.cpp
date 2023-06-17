#include "RelayController.h"
#include "TimerClass.h"
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

    else if (this->ActionName == "status") {
        this->ActionMethod = &RelayController::sendStatus;
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
    this->isOn = true;
    BTMessanger.sendResponse(BTMessanger.RELAY_ON);
    //Timer.isPreheating = false;
}

void RelayController::turnOff() {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("\tOff");
    this->isOn = false;
    BTMessanger.sendResponse(BTMessanger.RELAY_OFF);
}

bool RelayController::getIsOn() {
    return this->isOn;
}

void RelayController::sendStatus() {
    BTMessanger.sendResponse( this->getIsOn() ? BTMessanger.RELAY_ON : BTMessanger.RELAY_OFF); //sends relay_on or relay_off according relay_isOn
}

RelayController Relay;
