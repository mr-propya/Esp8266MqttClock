//
// Created by Tamse, Mahesh on 16/07/23.
//

#ifndef CUSTOMCLOCKNEW_COLORMANAGER_H
#define CUSTOMCLOCKNEW_COLORMANAGER_H

#include "../storage/storageWrapper.h"
#include "../../constants.h"
#include "./ColorPaletteHelper.h"
#include "../mqtt/mqttClient.h"

#if defined (ARDUINO_ARCH_ESP8266)
#include "../.pio/libdeps/nodemcuv2/FastLED/src/FastLED.h"
#include "../../../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h"
#elif defined(ESP32)
#include "../.pio/libdeps/esp32/FastLED/src/FastLED.h"
#include "../../../.pio/libdeps/esp32/ArduinoJson/src/ArduinoJson.h"
#endif


class ColorStatus {
public:
    int* staticColorRgb;
    int brightness;
    int mode;
    int paletteIdx;
};

class ColorManager {


private:
    CRGB blackColor;
    ColorStatus primaryColorStatus;
    bool stateUpdated;
    bool blockUpdates;
    ColorManager();
    void loadSavedValues();
    CRGB getStaticOnColor();
    ColorStatus previousColorMode;

public:
    void setRgbColor(int r, int g, int b);
    void setBrightness(int b);
    void setPaletteMode();
    void updateColorMode(int mode);
    void setPaletteShuffleMode();
    CRGB getPixelColor(int index, bool stateOn);
    bool needsToPublishUpdate();
    static ColorManager* getColorManagerInstance();
    int getBrightness();
    int* getColor();
    void updateTemporary(ColorStatus temporaryColorStatus);
    void rollbackTemporaryChanges();
    void loop();
    static void updateColorModeMqtt(char* topic, DynamicJsonDocument* doc, char* rawData);


};


#endif //CUSTOMCLOCKNEW_COLORMANAGER_H
