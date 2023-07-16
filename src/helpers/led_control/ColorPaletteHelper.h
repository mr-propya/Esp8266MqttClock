//
// Created by Tamse, Mahesh on 09/07/23.
//

#ifndef CUSTOMCLOCKNEW_COLORPALETTEHELPER_H
#define CUSTOMCLOCKNEW_COLORPALETTEHELPER_H


#include "../../../.pio/libdeps/nodemcuv2/FastLED/src/FastLED.h"

#define GRADIENT_SHIFT_SPEED .05

class GradientHelper{
private:
    GradientHelper();
    CRGBPalette16 palette;
    int offset;
    long lastUpdate;
public:
    CRGB getColorFromPalette(int ledIndex, int brightness);
    static GradientHelper* getGradientHelperInstance();
    void loop();
};



#endif //CUSTOMCLOCKNEW_COLORPALETTEHELPER_H
