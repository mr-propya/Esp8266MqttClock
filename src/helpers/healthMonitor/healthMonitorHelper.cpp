//
// Created by Tamse, Mahesh on 29/07/23.
//

#include "healthMonitorHelper.h"

bool HealthMonitor::hasUpdate() {
////TODO Implement this
    return false;
}

bool HealthMonitor::shouldPublishHeap() {
    int lowerLimit = lastKnownHeap - lastKnownHeap * HEAP_DEVIATION_PERCENT/100;
    int upperLimit = lastKnownHeap + lastKnownHeap * HEAP_DEVIATION_PERCENT/100;

    if(!(ESP.getFreeHeap() >= lowerLimit || ESP.getFreeHeap() <=upperLimit) ){
        return true;
    }

    if((lastPublishedHeap + HEAP_STATUS_INTERVAL * 1000) < (millis())){
        lastPublishedHeap = millis();
        return true;
    }
    return false;
}

void HealthMonitor::loop(bool safeToRestart) {
    if(shouldPublishHeap()){
        publishHeapData();
    }
}

void HealthMonitor::publishHeapData() {
    if(lastKnownHeap != ESP.getFreeHeap()){
        MqttClientWrapper::getMqttInstance()->publish(HEAP_UTILIZATION_DATA_MQTT_TOPIC, std::to_string(ESP.getFreeHeap()).data(), false);
        lastKnownHeap = ESP.getFreeHeap();
    }
}

bool HealthMonitor::markBootAttempt() {
    return false;
}

bool HealthMonitor::markSuccessAttempt() {
    return false;
}

HealthMonitor::HealthMonitor() {

}

HealthMonitor* healthMonitorInstance = nullptr;

HealthMonitor *HealthMonitor::getHealthMonitorInstance() {
    if(healthMonitorInstance == nullptr){
        healthMonitorInstance = new HealthMonitor();
    }
    return healthMonitorInstance;
}
