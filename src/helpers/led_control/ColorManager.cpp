//
// Created by Tamse, Mahesh on 16/07/23.
//

#include "ColorManager.h"

void ColorManager::loadSavedValues() {
    int modeTemp = StorageWrapper::getStorageWrapper()->getNumberIfExists(CLOCK_UPDATE_PARAM_COLOR_MODE);
    if(modeTemp !=-1){
        primaryColorStatus.mode = modeTemp;
    }

    int brightnessTemp = StorageWrapper::getStorageWrapper()->getNumberIfExists(PARAM_BRIGHTNESS_VAL);
    if(brightnessTemp !=-1){
        primaryColorStatus.brightness = brightnessTemp;
    }

    if(StorageWrapper::getStorageWrapper()->keyExists(PARAM_RGB_VAL)){
        char* data = StorageWrapper::getStorageWrapper()->getKey(PARAM_RGB_VAL);
        char *token = strtok(data, ",");
        int i=0;
        while (token!=NULL && i<3){
            Serial.println(token);
            primaryColorStatus.staticColorRgb[i] = atoi(token);
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
    primaryColorStatus.staticColorRgb = (int*) malloc(sizeof(int) * 3);
    fillColorArray(primaryColorStatus.staticColorRgb, 3, 255);
    primaryColorStatus.brightness = 128;
    primaryColorStatus.mode = COLOR_MANAGER_MODE_DEFAULT;
    blockUpdates = false;
    loadSavedValues();
    MqttClientWrapper::getMqttInstance()->registerCallback(COLOR_MODE_CONTROL_MQTT_CMD, ColorManager::updateColorModeMqtt);
}

void ColorManager::setRgbColor(int r, int g, int b) {
    if(blockUpdates){
        return;
    }
    primaryColorStatus.staticColorRgb[0]=r;
    primaryColorStatus.staticColorRgb[1]=g;
    primaryColorStatus.staticColorRgb[2]=b;
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
    if(blockUpdates){
        return;
    }
    primaryColorStatus.brightness = b;
    stateUpdated = true;
    StorageWrapper::getStorageWrapper()->setKey(PARAM_BRIGHTNESS_VAL, primaryColorStatus.brightness);
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
    if(blockUpdates){
        return;
    }
    primaryColorStatus.mode = m % COLOR_MANAGER_MODES_LEN;
    stateUpdated = true;
    StorageWrapper::getStorageWrapper()->setKey(CLOCK_UPDATE_PARAM_COLOR_MODE, primaryColorStatus.mode);
}

CRGB ColorManager::getPixelColor(int index, bool stateOn) {
    if(!stateOn){
        return blackColor;
    }
    switch (primaryColorStatus.mode) {
        case COLOR_MANAGER_MODE_RGB:
            return getStaticOnColor();
        case COLOR_MANAGER_MODE_PALETTE:
            return GradientHelper::getGradientHelperInstance()->getColorFromPalette(index, primaryColorStatus.brightness);
        case COLOR_MANAGER_MODE_PALETTE_SHUFFLE:
            //TODO implement shuffle
        default:
            return getStaticOnColor();
    }
}

ColorManager* colorManagerInstance = nullptr;
ColorManager *ColorManager::getColorManagerInstance() {
    if (colorManagerInstance == nullptr){
        colorManagerInstance = new ColorManager();
    }
    return colorManagerInstance;
}

int ColorManager::getBrightness() {
    return primaryColorStatus.brightness;
}

int *ColorManager::getColor() {
    return primaryColorStatus.staticColorRgb;
}


CRGB ColorManager::getStaticOnColor() {
    return CRGB(primaryColorStatus.staticColorRgb[0], primaryColorStatus.staticColorRgb[1], primaryColorStatus.staticColorRgb[2]);
}

void ColorManager::loop() {
    GradientHelper::getGradientHelperInstance()->loop();
}

void ColorManager::updateColorModeMqtt(char *topic, DynamicJsonDocument* doc, char *rawData) {
    Serial.println(rawData);
    if(doc->containsKey(CLOCK_UPDATE_PARAM_COLOR_MODE)){
        int targetMode = (*doc)[CLOCK_UPDATE_PARAM_COLOR_MODE];
        ColorManager::getColorManagerInstance()->updateColorMode(targetMode);
        if(doc->containsKey(CLOCK_UPDATE_PARAM_COLOR_PATTERN_INDEX)
                && ColorManager::getColorManagerInstance()->primaryColorStatus.mode == COLOR_MANAGER_MODE_PALETTE){
            GradientHelper::getGradientHelperInstance()->setPaletteIndex((*doc)[CLOCK_UPDATE_PARAM_COLOR_PATTERN_INDEX]);
        }else{
            GradientHelper::getGradientHelperInstance()->showNextPalette();
        }
    }
}

void ColorManager::rollbackTemporaryChanges() {
    primaryColorStatus = previousColorMode;
    blockUpdates = false;
}

void ColorManager::updateTemporary(ColorStatus temporaryColorStatus) {
    previousColorMode = primaryColorStatus;
    primaryColorStatus = temporaryColorStatus;
    blockUpdates = true;
}

