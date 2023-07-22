#include <Arduino.h>
#include <string>
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include "helpers/mqtt/mqttClient.h"
#include "helpers/storage/storageWrapper.h"
#include "helpers/smartHome/AlexaWrapper.h"
#include "helpers/clock/clockWrapper.h"
#include "helpers/led_control/ledControlWrapper.h"
#include "helpers/message_flasher/CustomMessageDisplay.h"
#include "helpers/led_control/ColorManager.h"

#if defined (ARDUINO_ARCH_ESP8266)
#include "../.pio/libdeps/nodemcuv2/WiFiManager/WiFiManager.h"
#elif defined(ESP32)
#include "../.pio/libdeps/esp32/WiFiManager/WiFiManager.h"
#endif


#define BAUD_RATE 115200
#define WIFI_SETUP_SSID "NodeMCU-Setup"
#define WIFI_SETUP_PASSWORD "testSetup"

#define HTTP_POLL_BASE_URL "https://us-central1-customwatch-f5c4a.cloudfunctions.net/getData"

WiFiManager wifiManager;

StorageWrapper *storageWrapper;
ClockWrapper *clockWrapper;
LEDWrapper *ledWrapper;
MqttClientWrapper* mqttClientWrapper;
AlexaWrapper* alexaWrapper;
GradientHelper* gradientHelper;
CustomMessageDisplay* customMessageDisplay;
ColorManager* colorManager;

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
    Serial.print("Wifi internal");
    Serial.print(WiFiClient().localIP().toString());
    Serial.println(WiFiClient().connected());
}

void initialize(){
    Serial.println(ESP.getFreeHeap());
//    MQTT should always be initialized first bcoz there might be dependencies on it for below wrappers
    clockWrapper = ClockWrapper::getClockWrapperInstance();
    mqttClientWrapper = MqttClientWrapper::getMqttInstance();
    ledWrapper = LEDWrapper::getLedWrapperInstance();
    alexaWrapper = AlexaWrapper::getAlexaWrapperInstance();
    gradientHelper = GradientHelper::getGradientHelperInstance();
    customMessageDisplay = CustomMessageDisplay::getCustomMessageInstance();
    colorManager = ColorManager::getColorManagerInstance();
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
    if( (hrs == 1 && mins ==1) || (ESP.getFreeHeap() < 2000)){
        RESTART_FLAG_GLOBAL = true;
    }
    return RESTART_FLAG_GLOBAL;
}

void restartController(){
    ESP.restart();
}
long heapAvail = -1;
void loop(){
    mqttClientWrapper->poll();
    colorManager->loop();
    if (customMessageDisplay->hasMessage()){
        CustomMessage* msg = customMessageDisplay->getTop();
        if(msg != nullptr){
            msg->loop();
        }
    }else {
        int time = clockWrapper->getTime();
        ledWrapper->setTime(time);
        alexaWrapper->loop();
        storageWrapper->loop();
        ledWrapper->loop();
        gradientHelper->loop();
        if (needRestart(time) && storageWrapper->isSafeToRestart()) {
            restartController();
        }
    }
    if(heapAvail != ESP.getFreeHeap()){
        mqttClientWrapper->publish(HEAP_UTILIZATION_DATA_MQTT_TOPIC, std::to_string(ESP.getFreeHeap()).data(), false);
        heapAvail = ESP.getFreeHeap();
    }
}

