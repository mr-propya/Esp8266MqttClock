//
// Created by Tamse, Mahesh on 16/04/23.
//

#include "ledControlWrapper.h"


int DIGITS_TO_BINARY_MAPPINGS[] = {
        63, 48, 109, 121, 114, 91, 95, 49, 255, 123, 0
};

CRGB LED[LED_TOTAL];


void LEDWrapper::loadSavedValues(){

    if(StorageWrapper::getStorageWrapper()->keyExists(CLOCK_UPDATE_PARAM_STATE_KEY)){
        stateOnOff = StorageWrapper::getStorageWrapper()->getNumber(CLOCK_UPDATE_PARAM_STATE_KEY);
    } else {
        stateOnOff = 1;
    }

    if(StorageWrapper::getStorageWrapper()->keyExists(CLOCK_UPDATE_PARAM_FILLER_DIGIT)){
        filler = StorageWrapper::getStorageWrapper()->getNumber(CLOCK_UPDATE_PARAM_FILLER_DIGIT);
    } else {
        filler = 10;
    }
    needToPushMqttStat = true;
}

LEDWrapper::LEDWrapper() {
    isFastLedInitialized = false;
    loadSavedValues();
    blinkState = false;
    MqttClientWrapper::getMqttInstance()->registerCallback(LED_CONTROL_MQTT_CMD, LEDWrapper::mqttCallBack);
    AlexaWrapper::getAlexaWrapperInstance()->addCallBack(LEDWrapper::alexaUpdate);
}

void LEDWrapper::alexaUpdate(int state,int brightness, int r, int g, int b, char* mode){
    if(state != -1){
        LEDWrapper::getLedWrapperInstance()->setState(state == 1);
    }
    if(brightness !=-1){
        ColorManager::getColorManagerInstance()->setBrightness(brightness);
    }
    if(r!=-1 && g!=-1 && b!=-1){
        ColorManager::getColorManagerInstance()->setRgbColor(r, g, b);
    }
}

void LEDWrapper::loop() {
    blinkDots();
    update();
}

void LEDWrapper::setDotSegment(int segment, bool isOn) {
    int segmentHead = (SEGMENTS_PER_DIGIT * LED_PER_DIGIT_SEGMENT) * POSITION_DOT_SEGMENT;
    segmentHead += LED_PER_DOT_SEGMENT * (segment -1);
    updateBulk(segmentHead, LED_PER_DOT_SEGMENT, isOn);
}

void LEDWrapper::setDigit(int digit, int value, bool isNumber) {
    if(isNumber)
        value = DIGITS_TO_BINARY_MAPPINGS[value];

    int digitHead = (SEGMENTS_PER_DIGIT * LED_PER_DIGIT_SEGMENT) * (digit - 1);
    if(digit > POSITION_DOT_SEGMENT){
        digitHead += (LED_PER_DOT_SEGMENT * SEGMENTS_PER_DOT);
    }
    for(int i=0; i<SEGMENTS_PER_DIGIT; i++){
        boolean isOn = (value >> i) & 1;
        updateBulk(digitHead, LED_PER_DIGIT_SEGMENT, isOn);
        digitHead +=LED_PER_DIGIT_SEGMENT;
    }
}

void LEDWrapper::setNumber(int digit, int number) {
    setDigit(digit, number, true);
}

void LEDWrapper::updateBulk(int start, int len, bool state) {
    state = stateOnOff && state;
    ColorManager* colorManager = ColorManager::getColorManagerInstance();
    for (int i = 0; i < len; ++i) {
        LED[start+i] = colorManager->getPixelColor(start+i, state);
    }
}

void LEDWrapper::update() {
    initializeFastLedIfNot();
    FastLED.setBrightness(ColorManager::getColorManagerInstance()->getBrightness());
    FastLED.show();
    publishState(false);
}


int lastTime = 0;
void LEDWrapper::setTime(int time) {
    if(lastTime != time){
        needToPushMqttStat = true;
        lastTime = time;
    }
    std::stack<int> digits;
    while(time!=0){
        digits.push(time%10);
        time/=10;
    }
    while (digits.size() != 4)
        digits.push(filler);

    int i=1;
    while (!digits.empty()){
        int x = digits.top();
        setNumber(i, x);
        digits.pop();
        i+=1;
    }
}

void LEDWrapper::shouldBlink(bool blinkingEnabled) {
    isBlinking = blinkingEnabled;
    for (int i = 0; i < SEGMENTS_PER_DOT; ++i) {
        setDotSegment(i+1, false);
    }
    blinkDots();
}

void LEDWrapper::blinkDots() {
    if(!isBlinking)
        return;
    if(lastBlink != 0 && ((millis() - lastBlink) < DOT_BLINK_INTERVAL * 1000))
        return;
    blinkState = !blinkState;
    for (int i = 0; i < SEGMENTS_PER_DOT; ++i) {
        setDotSegment(i+1, blinkState);
    }
    lastBlink = millis();
}
LEDWrapper* ledInstance = nullptr;
LEDWrapper *LEDWrapper::getLedWrapperInstance() {
    if(ledInstance == nullptr)
        ledInstance = new LEDWrapper();
    return ledInstance;
}


#define MQTT_RGB_MODE_COMMAND "updateRGBMode"
#define MQTT_GENERAL_UPDATE_COMMAND "baseConfig/json"


void LEDWrapper::mqttCallBack(char *topic, DynamicJsonDocument *doc, char *data) {
    LEDWrapper* wrapper = LEDWrapper::getLedWrapperInstance();
    ColorManager* colorManagerInstance = ColorManager::getColorManagerInstance();
    std::string topicIncoming(topic);
    Serial.print("Got MQTT data to update LED wrapper : ");
    Serial.println(data);

    std::string topicPath = topicIncoming.substr(strlen(LED_CONTROL_MQTT_CMD));
    bool intentFulfilled = false;
    if(!intentFulfilled && strcmp(MQTT_GENERAL_UPDATE_COMMAND, topicPath.c_str()) == 0){
        intentFulfilled = wrapper->updateUsingJson(doc);
    }

    if(!intentFulfilled && strcmp(MQTT_RGB_MODE_COMMAND, topicPath.c_str()) == 0){
        int newMode = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_COLOR_MODE]);
        colorManagerInstance->updateColorMode(newMode);
    }

}

void LEDWrapper::printStat() {
    int* rgb = ColorManager::getColorManagerInstance()->getColor();
    Serial.println("Led Stats: ");
    Serial.print("\tRGB val: ");
    Serial.print(rgb[0]);
    Serial.print(" ");
    Serial.print(rgb[1]);
    Serial.print(" ");
    Serial.println(rgb[2]);
    Serial.print("\tTotal LED val: ");
    Serial.println(LED_TOTAL);
}

bool LEDWrapper::updateUsingJson(DynamicJsonDocument *doc) {
    bool hasUpdate = false;
    if(doc->containsKey(CLOCK_UPDATE_PARAM_STATE_KEY)){
        const char* updateState = ((const char*)(*doc)[CLOCK_UPDATE_PARAM_STATE_KEY]);
        hasUpdate  = true;
        setState(
                strcmp(updateState, CLOCK_UPDATE_PARAM_STATE_ON) == 0 ? 1:0
        );
    }

    if(doc->containsKey(CLOCK_UPDATE_PARAM_RGB)){
        int r = (*doc)[CLOCK_UPDATE_PARAM_RGB][CLOCK_UPDATE_PARAM_RGB_R];
        int g = (*doc)[CLOCK_UPDATE_PARAM_RGB][CLOCK_UPDATE_PARAM_RGB_G];
        int b = (*doc)[CLOCK_UPDATE_PARAM_RGB][CLOCK_UPDATE_PARAM_RGB_B];
        ColorManager::getColorManagerInstance()->setRgbColor(r, g, b);
        hasUpdate = true;
    }

    if(doc->containsKey(CLOCK_UPDATE_PARAM_BRIGHTNESS)) {
        hasUpdate = true;
        ColorManager::getColorManagerInstance()->setBrightness((*doc)[CLOCK_UPDATE_PARAM_BRIGHTNESS]);
    }

    if(doc->containsKey(CLOCK_UPDATE_PARAM_FILLER_DIGIT)){
        timePadding((*doc)[CLOCK_UPDATE_PARAM_FILLER_DIGIT]);
        hasUpdate  = true;
    }

    return hasUpdate;
}

void LEDWrapper::setState(int state) {
    stateOnOff = state == 1 ? 1:0;
    needToPushMqttStat = true;
    StorageWrapper::getStorageWrapper()->setKey(CLOCK_UPDATE_PARAM_STATE_KEY, state);
}

void LEDWrapper::publishState(bool forcePush) {
    ColorManager* colorManagerInstance = ColorManager::getColorManagerInstance();
    int *rgb = colorManagerInstance->getColor();

    if(!(needToPushMqttStat || forcePush || colorManagerInstance->needsToPublishUpdate()) ){
        return;
    }

    AlexaWrapper::getAlexaWrapperInstance()->setDeviceState(stateOnOff, colorManagerInstance->getBrightness(), rgb[0], rgb[1], rgb[2]);

    StaticJsonDocument<256> jsonDoc;
    jsonDoc[CLOCK_UPDATE_PARAM_STATE_KEY] = stateOnOff == 1 ? CLOCK_UPDATE_PARAM_STATE_ON : CLOCK_UPDATE_PARAM_STATE_OFF;
    jsonDoc[CLOCK_UPDATE_PARAM_BRIGHTNESS] = colorManagerInstance->getBrightness();
    jsonDoc[CLOCK_UPDATE_PARAM_FILLER_DIGIT] = filler;
    jsonDoc[CLOCK_UPDATE_PARAM_HA_COLOR_MODE] = CLOCK_UPDATE_VAL_HA_COLOR_MODE;
    JsonObject colorDoc = jsonDoc.createNestedObject(CLOCK_UPDATE_PARAM_RGB);
    colorDoc[CLOCK_UPDATE_PARAM_RGB_R] = rgb[0];
    colorDoc[CLOCK_UPDATE_PARAM_RGB_G] = rgb[1];
    colorDoc[CLOCK_UPDATE_PARAM_RGB_B] = rgb[2];

    int size = measureJson(jsonDoc);
    String jsonUpdate;
    serializeJson(jsonDoc, jsonUpdate);
    jsonUpdate[size]= '\0';

    String topic;
    topic.concat(LED_CONTROL_MQTT_STAT);
    topic.concat(MQTT_GENERAL_UPDATE_COMMAND);

    MqttClientWrapper *mqttClientWrapper = MqttClientWrapper::getMqttInstance();
    mqttClientWrapper->publish((char*)topic.c_str(), (char*)jsonUpdate.c_str(), false);
    needToPushMqttStat = false;
}

void LEDWrapper::timePadding(bool shouldAddPadding) {
    filler = shouldAddPadding ? 0 : 10;
    StorageWrapper::getStorageWrapper()->setKey(CLOCK_UPDATE_PARAM_FILLER_DIGIT, filler);
}

void LEDWrapper::initializeFastLedIfNot() {
    if(!isFastLedInitialized){
        FastLED.addLeds<WS2812, 5, GRB>(LED, LED_TOTAL).setCorrection(TypicalLEDStrip);
    }
    isFastLedInitialized = true;
}


