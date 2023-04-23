#pragma once

#include "CommandExecutor.h"

class TimerClass : public CommandExecutor {
    private:
        bool isPaused = false;
        unsigned long int start_time;
        int init_time;
        int time_left;
        String ActionName;
        void (TimerClass::*ActionMethod)(); //Pointer on method, command should call

		void ParseCommand(String* command) override;
        
        void SelectActionViaName();

        static void KeepingRelayOnTask(void* pvParameters);


    public:
        bool isActive = false;

        void ExecuteCommand(String* command) override;

        void setRelayOnTimer();
        void setRelayOnTimer(int time);

        void pause();
        void resume();

        int get_time_left();
        void send_time_left();
};

extern TimerClass Timer;