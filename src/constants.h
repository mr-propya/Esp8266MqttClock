//
// Created by Tamse, Mahesh on 16/04/23.
//

#ifndef CUSTOMCLOCKNEW_CONSTANTS_H
#define CUSTOMCLOCKNEW_CONSTANTS_H

#include <vector>

#define GLOBAL_MAX_LOOP_TIMEOUT 90
#define DEBUG_ENABLED 0

// ----------- NTP Constants ----------- //
#define NTP_POLL_INTERVAL 5
#define BASE_NTP_OFFSET (3600 * 5 + 1800)


// ----------- MQTT Constants ----------- //
#define MQTT_SERVER_HOST "43d985e2b31746e6980bc97ba5b1d77d.s1.eu.hivemq.cloud"
#define MQTT_SERVER_PORT 8883
#define MQTT_SERVER_USER "nodeUser"
#define MQTT_SERVER_PASSWORD "nodePasswd"
#define MQTT_SERVER_CONNECT_RETRY 10
#define MQTT_SERVER_QOS 1

#define LED_CONTROL_MQTT_CMD "led/cmd/"
#define LED_CONTROL_MQTT_STAT "led/status/"

// ----------- LED Constants ----------- //
#define LED_PER_DIGIT_SEGMENT 7
#define SEGMENTS_PER_DIGIT 7
#define LED_PER_DOT_SEGMENT 4
#define POSITION_DOT_SEGMENT 2
#define SEGMENTS_PER_DOT 2
#define DOT_BLINK_INTERVAL 2
#define LED_TOTAL (LED_PER_DIGIT_SEGMENT*SEGMENTS_PER_DIGIT* 4 + SEGMENTS_PER_DOT*LED_PER_DOT_SEGMENT)
#define LED_CHANNELS 4


// ----------- Storage Constants ----------- //
#define STORAGE_FILE_NAME "/data.json"
#define WAIT_FOR_CYCLES_FOR_COMMIT 100

#define PARAM_DEVICE_ID "Device_ID"
#define PARAM_API_KEY "API_Key"
#define PARAM_AM_PM "AM_PM"
#define PARAM_RGB_VAL "RGB_VALUE"
#define PARAM_BRIGHTNESS_VAL "BRIGHTNESS"
#define PARAM_MIN_OFFSET_VAL "MIN_OFFSET"


#endif //CUSTOMCLOCKNEW_CONSTANTS_H
