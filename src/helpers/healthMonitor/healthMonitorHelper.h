//
// Created by Tamse, Mahesh on 29/07/23.
//

#ifndef CUSTOMCLOCKNEW_HEALTHMONITORHELPER_H
#define CUSTOMCLOCKNEW_HEALTHMONITORHELPER_H
#include <Arduino.h>
#include "constants.h"
#include "string"
#include "../mqtt/mqttClient.h"

class HealthMonitor{
private:
    long lastKnownHeap = -1;
    long lastPublishedHeap = -1;
    bool shouldPublishHeap();
    bool hasUpdate();
    void publishHeapData();
    HealthMonitor();
public:
    void loop(bool safeToRestart);
    bool markBootAttempt();
    bool markSuccessAttempt();
    static HealthMonitor* getHealthMonitorInstance();
};


#endif //CUSTOMCLOCKNEW_HEALTHMONITORHELPER_H
