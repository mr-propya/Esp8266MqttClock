//
// Created by Tamse, Mahesh on 21/07/23.
//

#include "CustomMessage.h"

bool CustomMessage::parseMessage(DynamicJsonDocument *doc) {
    if(!(doc->containsKey(MSG_PARAM_MSG_BINARY_VAL)  && doc->containsKey(MSG_PARAM_TIME))){
        return false;
    }
    colorMode = -1;
    totalTime = (*doc)[MSG_PARAM_TIME];
    if(doc->containsKey(MSG_PARAM_COLOR_MODE)) {
        const char* colorModeFromJson = (const char*)(*doc)[MSG_PARAM_COLOR_MODE];
        if(strcmp(colorModeFromJson, MSG_PARAM_COLOR_MODE_VAL_PALETTE) == 0){
            colorMode = COLOR_MANAGER_MODE_PALETTE;
            if(doc->containsKey(MSG_PARAM_PALETTE_INDEX)){
                paletteIndex = (*doc)[MSG_PARAM_PALETTE_INDEX];
            }
        } else if(strcmp(colorModeFromJson, MSG_PARAM_COLOR_MODE_VAL_RGB) == 0){
            colorMode = COLOR_MANAGER_MODE_RGB;
            if(doc->containsKey(MSG_PARAM_COLOR_RGB)){
                rgb[0] = (*doc)[MSG_PARAM_COLOR_RGB][MSG_PARAM_COLOR_RGB_R];
                rgb[1] = (*doc)[MSG_PARAM_COLOR_RGB][MSG_PARAM_COLOR_RGB_G];
                rgb[2] = (*doc)[MSG_PARAM_COLOR_RGB][MSG_PARAM_COLOR_RGB_B];
            }
        } else if (strcmp(colorModeFromJson, MSG_PARAM_COLOR_MODE_VAL_PALETTE_SHUFFLE) == 0){
            colorMode = COLOR_MANAGER_MODE_PALETTE_SHUFFLE;
        }
    }
    if(colorMode = -1){
        colorMode = COLOR_MANAGER_MODE_DEFAULT;
    }
    if(doc->containsKey(MSG_PARAM_BRIGHTNESS)){
        brightness = (*doc)[MSG_PARAM_BRIGHTNESS];
    }
    if(doc->containsKey(MSG_PARAM_DOT_ENABLED)){
        dotSegmentOn = (*doc)[MSG_PARAM_DOT_ENABLED] == 1;
    }else{
        dotSegmentOn = false;
    }
    blinkFor = -1;
    blinkAfter = -1;
    if(doc->containsKey(MSG_PARAM_BLINK_FOR) && doc->containsKey(MSG_PARAM_BLINK_AFTER)){
        if((*doc)[MSG_PARAM_BLINK_FOR] > 0 && (*doc)[MSG_PARAM_BLINK_AFTER]){
            blinkFor = (*doc)[MSG_PARAM_BLINK_FOR];
            blinkAfter = (*doc)[MSG_PARAM_BLINK_AFTER];
        }
    }
    return parseMessageUnits(doc);
}

bool CustomMessage::parseMessageUnits(DynamicJsonDocument* doc) {
    int len = 0;
    for (JsonVariant value : ((*doc)[MSG_PARAM_MSG_BINARY_VAL]).to<JsonArray>()) {
        len++;
    }
    int* temp = (int*)malloc(sizeof(int) * len);
    int i=0;
    for (JsonVariant value : ((*doc)[MSG_PARAM_MSG_BINARY_VAL]).to<JsonArray>()) {
        temp[i]= value.as<int>();
        i++;
    }

    int framesRequired = 1;
    if (len > 4){
        framesRequired = len - 4;
    }
    int frameDuration = floor(totalTime / framesRequired);
    for (int i = 0; i < framesRequired; i++){
        MessageUnitWithData messageUnit;
        messageUnit.loadFromArray(i, min(len, 4), temp);
        messageUnit.secondToShow = frameDuration;
        messageSegmentQueue.push(messageUnit);
    }
    free(temp);
    return frameDuration > 0 && (blinkFor == -1 || blinkFor < frameDuration);
}

bool CustomMessage::isValid() {
    while (!messageSegmentQueue.empty()){
        if(!messageSegmentQueue.front().isValid()){
            messageSegmentQueue.pop();
            continue;
        }
        return true;
    }
    return false;
}

void CustomMessage::loop() {
    while (isValid()){
        initializeSetup();
        if(isBlinkOff()){
            MessageUnit blinkUnit;
            blinkUnit.secondToShow = blinkFor;
            displayUnit(blinkUnit);
        }else{
            while (!messageSegmentQueue.empty()){
                if(!messageSegmentQueue.front().isValid()){
                    messageSegmentQueue.pop();
                    continue;
                }
                displayUnit(messageSegmentQueue.front());
                break;
            }
        }
    }
    rollbackWatch();
}

void CustomMessage::rollbackWatch() {
    ColorManager* colorManager = ColorManager::getColorManagerInstance();
    colorManager->rollbackTemporaryChanges();
}

void CustomMessage::initializeSetup() {
    if(initializedColorManager){
        return;
    }
    ColorManager* colorManager = ColorManager::getColorManagerInstance();
    messageColorStatus.mode = colorMode;
    messageColorStatus.brightness = brightness;
    messageColorStatus.staticColorRgb = rgb;
    messageColorStatus.paletteIdx = paletteIndex;
    colorManager->updateTemporary(messageColorStatus);
    startTime = floor(millis()/1000);
    initializedColorManager = true;
}

bool CustomMessage::isBlinkOff() {
    if(blinkAfter == -1)
        return false;
    int cycleLen = blinkFor + blinkAfter;
    int currentTime = floor(millis()/1000 - startTime);
    int cyclePosition = currentTime % cycleLen;
    return cyclePosition > blinkAfter;
}

void CustomMessage::displayUnit(MessageUnit messageUnit) {
    messageUnit.startDisplay();
    LEDWrapper* ledWrapperInstance = LEDWrapper::getLedWrapperInstance();
    for (int i = 0; i < 4; i++){
        ledWrapperInstance->setDigit(i, messageUnit.getDisplayChar(i), false);
    }
    for (int i = 0; i < 2; i++){
        ledWrapperInstance->setDotSegment(i, dotSegmentOn && messageUnit.isOn);
    }
    ledWrapperInstance->update();
}


bool MessageUnit::isValid() {
    return startTime == -1 || (startTime + secondToShow * 1000 ) < millis() ;
}

void MessageUnit::startDisplay() {
    if(startTime == -1){
        startTime = millis();
    }
}

int MessageUnit::getDisplayChar(int index) {
    return 0;
}

MessageUnitWithData::MessageUnitWithData() {
    for (int i = 0; i < 4; ++i){
        digitData[i] = 0;
    }
    isOn = true;
}

void MessageUnitWithData::loadFromArray(int start, int count, int* arr) {
    count = min(count, 4);
    for (int i = 0; i < count; ++i){
        digitData[i] = arr[i+start];
    }
}

int MessageUnitWithData::getDisplayChar(int index) {
    return digitData[min(3, index)];
}
