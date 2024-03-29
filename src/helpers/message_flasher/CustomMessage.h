//
// Created by Tamse, Mahesh on 21/07/23.
//

#ifndef CUSTOMCLOCKNEW_CUSTOMMESSAGE_H
#define CUSTOMCLOCKNEW_CUSTOMMESSAGE_H

#include <queue>
#include "../led_control/ColorManager.h"
#include "../led_control/ledControlWrapper.h"
#include "./constants.h"

#include "../led_control/ColorManager.h"
#include "../led_control/ledControlWrapper.h"
#include "./constants.h"

#if defined (ARDUINO_ARCH_ESP8266)
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#elif defined(ESP32)
#include <../.pio/libdeps/esp32/ArduinoJson/src/ArduinoJson.h>
#endif
#define MSG_PARAM_COLOR_MODE "color_mode"

#define MSG_PARAM_COLOR_MODE_VAL_PALETTE "palette"
#define MSG_PARAM_PALETTE_INDEX "paletteIndex"

#define MSG_PARAM_COLOR_MODE_VAL_PALETTE_SHUFFLE "paletteShuffle"

#define MSG_PARAM_COLOR_MODE_VAL_RGB "rgb"
#define MSG_PARAM_COLOR_RGB "color"
#define MSG_PARAM_COLOR_RGB_R "r"
#define MSG_PARAM_COLOR_RGB_G "g"
#define MSG_PARAM_COLOR_RGB_B "b"

#define MSG_PARAM_BRIGHTNESS "brightness"
#define MSG_PARAM_BLINK_FOR "blinkFor"
#define MSG_PARAM_BLINK_AFTER "blinkAfter"
#define MSG_PARAM_TIME "time"

#define MSG_PARAM_MSG_BINARY_VAL "dataString"
#define MSG_PARAM_DOT_ENABLED "dotEnabled"


class MessageUnit {
protected:
    long startTime =-1;
public:
    int secondToShow;
    bool isValid();
    void startDisplay();
    bool isOn =  false;
    virtual int getDisplayChar(int index);
};

class MessageUnitWithData : public MessageUnit{
    int digitData[4];
public:
    void loadFromArray(int start, int count, int* arr);
    MessageUnitWithData();
    int getDisplayChar(int index);

};

class CustomMessage {
private:
    std::queue<MessageUnit*> messageSegmentQueue;
    ColorStatus messageColorStatus;
    int colorMode;
    int rgb[3];
    bool dotSegmentOn;
    int paletteIndex;
    int blinkAfter;
    int blinkFor;
    int brightness;
    int totalTime;
    bool isBlinkOff();
    bool parseMessageUnits(DynamicJsonDocument* doc);
    void initializeSetup();
    bool initializedColorManager = false;
    void displayUnit(MessageUnit* messageUnit);
    void removeInValid();
public:
    int startTime;
    void rollbackWatch();
    void loop();
    bool parseMessage(DynamicJsonDocument* doc);
    bool isValid();
    int getFramesLeft();

};


#endif //CUSTOMCLOCKNEW_CUSTOMMESSAGE_H
