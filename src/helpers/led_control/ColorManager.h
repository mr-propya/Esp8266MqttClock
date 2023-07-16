//
// Created by Tamse, Mahesh on 16/07/23.
//

#ifndef CUSTOMCLOCKNEW_COLORMANAGER_H
#define CUSTOMCLOCKNEW_COLORMANAGER_H

#include "../storage/storageWrapper.h"
#include "../../constants.h"
#include "../.pio/libdeps/nodemcuv2/FastLED/src/FastLED.h"

class ColorManager {


private:
    CRGB blackColor;
    int* staticColorRgb;
    int brightness;
    int mode;
    bool stateUpdated;
    ColorManager();
    void loadSavedValues();
    CRGB getStaticOnColor();

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
    char* getStringColorMode();


};


#endif //CUSTOMCLOCKNEW_COLORMANAGER_H
