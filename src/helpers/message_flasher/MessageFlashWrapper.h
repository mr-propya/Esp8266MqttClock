//
// Created by Tamse, Mahesh on 08/07/23.
//

#ifndef CUSTOMCLOCKNEW_MESSAGEFLASHWRAPPER_H
#define CUSTOMCLOCKNEW_MESSAGEFLASHWRAPPER_H

#include <tgmath.h>
#include "./constants.h"
#include "string.h"
#include <Arduino.h>

//
//7*7*4
//1 -> .75
//204 -> 153

// int -> 4bytes | From MSB. 1st Byte is R 2nd Byte is G, 3rd Byte is B and 4th Byte is next R

//1st number will be time..... If negative, turn off and next array will be empty


class MessageData{
    private:
        int TOTAL_DATA_COUNT;
        unsigned int* data;
        bool isOn;
        long displayTill;
        int splitIntegerIntoSegment(unsigned int baseInt, int segmentOffset);
public:
    MessageData(char* baseVal);
    ~MessageData();
    int getDigitVal(int position, int byteNumber);
    bool shouldBeShown();

};




#endif //CUSTOMCLOCKNEW_MESSAGEFLASHWRAPPER_H