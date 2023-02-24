#include "CommandDistributorClass.h"
#include "RelayController.h"
#include "TimerClass.h"

CommandExecutor* CommandDistributorClass::SelectObjViaName() {
    if (objName == "relay")
        return &Relay;

    if (objName == "timer")
        return &Timer;
    
}	                                        /*TODO: throw expetion "no object" here*/

void CommandDistributorClass::ParseCommand(String &command) {
    this->objName = command.substring(0, command.indexOf(':')); /*Parsing first part of the command*/
    command = command.substring(command.indexOf(':') + 1); 
}



void CommandDistributorClass::ExecuteCommand(String &command) {
    ParseCommand(command);
    TargetObject = SelectObjViaName();
    
    TargetObject->ExecuteCommand(command); /*handle exception here*/
}	

CommandDistributorClass CommandDistributor;