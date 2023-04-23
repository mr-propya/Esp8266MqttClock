//
// Created by Tamse, Mahesh on 16/04/23.
//

#ifndef CUSTOMCLOCKNEW_LEDCONTROLWRAPPER_H
#define CUSTOMCLOCKNEW_LEDCONTROLWRAPPER_H

#include "constants.h"
#include "../.pio/libdeps/nodemcuv2/FastLED/src/FastLED.h"
#include <stack>
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>


#define CLOCK_UPDATE_PARAM_RGB "color"
#define CLOCK_UPDATE_PARAM_RGB_R "r"
#define CLOCK_UPDATE_PARAM_RGB_G "g"
#define CLOCK_UPDATE_PARAM_RGB_B "b"
#define CLOCK_UPDATE_PARAM_BRIGHTNESS "brightness"
#define CLOCK_UPDATE_PARAM_STATE_KEY "state"
#define CLOCK_UPDATE_PARAM_STATE_ON "ON"
#define CLOCK_UPDATE_PARAM_STATE_OFF "OFF"

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
    int *rgb;
    int brightness;
    bool needToPushMqttStat;
    int *black_rgb;
    int filler;
    bool isBlinking;
    long lastBlink;
    int stateOnOff;
    bool blinkState;
    void updateBulk(int start, int len, int rgb[3]);
    void blinkDots();
    void update();
    bool updateUsingJson(DynamicJsonDocument *doc);
    static void mqttCallBack(char* topic, DynamicJsonDocument *doc, char* data);
    LEDWrapper();
    void loadSavedValues();
    void publishState(bool forcePush);
public:
    void setBrightness(int b);
    void setState(int state);
    void setRgb(int r, int g, int b);
    void setDigit(int digit, int value, bool isNumber, int *rgb_l);
    void setDigit(int digit, int value, bool isNumber);
    void setNumber(int digit, int number);
    void setDotSegment(int segment, bool isOn, int *rgb_l);
    void setDotSegment(int segment, bool isOn);
    void setTime(int time);
    void loop();
    void shouldBlink(bool blinkingEnabled);
    void printStat();
    static LEDWrapper* getLedWrapperInstance();
};


#endif //CUSTOMCLOCKNEW_LEDCONTROLWRAPPER_H
