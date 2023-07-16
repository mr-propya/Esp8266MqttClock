//
// Created by Tamse, Mahesh on 16/07/23.
//

#include "ColorManager.h"

void ColorManager::loadSavedValues() {
    int modeTemp = StorageWrapper::getStorageWrapper()->getNumberIfExists(CLOCK_UPDATE_PARAM_COLOR_MODE);
    if(modeTemp !=-1){
        mode = modeTemp;
    }

    int brightnessTemp = StorageWrapper::getStorageWrapper()->getNumberIfExists(PARAM_BRIGHTNESS_VAL);
    if(brightnessTemp !=-1){
        brightness = brightnessTemp;
    }

    if(StorageWrapper::getStorageWrapper()->keyExists(PARAM_RGB_VAL)){
        char* data = StorageWrapper::getStorageWrapper()->getKey(PARAM_RGB_VAL);
        char *token = strtok(data, ",");
        int i=0;
        while (token!=NULL && i<3){
            Serial.println(token);
            staticColorRgb[i] = atoi(token);
            i++;
            token = strtok(NULL, ",");
        }
        free(data);
    }


}

void fillColorArray(int* arr, int size, int val){
    for (int i = 0; i < size; ++i){
        arr[i] = val;
    }
}

ColorManager::ColorManager() {
    blackColor = CRGB(0,0,0);
    staticColorRgb = (int*) malloc(sizeof(int)*3);
    fillColorArray(staticColorRgb, 3, 255);
    brightness = 128;
    mode = COLOR_MANAGER_MODE_RGB;
    loadSavedValues();

}

void ColorManager::setRgbColor(int r, int g, int b) {
    staticColorRgb[0]=r;
    staticColorRgb[1]=g;
    staticColorRgb[2]=b;
    String rgbString;
    rgbString.concat(r);
    rgbString.concat(",");
    rgbString.concat(g);
    rgbString.concat(",");
    rgbString.concat(b);
    StorageWrapper::getStorageWrapper()->setKey(PARAM_RGB_VAL, (char*)rgbString.c_str());
    updateColorMode(COLOR_MANAGER_MODE_RGB);
}

void ColorManager::setBrightness(int b) {
    brightness = b;
    stateUpdated = true;
    StorageWrapper::getStorageWrapper()->setKey(PARAM_BRIGHTNESS_VAL, brightness);
}

void ColorManager::setPaletteMode() {
    updateColorMode(COLOR_MANAGER_MODE_PALETTE);
}

void ColorManager::setPaletteShuffleMode() {
    updateColorMode(COLOR_MANAGER_MODE_PALETTE_SHUFFLE);
}

bool ColorManager::needsToPublishUpdate() {
    bool temp = stateUpdated;
    stateUpdated = false;
    return temp;
}

void ColorManager::updateColorMode(int m) {
    mode = m % COLOR_MANAGER_MODES;
    stateUpdated = true;
    StorageWrapper::getStorageWrapper()->setKey(CLOCK_UPDATE_PARAM_COLOR_MODE, mode);

}

CRGB ColorManager::getPixelColor(int index, bool stateOn) {
    if(!stateOn){
        return blackColor;
    }
    return getStaticOnColor();
}

ColorManager* colorManagerInstance = nullptr;
ColorManager *ColorManager::getColorManagerInstance() {
    if (colorManagerInstance == nullptr){
        colorManagerInstance = new ColorManager();
    }
    return colorManagerInstance;
}

int ColorManager::getBrightness() {
    return brightness;
}

int *ColorManager::getColor() {
    return staticColorRgb;
}

char *ColorManager::getStringColorMode() {
    return "";
}

CRGB ColorManager::getStaticOnColor() {
    return CRGB(staticColorRgb[0], staticColorRgb[1], staticColorRgb[2]);
}
