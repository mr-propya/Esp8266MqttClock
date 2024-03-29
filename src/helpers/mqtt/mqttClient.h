//
// Created by Tamse, Mahesh on 16/04/23.
//
#ifndef CUSTOMCLOCKNEW_MQTTCLIENT_H
#define CUSTOMCLOCKNEW_MQTTCLIENT_H

#include <helpers/storage/storageWrapper.h>
#include <helpers/smartHome/AlexaWrapper.h>
#include "constants.h"
#include "vector"
#include <string>
#include <Arduino.h>
#include <cstdlib>

#if defined (ARDUINO_ARCH_ESP8266)
#include <CertStoreBearSSL.h>
#include <../.pio/libdeps/nodemcuv2/PubSubClient/src/PubSubClient.h>
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#elif defined(ESP32)
#include <../.pio/libdeps/esp32/PubSubClient/src/PubSubClient.h>
#include <../.pio/libdeps/esp32/ArduinoJson/src/ArduinoJson.h>
#include "WiFiClientSecure.h"

#endif

class MqttClientWrapper{

private:
    #if defined (ARDUINO_ARCH_ESP8266)
        BearSSL::WiFiClientSecure wiFiClientSecure;
    #elif defined(ESP32)
        WiFiClientSecure wiFiClientSecure;
    #endif

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
    void publish(char* topic, const char* payload, bool persist);
    static MqttClientWrapper* getMqttInstance();

};


#endif //CUSTOMCLOCKNEW_MQTTCLIENT_H
