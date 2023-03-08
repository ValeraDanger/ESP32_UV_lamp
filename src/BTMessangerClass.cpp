#include "BTMessangerClass.h"
#include "CommandDistributorClass.h"


void BTMessangerClass::WaitForCommandTask(void *pvParameters) {
    BTMessangerClass* self = (BTMessangerClass*)pvParameters;
    for (;;) {
        if (self->isAvailable()) {
            String request = self->getRequest();
            String* command = new String(request);
            //delete &request;
            CommandDistributor.ExecuteCommand(command);
        }
        vTaskDelay(10);
    }
}


void BTMessangerClass::init() {
    SerialBT.begin("Солнышко ОУФК-01М");
    Serial.println("The device started, now you can pair it with bluetooth!");
}

void BTMessangerClass::startWaitingCommand() {
    xTaskCreate(
        this->WaitForCommandTask,                           /* Task method pointer*/
        "Waiter for Bluetooth command task",                /* Task name*/
        10000,                                               /* Stack deepth*/
        (void*) this,                                       /* Pointer on class object itself */
        1,                                                   /* Priority*/
        NULL                                                /* Task handle*/
    ); 
}

String BTMessangerClass::getRequest() {
    sendResponse(ReturnCode::OK);
    return SerialBT.readStringUntil(commandTerminateChar);;
}

void BTMessangerClass::sendResponse(int resp) {
    SerialBT.print(resp);
    SerialBT.println("#");
}

bool BTMessangerClass::isAvailable() {
    return SerialBT.available();
}


BTMessangerClass BTMessanger;