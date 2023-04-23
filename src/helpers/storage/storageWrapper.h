//
// Created by Tamse, Mahesh on 16/04/23.
//

#ifndef CUSTOMCLOCKNEW_STORAGEWRAPPER_H
#define CUSTOMCLOCKNEW_STORAGEWRAPPER_H

#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#include <LittleFS.h>
#include "constants.h"

class StorageWrapper{
private:
    DynamicJsonDocument* doc;
    void getStoredJson();
    char* getCompleteContent();
    void writeCompleteContent();
    void writeRawStr(const char* s);
    StorageWrapper();
public:
    bool allFilesAvailable(char** data, int len);
    bool keyExists( char* key);
    char* getKey( char* key);
    int getNumber(char* key);
    void setKey( char* key, char* val);
    void setKey(char* key, int val);
    static StorageWrapper* getStorageWrapper();
    void reset();
    void printState();
};


#endif //CUSTOMCLOCKNEW_STORAGEWRAPPER_H
