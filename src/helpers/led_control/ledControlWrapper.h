//
// Created by Tamse, Mahesh on 16/04/23.
//

#ifndef CUSTOMCLOCKNEW_LEDCONTROLWRAPPER_H
#define CUSTOMCLOCKNEW_LEDCONTROLWRAPPER_H

#include "constants.h"
#include "./ColorManager.h"
#include "../clock/clockWrapper.h"
#include "../.pio/libdeps/nodemcuv2/FastLED/src/FastLED.h"
#include <stack>
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#include "helpers/mqtt/mqttClient.h"
#include "ColorPaletteHelper.h"
#include <string>


/*
 *         1
 *      ------
 *     |      |
 *   2 |   7  | 6
 *      -------
 *   3 |      | 5
 *     |      |
 *      -------
 *          4
 */


class LEDWrapper{
private:
    bool needToPushMqttStat;
    bool isFastLedInitialized;
    int filler;
    bool isBlinking;
    long lastBlink;
    int stateOnOff;
    bool blinkState;
    void updateBulk(int start, int len, bool state);
    void blinkDots();
    void update();
    bool updateUsingJson(DynamicJsonDocument *doc);
    static void mqttCallBack(char* topic, DynamicJsonDocument *doc, char* data);
    LEDWrapper();
    void loadSavedValues();
    void publishState(bool forcePush);
    static void alexaUpdate(int state,int brightness, int r, int g, int b, char* mode);
    void initializeFastLedIfNot();
public:
    void setState(int state);
    void timePadding(bool shouldAddPadding);
    void setDigit(int digit, int value, bool isNumber);
    void setNumber(int digit, int number);
    void setDotSegment(int segment, bool isOn);
    void setTime(int time);
    void loop();
    void shouldBlink(bool blinkingEnabled);
    void printStat();
    static LEDWrapper* getLedWrapperInstance();
};


#endif //CUSTOMCLOCKNEW_LEDCONTROLWRAPPER_H
