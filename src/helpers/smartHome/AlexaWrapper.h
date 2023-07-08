//
// Created by Tamse, Mahesh on 17/06/23.
//

#ifndef CUSTOMCLOCKNEW_ALEXAWRAPPER_H
#define CUSTOMCLOCKNEW_ALEXAWRAPPER_H

#include "../../../.pio/libdeps/nodemcuv2/Espalexa/src/Espalexa.h"
#include "../../../.pio/libdeps/nodemcuv2/Espalexa/src/EspalexaDevice.h"

class AlexaWrapper {
private:
        Espalexa* espalexa;
        AlexaWrapper();
        std::vector<std::function<void (int state,int brightness, int r, int g, int b, char* mode)>> callbacks;
        static void handleCallback(EspalexaDevice* espalexaDevice);
    public:
        void addCallBack(std::function<void (int state,int brightness, int r, int g, int b, char* mode)> callback);
        static AlexaWrapper* getAlexaWrapperInstance();
        void loop();
        void setDeviceState(bool state, int brightness, int r, int g, int b);

};

#endif //CUSTOMCLOCKNEW_ALEXAWRAPPER_H
