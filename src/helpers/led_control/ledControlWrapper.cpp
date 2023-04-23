//
// Created by Tamse, Mahesh on 16/04/23.
//

#include "ledControlWrapper.h"
#include "../.pio/libdeps/nodemcuv2/FastLED/src/FastLED.h"
#include <stack>
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#include "helpers/mqtt/mqttClient.h"
#include <string>

#define LED_CONTROL_MQTT_CMD "led/cmd/"
#define LED_CONTROL_MQTT_STAT "led/status/"


int DIGITS_TO_BINARY_MAPPINGS[] = {
        63, 48, 109, 121, 114, 91, 95, 49, 255, 123, 0
};

CRGB LED[LED_TOTAL];


void LEDWrapper::loadSavedValues(){
    if(StorageWrapper::getStorageWrapper()->keyExists(PARAM_RGB_VAL)){
        Serial.println("Loading stored RGB val");
        char* data = StorageWrapper::getStorageWrapper()->getKey(PARAM_RGB_VAL);
        char *token = strtok(data, ",");
        int i=0;
        while (token!=NULL && i<3){
            Serial.println(token);
            rgb[i] = atoi(token);
            i++;
            token = strtok(NULL, ",");
        }
        free(data);
    }else{
        rgb[0]=50;
        rgb[1]=50;
        rgb[2]=50;
    }

    if(StorageWrapper::getStorageWrapper()->keyExists(PARAM_BRIGHTNESS_VAL)){
        brightness = StorageWrapper::getStorageWrapper()->getNumber(PARAM_RGB_VAL);
    } else {
        brightness = 128;
    }

    if(StorageWrapper::getStorageWrapper()->keyExists(CLOCK_UPDATE_PARAM_STATE_KEY)){
        stateOnOff = StorageWrapper::getStorageWrapper()->getNumber(CLOCK_UPDATE_PARAM_STATE_KEY);
    } else {
        stateOnOff = 1;
    }

    if(StorageWrapper::getStorageWrapper()->keyExists(PARAM_SHOW_ZERO_VAL)){
        filler = StorageWrapper::getStorageWrapper()->getNumber(PARAM_SHOW_ZERO_VAL);
    } else {
        filler = 10;
    }

    needToPushMqttStat = true;
}

LEDWrapper::LEDWrapper() {
    FastLED.addLeds<WS2812, D6, GRB>(LED, LED_TOTAL).setCorrection(TypicalLEDStrip);
    black_rgb = (int*) malloc(sizeof(int) * 3);
    rgb = (int*) malloc(sizeof(int) * 3);
    for (int i = 0; i < 3; ++i) {
        black_rgb[i]=0;
    }
    loadSavedValues();
    blinkState = false;
    MqttClientWrapper::getMqttInstance()->registerCallback(LED_CONTROL_MQTT_CMD, LEDWrapper::mqttCallBack);
}

void LEDWrapper::loop() {
    blinkDots();
    update();
}

void LEDWrapper::setDotSegment(int segment, bool isOn, int *rgb_l) {
    int segmentHead = (SEGMENTS_PER_DIGIT * LED_PER_DIGIT_SEGMENT) * POSITION_DOT_SEGMENT;
    segmentHead += LED_PER_DOT_SEGMENT * (segment -1);
    updateBulk(segmentHead, LED_PER_DOT_SEGMENT, isOn ? rgb_l : black_rgb);
}

void LEDWrapper::setDotSegment(int segment, bool isOn){
    setDotSegment(segment, isOn, rgb);
}

void LEDWrapper::setDigit(int digit, int value, bool isNumber, int *rgb_l) {
    if(isNumber)
        value = DIGITS_TO_BINARY_MAPPINGS[value];

    int digitHead = (SEGMENTS_PER_DIGIT * LED_PER_DIGIT_SEGMENT) * (digit - 1);
    if(digit > POSITION_DOT_SEGMENT){
        digitHead += (LED_PER_DOT_SEGMENT * SEGMENTS_PER_DOT);
    }
    for(int i=0; i<SEGMENTS_PER_DIGIT; i++){
        boolean isOn = (value >> i) & 1;
        int* rgbVal = isOn ? rgb_l: black_rgb;
        updateBulk(digitHead, LED_PER_DIGIT_SEGMENT, rgbVal);
        digitHead +=LED_PER_DIGIT_SEGMENT;
    }
}

void LEDWrapper::setDigit(int digit, int value, bool isNumber) {
    setDigit(digit, value, isNumber, rgb);
}

void LEDWrapper::setNumber(int digit, int number) {
    setDigit(digit, number, true);
}

void LEDWrapper::updateBulk(int start, int len, int *rgb_l) {
    if(stateOnOff !=1){
        rgb_l = black_rgb;
    }
    for (int i = 0; i < len; ++i) {
        LED[start+i] = CRGB(rgb_l[0], rgb_l[1], rgb_l[2]);
    }
    update();
}

void LEDWrapper::update() {
    FastLED.setBrightness(brightness);
    FastLED.show();
    publishState(false);
}

void LEDWrapper::setBrightness(int b) {
    brightness = b;
    needToPushMqttStat = true;
    update();
    StorageWrapper::getStorageWrapper()->setKey(CLOCK_UPDATE_PARAM_BRIGHTNESS, brightness);
}

void LEDWrapper::setTime(int time) {
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
    update();
}

void LEDWrapper::shouldBlink(bool blinkingEnabled) {
    isBlinking = blinkingEnabled;
    for (int i = 0; i < SEGMENTS_PER_DOT; ++i) {
        setDotSegment(i+1, false);
    }
    blinkDots();
    update();
}

void LEDWrapper::blinkDots() {
    if(!isBlinking)
        return;
    if(lastBlink != 0 && (lastBlink - millis()) < DOT_BLINK_INTERVAL * 1000 )
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


#define MQTT_RGB_COMMAND "updateRGB"
#define MQTT_BRIGHTNESS_COMMAND "updateBrightness"
#define MQTT_GENERAL_UPDATE_COMMAND "baseConfig/json"


void LEDWrapper::mqttCallBack(char *topic, DynamicJsonDocument *doc, char *data) {
    LEDWrapper* wrapper = LEDWrapper::getLedWrapperInstance();
    std::string topicIncoming(topic);
    Serial.print("Got MQTT data to update LED wrapper : ");
    Serial.println(data);

    std::string topicPath = topicIncoming.substr(strlen(LED_CONTROL_MQTT_CMD));
    bool intentFulfilled = false;
    if(!intentFulfilled && strcmp(MQTT_GENERAL_UPDATE_COMMAND, topicPath.c_str()) == 0){
        intentFulfilled = wrapper->updateUsingJson(doc);
    }

    if(!intentFulfilled && strcmp(MQTT_RGB_COMMAND, topicPath.c_str()) == 0){
        int r = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_RGB_R]);
        int g = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_RGB_G]);
        int b = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_RGB_B]);
        wrapper->setRgb(
            r, g, b
        );

    }

    if(!intentFulfilled && strcmp(MQTT_BRIGHTNESS_COMMAND, topicPath.c_str()) == 0){
        int newBrightness = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_BRIGHTNESS]);
        wrapper->setBrightness(newBrightness);
        StorageWrapper::getStorageWrapper()->setKey(PARAM_BRIGHTNESS_VAL, newBrightness);
    }

    wrapper->update();

}

void LEDWrapper::setRgb(int r, int g, int b) {
    rgb[0]=r;
    rgb[1]=g;
    rgb[2]=b;
    needToPushMqttStat = false;
    update();
    String rgbString;
    rgbString.concat(r);
    rgbString.concat(",");
    rgbString.concat(g);
    rgbString.concat(",");
    rgbString.concat(b);
    StorageWrapper::getStorageWrapper()->setKey(PARAM_RGB_VAL, (char*)rgbString.c_str());

}

void LEDWrapper::printStat() {
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
        char* updateState = (char*)(*doc)[CLOCK_UPDATE_PARAM_STATE_KEY];
        setState(atoi(updateState));
        hasUpdate  = true;
    }

    if(doc->containsKey(CLOCK_UPDATE_PARAM_RGB)){
        int r = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_RGB_R]);
        int g = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_RGB_G]);
        int b = atoi((const char*)(*doc)[CLOCK_UPDATE_PARAM_RGB_B]);
        setRgb(
                r, g, b
        );
        hasUpdate = true;
    }

    if(doc->containsKey(CLOCK_UPDATE_PARAM_BRIGHTNESS)){
        char* b = (char*)(*doc)[CLOCK_UPDATE_PARAM_BRIGHTNESS];
        setBrightness(atoi(b));
        hasUpdate  = true;
    }

    return hasUpdate;
}

void LEDWrapper::setState(int state) {
    stateOnOff = state == 1 ? 1:0;
    needToPushMqttStat = true;
    update();
    StorageWrapper::getStorageWrapper()->setKey(CLOCK_UPDATE_PARAM_STATE_KEY, state);
}

void LEDWrapper::publishState(bool forcePush) {
    if( !(needToPushMqttStat || forcePush) ){
        return;
    }
    String jsonUpdate;
    jsonUpdate.concat("{\"state\":");
    jsonUpdate.concat(stateOnOff == 1 ? CLOCK_UPDATE_PARAM_STATE_ON : CLOCK_UPDATE_PARAM_STATE_OFF);
    jsonUpdate.concat(",\"brightness\":");
    jsonUpdate.concat(brightness);
    jsonUpdate.concat(",\"color\":{");
    jsonUpdate.concat("\"r\":");
    jsonUpdate.concat(rgb[0]);
    jsonUpdate.concat(",\"g\":");
    jsonUpdate.concat(rgb[1]);
    jsonUpdate.concat(",\"b\":");
    jsonUpdate.concat(rgb[2]);
    jsonUpdate.concat("}}");

    String topic;
    topic.concat(LED_CONTROL_MQTT_STAT);
    topic.concat(MQTT_GENERAL_UPDATE_COMMAND);

    MqttClientWrapper *mqttClientWrapper = MqttClientWrapper::getMqttInstance();
    mqttClientWrapper->publish((char*)topic.c_str(), (char*)jsonUpdate.c_str(), false);

}


