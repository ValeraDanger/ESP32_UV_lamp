#pragma once

#include "CommandExecutor.h"

class CommandDistributorClass : CommandExecutor{
    private:
    
        String objName;
        CommandExecutor* TargetObject; 

        CommandExecutor* SelectObjViaName();	

        void ParseCommand(String* command) override;
        

    public:
        void ExecuteCommand(String* command) override;

};

extern CommandDistributorClass CommandDistributor;