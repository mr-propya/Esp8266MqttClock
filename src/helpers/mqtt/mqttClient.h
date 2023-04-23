//
// Created by Tamse, Mahesh on 16/04/23.
//
//https://github.com/marvinroger/async-mqtt-client
#ifndef CUSTOMCLOCKNEW_MQTTCLIENT_H
#define CUSTOMCLOCKNEW_MQTTCLIENT_H

#include <../.pio/libdeps/nodemcuv2/PubSubClient/src/PubSubClient.h>
#include "../../../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h"
#include <helpers/storage/storageWrapper.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "constants.h"
#include "vector"

class MqttClientWrapper{

private:
    char *deviceId;
    PubSubClient *mqttClient;
    std::vector<char*> subscriptionTopicPrefix;
    std::vector<std::function<void (char*, DynamicJsonDocument*, char*)>> subscriptionTopicCallback;
    MqttClientWrapper(char* deviceId);
public:
    static void callBack(char* topic, byte *payload, int len);
    bool connectToServer();
    void poll();
    void registerCallback(char* topicPrefix, std::function<void (char*, DynamicJsonDocument*, char*)> callback);
    void publish(char* topic, char* payload, bool persist);
    static MqttClientWrapper* getMqttInstance();

};


#endif //CUSTOMCLOCKNEW_MQTTCLIENT_H
