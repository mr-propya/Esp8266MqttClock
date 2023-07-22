#include <Arduino.h>
#include <string>
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "helpers/mqtt/mqttClient.h"
#include "helpers/storage/storageWrapper.h"
#include "helpers/smartHome/AlexaWrapper.h"
#include "helpers/clock/clockWrapper.h"
#include "helpers/led_control/ledControlWrapper.h"
//#include "helpers/led_control/ColorPaletteHelper.h"

#if defined (ARDUINO_ARCH_ESP8266)
#include "../.pio/libdeps/nodemcuv2/WiFiManager/WiFiManager.h"
#elif defined(ESP32)
#include "../.pio/libdeps/esp32/WiFiManager/WiFiManager.h"
#endif


#define LED_PER_DIGIT_SEGMENT 7
#define SEGMENTS_PER_DIGIT 7
#define LED_PER_DOT_SEGMENT 4
#define POSITION_DOT_SEGMENT 2
#define SEGMENTS_PER_DOT 2
#define DOT_BLINK_INTERVAL 2

#define BAUD_RATE 115200
#define WIFI_SETUP_SSID "NodeMCU-Setup"
#define WIFI_SETUP_PASSWORD "testSetup"
#define NTP_POLL_INTERVAL 5
#define LED_TOTAL (LED_PER_DIGIT_SEGMENT*SEGMENTS_PER_DIGIT* 4 + SEGMENTS_PER_DOT*LED_PER_DOT_SEGMENT)




WiFiManager wifiManager;

StorageWrapper *storageWrapper;
ClockWrapper *clockWrapper;
LEDWrapper *ledWrapper;
MqttClientWrapper* mqttClientWrapper;
AlexaWrapper* alexaWrapper;
GradientHelper* gradientHelper;


int brightness = 128;

/*
 *         1
 *      ------
 *     |      |
 *   2 |   7  | 6
 *      -------
 *   3 |      | 5
 *     |      |
 *      -------
 *          4
 */

//H -> 2+4+16+32+64

std::string customInputNames[] = {
        PARAM_DEVICE_ID,
        PARAM_API_KEY,
        PARAM_AM_PM,
        PARAM_RGB_VAL,
        PARAM_BRIGHTNESS_VAL,
        PARAM_MIN_OFFSET_VAL
};

std::vector<WiFiManagerParameter*> customParamArray;

void resetSettings(){
    Serial.print("Resetting controller");
    storageWrapper->reset();
    wifiManager.resetSettings();
}

void saveConfigCallback(){
    Serial.print("Saving config from device");
    for (auto item: customParamArray){
        storageWrapper->setKey((char*)item->getID(), (char*)item->getValue());
        free(item);
    }
    storageWrapper->flushBuffer();
    customParamArray.clear();
    storageWrapper->printState();
    Serial.print("Out of write 4\n");
}

void initializeWifi(){
    Serial.print("Initializing Wifi\n");

    Serial.print("Checking vars\n");
    for (auto item: customInputNames){
        Serial.print("Checking vars ");
        Serial.println(item.c_str());
        if(!storageWrapper->keyExists((char*)item.data())){
            Serial.print("Resetting Wifi Since details not available \n");
            resetSettings();
            break;
        }
    }
    for (const std::string& item: customInputNames){
        auto* customParam = new WiFiManagerParameter(item.c_str() , item.c_str() , "", 40);
        wifiManager.addParameter(customParam);
        customParamArray.push_back(customParam);
    }
    wifiManager.setClass("invert");
    wifiManager.setSaveConfigCallback(saveConfigCallback);
    wifiManager.setConfigPortalTimeout(120);
    while(!wifiManager.autoConnect(WIFI_SETUP_SSID, WIFI_SETUP_PASSWORD)){
        delay(5000);
    }
}

void initialize(){
    Serial.println(ESP.getFreeHeap());
//    MQTT should always be initialized first bcoz there might be dependencies on it for below wrappers
    clockWrapper = ClockWrapper::getClockWrapperInstance();
    mqttClientWrapper = MqttClientWrapper::getMqttInstance();
    ledWrapper = LEDWrapper::getLedWrapperInstance();
    alexaWrapper = AlexaWrapper::getAlexaWrapperInstance();
    gradientHelper = GradientHelper::getGradientHelperInstance();
}

void setup() {
    Serial.begin(BAUD_RATE);
    Serial.print("Reading");
    storageWrapper = StorageWrapper::getStorageWrapper();
    initializeWifi();
    delay(1000);
    initialize();
    ledWrapper->shouldBlink(true);
}

bool RESTART_FLAG_GLOBAL = false;
bool needRestart(int time){
    int mins = time %100;
    int hrs = time/100;
    if(hrs == 1 && mins ==1){
        RESTART_FLAG_GLOBAL = true;
    }
    return RESTART_FLAG_GLOBAL;
}

void restartController(){
    ESP.restart();
}

void loop(){
    mqttClientWrapper->poll();
    int time = clockWrapper->getTime();
    ledWrapper->setTime(time);
    alexaWrapper->loop();
    storageWrapper->loop();
    ledWrapper->loop();
    gradientHelper-> loop();
    if(needRestart(time) && storageWrapper->isSafeToRestart()){
        restartController();
    }
    if(DEBUG_ENABLED){
        ledWrapper->printStat();
        storageWrapper->printState();
    }
}

