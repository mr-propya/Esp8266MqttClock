//
// Created by Tamse, Mahesh on 16/04/23.
//

#include "clockWrapper.h"



#define CLOCK_CONTROL_MQTT_CMD "cmd/clock/"

void ClockWrapper::setTimeFormat(int amPm) {
    if(amPm == 12){
        timeFormat = 12;
    }
    if(amPm == 24){
        timeFormat = 24;
    }
    StorageWrapper::getStorageWrapper()->setKey(PARAM_AM_PM, timeFormat);
}
WiFiUDP ntpUDP;
NTPClient timeClientInternal(ntpUDP, "pool.ntp.org");

ClockWrapper::ClockWrapper() {
    timeFormat= StorageWrapper::getStorageWrapper()->keyExists(PARAM_AM_PM) ? StorageWrapper::getStorageWrapper()->getNumberIfExists(PARAM_AM_PM) : 12;
    lastKnownTime = -1;
    timeClient = &timeClientInternal;
    timeClient->begin();
    timeClient->setTimeOffset(
            BASE_NTP_OFFSET
           + (StorageWrapper::getStorageWrapper()->keyExists(PARAM_MIN_OFFSET_VAL) ? StorageWrapper::getStorageWrapper()->getNumberIfExists(PARAM_MIN_OFFSET_VAL) : 0)
        );
    MqttClientWrapper::getMqttInstance()->registerCallback(CLOCK_CONTROL_MQTT_CMD, ClockWrapper::mqttCallBack);
}

int ClockWrapper::getTime() {
    if(lastKnownTime !=-1 && ( millis() - lastFetchedTime ) < NTP_POLL_INTERVAL * 1000 ){
        return lastKnownTime;
    }
    lastKnownTime = ClockWrapper::fetchTimeFromNtp();
    return lastKnownTime;
}

void ClockWrapper::setOffset(int offs) {
    offset = offs;
    timeClient->setTimeOffset(BASE_NTP_OFFSET + offset);
}

int ClockWrapper::adjust12hrFormat(int hour) {
    bool adjust = 12 == timeFormat;
    if(!adjust){
        return hour;
    }
    if(hour > 12){
        hour -=12;
    }
    return hour == 0 ? 12: hour;
}

int ClockWrapper::fetchTimeFromNtp(){
    timeClient->forceUpdate();
    int hrs = ClockWrapper::adjust12hrFormat(timeClient->getHours());
    int mins = timeClient->getMinutes();
    lastFetchedTime = millis();
    return hrs*100 + mins;
}
ClockWrapper* clockInstance= nullptr;
ClockWrapper *ClockWrapper::getClockWrapperInstance() {
    if(clockInstance == nullptr)
        clockInstance = new ClockWrapper();
    return clockInstance;
}



#define MQTT_UPDATE_CLOCK_COMMAND "updateParams"


void ClockWrapper::mqttCallBack(char *topic, DynamicJsonDocument *doc, char *data) {
    ClockWrapper* wrapper = ClockWrapper::getClockWrapperInstance();
    std::string topicIncoming(topic);
    Serial.print("Got MQTT data to update Clock wrapper : ");
    Serial.println(data);

    std::string topicPath = topicIncoming.substr(strlen(CLOCK_CONTROL_MQTT_CMD));

    if(strcmp(MQTT_UPDATE_CLOCK_COMMAND, topicPath.c_str()) == 0){
        if(doc->containsKey(PARAM_MIN_OFFSET_VAL)){
            int offset = atoi((const char*)(*doc)[PARAM_MIN_OFFSET_VAL]);
            wrapper->setOffset(offset);
            StorageWrapper::getStorageWrapper()->setKey(PARAM_MIN_OFFSET_VAL, offset);
        }
        if(doc->containsKey(PARAM_AM_PM)){
            int format = atoi((const char*)(*doc)[PARAM_AM_PM]);
            wrapper->setTimeFormat(format);
        }
    }

}
