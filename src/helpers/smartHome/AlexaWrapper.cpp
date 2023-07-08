//
// Created by Tamse, Mahesh on 17/06/23.
//

#include "AlexaWrapper.h"


AlexaWrapper::AlexaWrapper() {
    espalexa = new Espalexa();
    espalexa->addDevice("Watch", handleCallback, EspalexaDeviceType::color);
    espalexa->begin();
}

AlexaWrapper* alexaWrapperInstance = nullptr;
AlexaWrapper *AlexaWrapper::getAlexaWrapperInstance() {
    if(alexaWrapperInstance == nullptr){
        alexaWrapperInstance = new AlexaWrapper();
    }
    return alexaWrapperInstance;
}

void AlexaWrapper::addCallBack(std::function<void(int,int, int, int, int, char *)> callback) {
    callbacks.push_back(callback);
}

void AlexaWrapper::handleCallback(EspalexaDevice* device) {
    Serial.println("Received callback from alexa");
    if(device->getLastChangedProperty() == EspalexaDeviceProperty::none){
        return;
    }
    int size = getAlexaWrapperInstance()->callbacks.size();
    for (int i = 0; i < size; ++i){
        std::function<void(int,int, int, int, int, char *)> callback = getAlexaWrapperInstance()->callbacks[i];
        if(device-> getLastChangedProperty() == EspalexaDeviceProperty::bri){
            callback(-1, device->getValue(), -1, -1, -1 , nullptr);
        }else if(device->getLastChangedProperty() == EspalexaDeviceProperty::off || device->getLastChangedProperty() == EspalexaDeviceProperty::on){
            callback(device->getState() ? 1: 0, -1, -1,-1, -1, nullptr);
        }
        else{
            callback(-1 , -1, device->getR(), device->getG(), device->getB(), nullptr);
        }
    }
}

void AlexaWrapper::loop() {
    espalexa->loop();
}

void AlexaWrapper::setDeviceState(bool state,int brightness, int r, int g, int b) {
    espalexa->getDevice(0)->setState(state);
    int color = 0;
    color+= r<<16;
    color+= g<<8;
    color+=b;
    espalexa->getDevice(0)->setColor(color);
    espalexa->getDevice(0)->setValue(brightness);
}
