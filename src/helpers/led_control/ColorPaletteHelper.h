//
// Created by Tamse, Mahesh on 09/07/23.
//

#ifndef CUSTOMCLOCKNEW_COLORPALETTEHELPER_H
#define CUSTOMCLOCKNEW_COLORPALETTEHELPER_H

#include <vector>
#if defined (ARDUINO_ARCH_ESP8266)
#include "../.pio/libdeps/nodemcuv2/FastLED/src/FastLED.h"
#elif defined(ESP32)
#include "../.pio/libdeps/esp32/FastLED/src/FastLED.h"
#endif

#define GRADIENT_SHIFT_SPEED .05

class GradientHelper{
private:
    GradientHelper();
    std::vector<CRGBPalette16> availablePalettes;
    int paletteIndex;
    int offset;
    long lastUpdate;
public:
    CRGB getColorFromPalette(int ledIndex, int brightness);
    static GradientHelper* getGradientHelperInstance();
    void loop();
    void showNextPalette();
    void setPaletteIndex(int index);
};



#endif //CUSTOMCLOCKNEW_COLORPALETTEHELPER_H
