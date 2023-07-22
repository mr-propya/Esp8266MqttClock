//
// Created by Tamse, Mahesh on 16/04/23.
//
//https://github.com/marvinroger/async-mqtt-client
#ifndef CUSTOMCLOCKNEW_MQTTCLIENT_H
#define CUSTOMCLOCKNEW_MQTTCLIENT_H

#include <helpers/storage/storageWrapper.h>
#include <helpers/smartHome/AlexaWrapper.h>
#include "constants.h"
#include "vector"
#include <string>
#include <Arduino.h>
#include <cstdlib>
#include "WiFiClient.h"

#if defined (ARDUINO_ARCH_ESP8266)
#include <../.pio/libdeps/nodemcuv2/PubSubClient/src/PubSubClient.h>
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#elif defined(ESP32)
#include <../.pio/libdeps/esp32/PubSubClient/src/PubSubClient.h>
#include <../.pio/libdeps/esp32/ArduinoJson/src/ArduinoJson.h>
#include "WiFiClient.h"

#endif

class MqttClientWrapper{

private:
    char *deviceId;
    PubSubClient *mqttClient;
    std::vector<char*> subscriptionTopicPrefix;
    std::vector<std::function<void (char*, DynamicJsonDocument*, char*)>> subscriptionTopicCallback;
    MqttClientWrapper(char* deviceId);
    WiFiClient bear ;

public:
    static void callBack(char* topic, byte *payload, int len);
    bool connectToServer();
    void poll();
    void registerCallback(char* topicPrefix, std::function<void (char*, DynamicJsonDocument*, char*)> callback);
    void publish(char* topic, char* payload, bool persist);
    static MqttClientWrapper* getMqttInstance();

};


#endif //CUSTOMCLOCKNEW_MQTTCLIENT_H
