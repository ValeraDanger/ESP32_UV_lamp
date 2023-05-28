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

    else if (this->ActionName == "gettime") {
        this->ActionMethod = &TimerClass::send_time_left; 
    }

    else if (this->ActionName == "status") {
        this->ActionMethod = &TimerClass::sendStatus; 
    }
    
}

void TimerClass::TimerTicker(void *pvParameters) {
    TimerClass* self = (TimerClass*)pvParameters;       /*<self> is pointing on class object*/

    for(;;) {
        if(self->tmr.tick()) {
            self->stop();  
            vTaskDelete(NULL); 
        }
        //Serial.println(self->tmr.timeLeft());
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
    this->tmr.force();
    this->tmr.setTime(this->time_left);
    Serial.println(this->time_left);
    this->tmr.start(); 
    Serial.println("Таймер запщуен");
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
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
    this->tmr.force();
    this->tmr.stop();
    Serial.println("Таймер остановлен");
    BTMessanger.sendResponse(BTMessanger.TIMER_OFF);
    Relay.turnOff();
    vTaskDelete(this->TimerTickerHandle);      /*Delete TimerTicker task*/
}


void TimerClass::send_time_left() {
    BTMessanger.sendResponse(this->tmr.timeLeft());
}

void TimerClass::sendStatus() {
    if (this->isPaused) {
        BTMessanger.sendResponse(BTMessanger.TIMER_PAUSED);
        return;
    }
    BTMessanger.sendResponse(this->tmr.active() ? BTMessanger.TIMER_ON : BTMessanger.TIMER_OFF); //sends timer_on or timer_off according timer_isActive
}

TimerClass Timer;
