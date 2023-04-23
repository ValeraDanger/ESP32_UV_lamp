#include "TimerClass.h"
#include "RelayController.h"
#include "freertos/FreeRTOS.h"
#include "BTMessangerClass.h"
#include <Arduino.h>

void TimerClass::ParseCommand(String* command) { /*Write action part in ActionName and time part in time*/
    this->ActionName = command->substring(0, command->indexOf(':')); /*Selecting action part of the command*/
    *command = command->substring(command->indexOf(':') + 1);
    if (this->ActionName == "settimer") {
        this->init_time = command->substring(0, command->indexOf(':')).toInt() * 1000; /*Selecting time part of the command*/
        *command = command->substring(command->indexOf(':') + 1);
    }
}

void TimerClass::SelectActionViaName() {
    if (this->ActionName == "settimer") {
        this->ActionMethod = &TimerClass::setRelayOnTimer;                  //parse command to select method
    }

    else if (this->ActionName == "pause") {
        this->ActionMethod = &TimerClass::pause; 
    }

    else if (this->ActionName == "resume") {
        this->ActionMethod = &TimerClass::resume; 
    }
    
    else if (this->ActionName == "gettime") {
        this->ActionMethod = &TimerClass::send_time_left; 
    }

}

void TimerClass::KeepingRelayOnTask(void *pvParameters) {
    TimerClass* self = (TimerClass*)pvParameters;       /*<self> is pointing on class object*/
    Relay.turnOn();
    self->isActive = true;
    self->start_time = millis();
    Serial.println(self->init_time);
    self->time_left = self->init_time;
    while (self->time_left >= 0) {
        if (!self->isPaused) {
            self->time_left = self->init_time - (millis() - self->start_time);
        }
        vTaskDelay(5);
    }
    //Serial.println(self->time_left);
    //vTaskDelay(self->time * 1000 / portTICK_RATE_MS);
    Relay.turnOff();
    self->isActive = false;
    Serial.println("Таймер остановлен");
    BTMessanger.sendResponse(BTMessanger.TIMER_OFF);
    vTaskDelete(NULL);      /*Delete this task*/
}


void TimerClass::ExecuteCommand(String* command) {
    Serial.println(*command);
    this->ParseCommand(command);
    delete command;
    this->SelectActionViaName();
    (this->*ActionMethod)();  //executing method, selected in SelectActionViaName
}

void TimerClass::setRelayOnTimer() {  
    xTaskCreate(
        this->KeepingRelayOnTask,   /* Task method pointer*/
        "Relay on keeper",          /* Task name*/
        10000,                       /* Stack deepth*/
        (void*) this,               /* Pointer on class object itself */
        2,                          /* Priority*/
        NULL                        /* Task handle*/
    );  
    Serial.println("Таймер запущен");
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
}

void TimerClass::setRelayOnTimer(int time) { 
    this->init_time = time * 1000;

    xTaskCreate(
        this->KeepingRelayOnTask,   /* Task method pointer*/
        "Relay on keeper",          /* Task name*/
        10000,                       /* Stack deepth*/
        (void*) this,               /* Pointer on class object itself */
        2,                          /* Priority*/
        NULL                        /* Task handle*/
    );  
    Serial.println("Таймер запущен");
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
}

void TimerClass::pause() {
    this->isPaused = true;
    this->init_time = this->time_left;
    BTMessanger.sendResponse(BTMessanger.TIMER_PAUSED);
}

void TimerClass::resume(){
    this->start_time = millis();
    this->isPaused = false;
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
}

int TimerClass::get_time_left() {
    return this->time_left;
}

void TimerClass::send_time_left() {
    BTMessanger.sendResponse(this->get_time_left());
}

TimerClass Timer;
