#include "TimerClass.h"
#include "RelayController.h"
#include "freertos/FreeRTOS.h"
#include "BTMessangerClass.h"
#include <Arduino.h>


void TimerClass::ParseCommand(String* command) { /*Write action part in ActionName and time part in time*/
    this->ActionName = command->substring(0, command->indexOf(':')); /*Selecting action part of the command*/
    *command = command->substring(command->indexOf(':') + 1);
    if (this->ActionName == "settimer") {
        this->time_left = command->substring(0, command->indexOf(':')).toInt() * 1000; /*Selecting time part of the command*/
        *command = command->substring(command->indexOf(':') + 1);
    }
}

void TimerClass::SelectActionViaName() {
    if (this->ActionName == "settimer") {
        this->ActionMethod = &TimerClass::start;                  //parse command to select method
    }

    else if (this->ActionName == "pause") {
        this->ActionMethod = &TimerClass::pause; 
    }

    else if (this->ActionName == "resume") {
        this->ActionMethod = &TimerClass::resume; 
    }
    
}

void TimerClass::TimerTicker(void *pvParameters) {
    TimerClass* self = (TimerClass*)pvParameters;       /*<self> is pointing on class object*/

    for(;;) {
        if(self->tmr.tick()) {
            self->stop();
        }
        //Serial.println(self->tmr.tick());
        vTaskDelay(5);
    }
}


void TimerClass::ExecuteCommand(String* command) {
    Serial.println(*command);
    this->ParseCommand(command);
    delete command;
    this->SelectActionViaName();
    (this->*ActionMethod)();  //executing method, selected in SelectActionViaName
}

void TimerClass::start() {  
    Relay.turnOn();
    this->isActive = true;
    this->tmr.setTime(this->time_left);
    Serial.println(this->time_left);
    this->tmr.start(); 
    Serial.println("Таймер запщуен");

    xTaskCreate(
        this->TimerTicker,   /* Task method pointer*/
        "Timer ticker",          /* Task name*/
        10000,                       /* Stack deepth*/
        (void*) this,               /* Pointer on class object itself */
        2,                          /* Priority*/
        &this->TimerTickerHandle                        /* Task handle*/
    );  

}

void TimerClass::start(int time) { 
    this->time_left = time * 1000;
    this->start();  
}

void TimerClass::pause() {
    this->tmr.stop();
    this->isPaused = true;
    Serial.println("Таймер приостановлен");
    BTMessanger.sendResponse(BTMessanger.TIMER_PAUSED);
}

void TimerClass::resume(){
    this->isPaused = false;
    this->tmr.resume();
    Serial.println("Таймер запущен после паузы");
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
}

void TimerClass::stop() {
    vTaskDelete(this->TimerTickerHandle);      /*Delete TimerTicker task*/
    Relay.turnOff();
    this->tmr.stop();
    Serial.println("Таймер остановлен");
    BTMessanger.sendResponse(BTMessanger.TIMER_OFF);
}

TimerClass Timer;
