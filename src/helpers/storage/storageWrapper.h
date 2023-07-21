//
// Created by Tamse, Mahesh on 16/04/23.
//

#ifndef CUSTOMCLOCKNEW_STORAGEWRAPPER_H
#define CUSTOMCLOCKNEW_STORAGEWRAPPER_H

#include <LittleFS.h>
#include "constants.h"
#include <string>

#if defined (ARDUINO_ARCH_ESP8266)
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#elif defined(ESP32)
#include <../.pio/libdeps/esp32/ArduinoJson/src/ArduinoJson.h>
#endif
class StorageWrapper{
private:
    DynamicJsonDocument* doc;
    void getStoredJson();
    char* getCompleteContent();
    void writeCompleteContent();
    void writeRawStr(const char* s);
    StorageWrapper();
    bool isDocModified;
    int clockSinceLastModifiedCycle;
    bool shouldFlush();
    void markWriteToPersist();
public:
    bool allFilesAvailable(char** data, int len);
    bool keyExists( char* key);
    char* getKey( char* key);
    int getNumber(char* key);
    int getNumberIfExists(char* key);
    void setKey( char* key, char* val);
    void setKey(char* key, int val);
    static StorageWrapper* getStorageWrapper();
    void reset();
    void flushBuffer();
    void printState();
    void loop();
    bool isSafeToRestart();
};


#endif //CUSTOMCLOCKNEW_STORAGEWRAPPER_H
