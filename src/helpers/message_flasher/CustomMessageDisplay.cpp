//
// Created by Tamse, Mahesh on 21/07/23.
//

#include "CustomMessageDisplay.h"

void CustomMessageDisplay::mqttCallback(char *topic, DynamicJsonDocument *doc, char *rawData) {
    CustomMessage* customMessage = new CustomMessage();
    if(customMessage->parseMessage(doc)){
        CustomMessageDisplay::getCustomMessageInstance()->addMessageToDisplay(customMessage);
        Serial.println("Added new Custom message from ");
        Serial.println(customMessage->isValid());
        Serial.println(customMessage->getFramesLeft());
    }else{
        Serial.println("Failed parsing message");
    }
}

CustomMessageDisplay::CustomMessageDisplay() {
    MqttClientWrapper::getMqttInstance()->registerCallback(CUSTOM_MSG_CONTROL_MQTT_CMD, CustomMessageDisplay::mqttCallback);
}

void CustomMessageDisplay::addMessageToDisplay(CustomMessage* msg) {
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
    removeInvalid();
    return !messagesToShow.empty();
}

CustomMessage* CustomMessageDisplay::getTop() {
    removeInvalid();
    return messagesToShow.empty() ? nullptr : messagesToShow.front();
}

void CustomMessageDisplay::removeInvalid() {
    while (!messagesToShow.empty()){
        if(!messagesToShow.front()->isValid()){
            CustomMessage* message = messagesToShow.front();
            messagesToShow.pop();
            message->rollbackWatch();
            free(message);
        } else{
            break;
        }
    }
}
