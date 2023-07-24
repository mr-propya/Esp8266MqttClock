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
        Serial.print("Color mode from JSON ");
        Serial.println(colorModeFromJson);
        if(strcmp(colorModeFromJson, MSG_PARAM_COLOR_MODE_VAL_PALETTE) == 0){
            colorMode = COLOR_MANAGER_MODE_PALETTE;
            if(doc->containsKey(MSG_PARAM_PALETTE_INDEX)){
                paletteIndex = (*doc)[MSG_PARAM_PALETTE_INDEX];
            }
        } else if(strcmp(colorModeFromJson, MSG_PARAM_COLOR_MODE_VAL_RGB) == 0){
            Serial.println("Setting up RGB mode");
            colorMode = COLOR_MANAGER_MODE_RGB;
            if(doc->containsKey(MSG_PARAM_COLOR_RGB)){
                rgb[0] = (*doc)[MSG_PARAM_COLOR_RGB][MSG_PARAM_COLOR_RGB_R];
                rgb[1] = (*doc)[MSG_PARAM_COLOR_RGB][MSG_PARAM_COLOR_RGB_G];
                rgb[2] = (*doc)[MSG_PARAM_COLOR_RGB][MSG_PARAM_COLOR_RGB_B];
                Serial.println(rgb[0]);
            }
        } else if (strcmp(colorModeFromJson, MSG_PARAM_COLOR_MODE_VAL_PALETTE_SHUFFLE) == 0){
            colorMode = COLOR_MANAGER_MODE_PALETTE_SHUFFLE;
        }
    }
    if(colorMode == -1){
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
        if((*doc)[MSG_PARAM_BLINK_FOR] > 0 && (*doc)[MSG_PARAM_BLINK_AFTER] > 0){
            blinkFor = (*doc)[MSG_PARAM_BLINK_FOR];
            blinkAfter = (*doc)[MSG_PARAM_BLINK_AFTER];
        }
    }
    return parseMessageUnits(doc);
}

bool CustomMessage::parseMessageUnits(DynamicJsonDocument* doc) {
    JsonArray arrTemp = (*doc)[MSG_PARAM_MSG_BINARY_VAL].as<JsonArray>();
    int len = arrTemp.size();
    int framesRequired = 1;
    if (len > 4){
        framesRequired = len - 3;
    }
    int frameDuration = floor(totalTime / framesRequired);
    bool isValidData = frameDuration > 0 && (blinkFor == -1 || blinkFor < frameDuration);
    if(!isValidData){
        return false;
    }


    int* temp = (int*)malloc(sizeof(int) * len);
    int i=0;
    for (JsonVariant value : arrTemp) {
        Serial.println(value.as<int>());
        temp[i]= value.as<int>();
        i++;
    }
    Serial.println("Done Parsing units");


    for (int i = 0; i < framesRequired; i++){
        MessageUnitWithData* messageUnit = new MessageUnitWithData();
        messageUnit->loadFromArray(i, min(len, 4), temp);
        messageUnit->secondToShow = frameDuration;
        messageSegmentQueue.push(messageUnit);
    }
    free(temp);
    return isValidData;
}

bool CustomMessage::isValid() {
    removeInValid();
    return !messageSegmentQueue.empty();
}

void CustomMessage::loop() {
    if (isValid()){
        initializeSetup();
        if(isBlinkOff()){
            MessageUnit blinkUnit;
            blinkUnit.secondToShow = blinkFor;
            displayUnit(&blinkUnit);
        }else{
            displayUnit(messageSegmentQueue.front());
        }
    }
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
    Serial.println("Setting up color mode");
    Serial.println(colorMode);
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

void CustomMessage::displayUnit(MessageUnit* messageUnit) {
    messageUnit->startDisplay();
    LEDWrapper* ledWrapperInstance = LEDWrapper::getLedWrapperInstance();
    for (int i = 0; i < 4; i++){
        ledWrapperInstance->setDigit(i+1, messageUnit->getDisplayChar(i), false);
    }
    for (int i = 0; i < 2; i++){
        ledWrapperInstance->setDotSegment(i+1, dotSegmentOn && messageUnit->isOn);
    }
    ledWrapperInstance->update();
}

void CustomMessage::removeInValid() {
    while (!messageSegmentQueue.empty()){
        if(!messageSegmentQueue.front()->isValid()){
            MessageUnit* msgPtr = messageSegmentQueue.front();
            free(msgPtr);
            messageSegmentQueue.pop();
        }else{
            break;
        }
    }
}

int CustomMessage::getFramesLeft() {
    return messageSegmentQueue.size();
}


bool MessageUnit::isValid() {
    return startTime == -1 || (startTime + secondToShow * 1000 ) > millis() ;
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
