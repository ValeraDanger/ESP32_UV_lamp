#include "TimerClass.h"
#include "RelayController.h"
#include "freertos/FreeRTOS.h"
#include "BTMessangerClass.h"
#include <Arduino.h>

void TimerClass::ParseCommand(String &command) { /*Write action part in ActionName and time part in time*/
    this->ActionName = command.substring(0, command.indexOf(':')); /*Selecting action part of the command*/
    command = command.substring(command.indexOf(':') + 1);
    this->time = command.substring(0, command.indexOf(':')).toInt(); /*Selecting time part of the command*/
    command = command.substring(command.indexOf(':') + 1);
}

void TimerClass::SelectActionViaName() {
    if (this->ActionName == "settimer") {
        this->ActionMethod = &TimerClass::setRelayOnTimer;                  //parse command to select method
    }
}

void TimerClass::KeepingRelayOnTask(void *pvParameters) {
    TimerClass* self = (TimerClass*)pvParameters;       /*<self> is pointing on class object*/
    Relay.turnOn();
    self->isActive = true;
    vTaskDelay(self->time * 1000 / portTICK_RATE_MS);
    Relay.turnOff();
    self->isActive = false;
    Serial.println("Таймер остановлен");
    BTMessanger.sendResponse(BTMessanger.TIMER_OFF);
    vTaskDelete(NULL);      /*Delete this task*/
}


void TimerClass::ExecuteCommand(String &command) {
    Serial.println(command);
    this->ParseCommand(command);
    this->SelectActionViaName();
    (this->*ActionMethod)();  //executing method, selected in SelectActionViaName
}

void TimerClass::setRelayOnTimer() {  
    xTaskCreate(
        this->KeepingRelayOnTask,   /* Task method pointer*/
        "Relay on keeper",          /* Task name*/
        1000,                       /* Stack deepth*/
        (void*) this,               /* Pointer on class object itself */
        2,                          /* Priority*/
        NULL                        /* Task handle*/
    );  
    Serial.println("Таймер запущен");
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
}

void TimerClass::setRelayOnTimer(int time) { 
    this->time = time;

    xTaskCreate(
        this->KeepingRelayOnTask,   /* Task method pointer*/
        "Relay on keeper",          /* Task name*/
        1000,                       /* Stack deepth*/
        (void*) this,               /* Pointer on class object itself */
        2,                          /* Priority*/
        NULL                        /* Task handle*/
    );  
    Serial.println("Таймер запущен");
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
}


TimerClass Timer;
