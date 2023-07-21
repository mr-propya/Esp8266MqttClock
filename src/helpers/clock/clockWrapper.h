//
// Created by Tamse, Mahesh on 16/04/23.
//

#ifndef CUSTOMCLOCKNEW_CLOCKWRAPPER_H
#define CUSTOMCLOCKNEW_CLOCKWRAPPER_H

#include "../../../.pio/libdeps/nodemcuv2/NTPClient/NTPClient.h"
#include <WiFiUdp.h>
#include <core_esp8266_features.h>
#include "constants.h"
#include "helpers/storage/storageWrapper.h"
#include "helpers/mqtt/mqttClient.h"
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>


class ClockWrapper{
private:
    long lastFetchedTime;
    int lastKnownTime;
    int timeFormat;
    NTPClient* timeClient;
    int offset;
    int adjust12hrFormat(int hrs);
    int fetchTimeFromNtp();
    ClockWrapper();
    static void mqttCallBack(char *topic, DynamicJsonDocument *doc, char *data);
public:
    void setTimeFormat(int amPm);
    void setOffset(int offset);
    int getTime();
    static ClockWrapper* getClockWrapperInstance();
};


#endif //CUSTOMCLOCKNEW_CLOCKWRAPPER_H
