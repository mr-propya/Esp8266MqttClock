//
// Created by Tamse, Mahesh on 08/07/23.
//

#include <cstdlib>
#include "MessageFlashWrapper.h"

MessageData::MessageData(char *baseVal) {
    TOTAL_DATA_COUNT = ceil( LED_TOTAL * .75 );
    data = (unsigned int*) malloc(sizeof(unsigned int)* TOTAL_DATA_COUNT);

    int i = 0;
    char *token = strtok(baseVal, ",");
    int timeToShow = 0;
    while (token != NULL)
    {
        if(i==0){
            timeToShow = atoi(token);
        } else{
            data[i] = atoi(token);
        }
        token = strtok(NULL, ",");
        i+=1;
        if(i == TOTAL_DATA_COUNT){
            break;
        }
    }
    displayTill = millis() + timeToShow * 1000;
}

int MessageData::getDigitVal(int position, int byteNumber) {
    double baseCount =  position*.75 + byteNumber*.25;
    int baseInteger = floor(baseCount);
    int byteSegment = (((int)(baseCount*100)) % 100) /25;
    return splitIntegerIntoSegment(data[baseInteger], byteSegment);
}

MessageData::~MessageData() {
    free(data);
}

int MessageData::splitIntegerIntoSegment(unsigned int baseInt, int segmentOffset) {
    int msbStart = 31-segmentOffset*4;
    int lsbEnd = msbStart-4;
    int val = 0;
    for (int i = msbStart; i > lsbEnd ; i--) {
        val <<=1;
        val |=( baseInt>>i);
    }
    return val;
}
