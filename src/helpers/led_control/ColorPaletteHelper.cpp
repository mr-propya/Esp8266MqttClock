//
// Created by Tamse, Mahesh on 09/07/23.
//

#include "ColorPaletteHelper.h"

DEFINE_GRADIENT_PALETTE( testColorPaletteCustom123 ) {
        0,   1,  8, 87,
        71,  23,195,130,
        122, 186,248,233,
        168,  23,195,130,
        255,   1,  8, 87
};

CRGB GradientHelper::getColorFromPalette(int ledIndex, int brightness) {
    return ColorFromPalette(availablePalettes[paletteIndex%availablePalettes.size()], ledIndex + offset, brightness);
}

GradientHelper *gradientHelperInstance = nullptr;
GradientHelper* GradientHelper::getGradientHelperInstance() {
    if(gradientHelperInstance == nullptr){
        gradientHelperInstance = new GradientHelper();
    }
    return gradientHelperInstance;
}

GradientHelper::GradientHelper() {
    availablePalettes.push_back(testColorPaletteCustom123);
    paletteIndex = 0;
    offset = 0;
    lastUpdate = 0;
}

void GradientHelper::loop() {
    if(millis() - lastUpdate < (GRADIENT_SHIFT_SPEED * 1000)){
        return;
    }
    lastUpdate = millis();
    offset+=1;
}

void GradientHelper::showNextPalette() {
    paletteIndex+=1;
    paletteIndex = paletteIndex % (int)availablePalettes.size();
}

void GradientHelper::setPaletteIndex(int index) {
    paletteIndex = index;
}
