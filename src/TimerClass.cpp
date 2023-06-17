#include "TimerClass.h"
#include "RelayController.h"
#include "freertos/FreeRTOS.h"
#include "BTMessangerClass.h"
#include <Arduino.h>

#define PREHEAT_TIME 60000


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

    else if (this->ActionName == "getpreheattime") {
        this->ActionMethod = &TimerClass::send_preheat_time_left; 
    }

    else if (this->ActionName == "status") {
        this->ActionMethod = &TimerClass::sendStatus; 
    }

    else if (this->ActionName == "preheat") {
        this->ActionMethod = &TimerClass::start_preheat; 
    }

    else if (this->ActionName == "preheatstop") {
        this->ActionMethod = &TimerClass::stop_preheating; 
    }
    
}

void TimerClass::TimerTicker(void *pvParameters) {
    TimerClass* self = (TimerClass*)pvParameters;       /*<self> is pointing on class object*/

    for(;;) {
        if(self->tmr.tick()) {
            self->stop();  
            vTaskDelete(self->TimerTickerHandle); 
        }
        //Serial.println(self->tmr.timeLeft());
        vTaskDelay(5);
    }
}

void TimerClass::PreheaterTicker(void *pvParameters) {
    TimerClass* self = (TimerClass*)pvParameters;       /*<self> is pointing on class object*/

    for(;;) {
        if(self->tmr.tick()) {
            self->stop_preheating();
            vTaskDelete(self->PreheaterTickerHandle); 
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
    //this->isPreheating = false;
    vTaskDelay(10);
    this->isActive = true;
    this->tmr.stop();
    this->tmr.force();
    this->tmr.setTime(this->time_left);
    this->tmr.start(); 
    Serial.println(this->time_left);
    this->send_time_left();
    Relay.turnOn();
    
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

void TimerClass::start_preheat() {  
    this->isPreheating = true;
    this->tmr.stop();
    this->tmr.force();
    this->tmr.setTime(PREHEAT_TIME);
    this->tmr.start(); 
    this->send_preheat_time_left();
    Relay.turnOn();
    Serial.println("Преднагрев запщуен");
    BTMessanger.sendResponse(BTMessanger.PREHEAT_ON);
    xTaskCreate(
        this->PreheaterTicker,   /* Task method pointer*/
        "Preheat ticker",          /* Task name*/
        10000,                       /* Stack deepth*/
        (void*) this,               /* Pointer on class object itself */
        2,                          /* Priority*/
        &this->PreheaterTickerHandle                        /* Task handle*/
    );  

}

void TimerClass::start(int time) { 
    this->time_left = time * 1000;
    this->start();  
}

void TimerClass::pause() {
    this->tmr.stop();
    this->isPaused = true;
    this->send_time_left();
    Serial.println("Таймер приостановлен");
    BTMessanger.sendResponse(BTMessanger.TIMER_PAUSED);
}

void TimerClass::resume(){
    this->isPaused = false;
    this->send_time_left();
    this->tmr.resume();
    Serial.println("Таймер запущен после паузы");
    BTMessanger.sendResponse(BTMessanger.TIMER_ON);
}

void TimerClass::stop() {
    //this->isPreheating = false;
    this->tmr.stop();
    this->tmr.force();
    this->tmr.setTime(0);
    Serial.println("Таймер остановлен");
    this->isActive = false;
    BTMessanger.sendResponse(BTMessanger.TIMER_OFF);
    Relay.turnOff();
    vTaskDelete(this->TimerTickerHandle);      /*Delete TimerTicker task*/

}

void TimerClass::stop_preheating() {
    this->tmr.stop();
    this->tmr.force();
    this->tmr.setTime(0);
    Serial.println("Преднагрев остановлен");
    this->isPreheating = false;
    BTMessanger.sendResponse(BTMessanger.PREHEAT_OFF);
    vTaskDelete(this->PreheaterTickerHandle); 
}

void TimerClass::send_time_left() {
    BTMessanger.sendResponse(this->tmr.timeLeft());
}

void TimerClass::send_preheat_time_left() {
    if (this->isPreheating) {
        BTMessanger.sendResponse(this->tmr.timeLeft());
    }
}

void TimerClass::sendStatus() {
    BTMessanger.sendResponse(this->isPreheating ? BTMessanger.PREHEAT_ON : BTMessanger.PREHEAT_OFF);
    if (this->isPaused) {
        BTMessanger.sendResponse(BTMessanger.TIMER_PAUSED);
        return;
    }
    BTMessanger.sendResponse(this->tmr.active() ? BTMessanger.TIMER_ON : BTMessanger.TIMER_OFF); //sends timer_on or timer_off according timer_isActive
    
}

TimerClass Timer;
