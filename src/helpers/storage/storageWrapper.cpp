//
// Created by Tamse, Mahesh on 16/04/23.
//

#include <LittleFS.h>
#include "storageWrapper.h"
#include "constants.h"
#include <../.pio/libdeps/nodemcuv2/ArduinoJson/src/ArduinoJson.h>
#include <string>

StorageWrapper* instance = nullptr;
//always free the returned ptr
// DO not direct read this since the data can be stale
char* StorageWrapper::getCompleteContent() {
    File file = LittleFS.open(STORAGE_FILE_NAME, "r");
    if(!file){
        Serial.println("Error opening storage file for read");
        return nullptr;
    }
    size_t size = file.size();
    char* readBuffer = (char*) malloc(sizeof(char) * (size+1));
    file.readBytes(readBuffer, size);
    readBuffer[size] = '\0';
    file.close();
    return readBuffer;
}

void StorageWrapper::writeRawStr(const char* s){
    File file = LittleFS.open(STORAGE_FILE_NAME, "w");
    if(!file){
        Serial.println("Error opening storage file for write");
        return;
    }
    file.write((const char*)s);
    file.flush();
    file.close();
    delay(100);

}

char outputJson[512];
void StorageWrapper::writeCompleteContent() {
    int size = measureJson(*doc);
    outputJson[0]='\0';
    serializeJson(*doc, outputJson);
    outputJson[size]= '\0';
    Serial.println("Saving JSON data");
    Serial.println(outputJson);
    writeRawStr(outputJson);
}

StorageWrapper::StorageWrapper() {
    isDocModified = false;
    doc = new DynamicJsonDocument(512);
    if(!LittleFS.begin()){
        Serial.println("Error while initializing little FS");
        return;
    }
    getStoredJson();
}

void StorageWrapper::getStoredJson() {
    if(isDocModified){
        return;
    }
    char* fileContent = getCompleteContent();
    if(fileContent == nullptr || strlen(fileContent) < 5) {
        if(fileContent != nullptr)
            free(fileContent);
        return;
    }
    Serial.println("Read data");
    Serial.println(fileContent);
    deserializeJson(*doc, (const char*)fileContent);
    free(fileContent);
}

bool StorageWrapper::keyExists(char *key) {
    Serial.print("Checking key ");
    Serial.println(key);
    return doc->containsKey(key);
}

char* StorageWrapper::getKey(char* key) {
    const char* ptr = (*doc)[key];
    int len = strlen(ptr);
    char* data = (char *)malloc(sizeof(char) * ( len +1 ));
    strcpy(data, ptr);
    data[len] = '\0';
    return data;
}

void StorageWrapper::setKey(char *key, char *val) {
    Serial.print("Saving key val as ");
    Serial.print(key);
    Serial.print(":");
    Serial.println(val);
    (*doc)[key] = val;
    markWriteToPersist();
}

bool StorageWrapper::allFilesAvailable(char** data, int len) {
    for (int i = 0; i < len; ++i) {
        if(!keyExists(data[i]))
            return false;
    }
    return true;
}

StorageWrapper *StorageWrapper::getStorageWrapper() {
    if(instance == nullptr)
        instance = new StorageWrapper();
    return instance;
}

int StorageWrapper::getNumber(char *key) {
    return atoi(getKey(key));
}

void StorageWrapper::setKey(char *key, int val) {
    char *data = (char*) malloc(sizeof(char) * 10);
    itoa(val, data, 10);
    setKey(key, data);
    free(data);
    markWriteToPersist();
}

void StorageWrapper::reset() {
    writeRawStr("");
    isDocModified = false;
    getStoredJson();
}

void StorageWrapper::printState() {
    char* data = getCompleteContent();
    Serial.println(data);
    free(data);
}

void StorageWrapper::loop() {
    if(shouldFlush()){
        flushBuffer();
    }
}

void StorageWrapper::flushBuffer() {
    Serial.println("Flushing storage buffer");
    writeCompleteContent();
    isDocModified = false;
    getStoredJson();
}

bool StorageWrapper::shouldFlush() {
    if(isDocModified && clockSinceLastModifiedCycle == WAIT_FOR_CYCLES_FOR_COMMIT){
        clockSinceLastModifiedCycle = 0;
        return true;
    }
    clockSinceLastModifiedCycle+=1;
    return false;
}

void StorageWrapper::markWriteToPersist() {
    isDocModified = true;
    clockSinceLastModifiedCycle = 0;
}
