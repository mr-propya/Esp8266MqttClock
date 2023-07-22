//
// Created by Tamse, Mahesh on 21/07/23.
//

#ifndef CUSTOMCLOCKNEW_CUSTOMMESSAGEDISPLAY_H
#define CUSTOMCLOCKNEW_CUSTOMMESSAGEDISPLAY_H

#include <queue>
#include "../../../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h"
#include "../mqtt/mqttClient.h"
#include "./CustomMessage.h"
#include "../led_control/ColorManager.h"


class CustomMessageDisplay {
private:
    CustomMessageDisplay();
    std::queue<CustomMessage*> messagesToShow;
    static void mqttCallback(char* topic, DynamicJsonDocument* doc, char* rawData);
    void removeInvalid();
public:
    bool hasMessage();
    CustomMessage* getTop();
    static CustomMessageDisplay* getCustomMessageInstance();
    void addMessageToDisplay(CustomMessage* msg);
};


#endif //CUSTOMCLOCKNEW_CUSTOMMESSAGEDISPLAY_H
