//
// Created by Tamse, Mahesh on 09/07/23.
//

#include "ColorPaletteHelper.h"
// Gradient palette "bhw1_15_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_15.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 20 bytes of program space.
DEFINE_GRADIENT_PALETTE( testColorPaletteCustom123 ) {
        0,   1,  8, 87,
        71,  23,195,130,
        122, 186,248,233,
        168,  23,195,130,
        255,   1,  8, 87
};

// Gradient palette "bhw1_06_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/bhw/bhw1/tn/bhw1_06.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 16 bytes of program space.

DEFINE_GRADIENT_PALETTE( bhw1_06_gp ) {
        0, 184,  1,128,
        160,   1,193,182,
        219, 153,227,190,
        255, 255,255,255};


CRGB GradientHelper::getColorFromPalette(int ledIndex, int brightness) {
    return ColorFromPalette(palette, ledIndex + offset, brightness);
}

GradientHelper *gradientHelperInstance = nullptr;
GradientHelper* GradientHelper::getGradientHelperInstance() {
    if(gradientHelperInstance == nullptr){
        gradientHelperInstance = new GradientHelper();
    }
    return gradientHelperInstance;
}

GradientHelper::GradientHelper() {
    palette = bhw1_06_gp;
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
