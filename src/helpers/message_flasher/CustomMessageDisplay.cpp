//
// Created by Tamse, Mahesh on 21/07/23.
//

#include "CustomMessageDisplay.h"

void CustomMessageDisplay::mqttCallback(char *topic, DynamicJsonDocument *doc, char *rawData) {
    CustomMessage customMessage;
    if(customMessage.parseMessage(doc)){
        CustomMessageDisplay::getCustomMessageInstance()->addMessageToDisplay(customMessage);
    }
}

CustomMessageDisplay::CustomMessageDisplay() {
    MqttClientWrapper::getMqttInstance()->registerCallback(CUSTOM_MSG_CONTROL_MQTT_CMD, mqttCallback);
}

void CustomMessageDisplay::addMessageToDisplay(CustomMessage msg) {
    messagesToShow.push(msg);
}


CustomMessageDisplay* customMessageDisplayInstance = nullptr;

CustomMessageDisplay *CustomMessageDisplay::getCustomMessageInstance() {
    if(customMessageDisplayInstance == nullptr){
        customMessageDisplayInstance = new CustomMessageDisplay();
    }
    return customMessageDisplayInstance;
}

bool CustomMessageDisplay::hasMessage() {
    while (!messagesToShow.empty()){
        if(!messagesToShow.front().isValid()){
            messagesToShow.pop();
            continue;
        }
        return true;
    }
    return false;
}
