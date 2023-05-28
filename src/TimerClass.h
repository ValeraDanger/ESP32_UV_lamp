#pragma once

#include <TimerMs.h>

#include "CommandExecutor.h"

class TimerClass : public CommandExecutor {
    private:
        bool isPaused = false;
        unsigned long int start_time;
        int time_left;
        String ActionName;
        

        void (TimerClass::*ActionMethod)(); //Pointer on method, command should call

		void ParseCommand(String* command) override;
        
        void SelectActionViaName();

        TaskHandle_t TimerTickerHandle;
        static void TimerTicker(void* pvParameters);


    public:
        bool isActive = false;

        TimerMs tmr = TimerMs(0, 0, 1);    /*(period, ms), (0 not started / 1 started), (mode: 0 period / 1 timer)*/

        void ExecuteCommand(String* command) override;

        void start();

        void start(int time);

        void pause();

        void resume();

        void stop();

        void send_time_left();

        void sendStatus();
};

extern TimerClass Timer;