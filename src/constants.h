//
// Created by Tamse, Mahesh on 16/04/23.
//

#ifndef CUSTOMCLOCKNEW_CONSTANTS_H
#define CUSTOMCLOCKNEW_CONSTANTS_H

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

#define MQTT_SUBSCRIPTION "cmd/#"
#define LED_CONTROL_MQTT_CMD "cmd/led/"
#define LED_CONTROL_MQTT_STAT "status/led/"
#define COLOR_MODE_CONTROL_MQTT_CMD "cmd/colorMode/"
#define CUSTOM_MSG_CONTROL_MQTT_CMD "cmd/customMsg/"
#define HEAP_UTILIZATION_DATA_MQTT_TOPIC "status/heap/available"

// ----------- LED Constants ----------- //
#define LED_PER_DIGIT_SEGMENT 7
#define SEGMENTS_PER_DIGIT 7
#define LED_PER_DOT_SEGMENT 4
#define POSITION_DOT_SEGMENT 2
#define SEGMENTS_PER_DOT 2
#define DOT_BLINK_INTERVAL 1
#define LED_TOTAL (LED_PER_DIGIT_SEGMENT*SEGMENTS_PER_DIGIT* 4 + SEGMENTS_PER_DOT*LED_PER_DOT_SEGMENT)


// ----------- LedWrapper Constants -------//
#define CLOCK_UPDATE_PARAM_RGB "color"
#define CLOCK_UPDATE_PARAM_RGB_R "r"
#define CLOCK_UPDATE_PARAM_RGB_G "g"
#define CLOCK_UPDATE_PARAM_RGB_B "b"
#define CLOCK_UPDATE_PARAM_COLOR_MODE "color_pattern"
#define CLOCK_UPDATE_PARAM_COLOR_PATTERN_INDEX "color_pattern_index"

//this param is for MQTT lights on HA... This has to be constant set below
#define CLOCK_UPDATE_PARAM_HA_COLOR_MODE "color_mode"
#define CLOCK_UPDATE_VAL_HA_COLOR_MODE "rgb"

#define CLOCK_UPDATE_PARAM_BRIGHTNESS "brightness"
#define CLOCK_UPDATE_PARAM_FILLER_DIGIT "filler_digit"
#define CLOCK_UPDATE_PARAM_STATE_KEY "state"
#define CLOCK_UPDATE_PARAM_STATE_ON "ON"
#define CLOCK_UPDATE_PARAM_STATE_OFF "OFF"



// ----------- ColorManager Constants -------//
#define COLOR_MANAGER_MODES_LEN 3

#define COLOR_MANAGER_MODE_RGB 0
#define COLOR_MANAGER_MODE_PALETTE 1
#define COLOR_MANAGER_MODE_PALETTE_SHUFFLE 2

#define COLOR_MANAGER_MODE_DEFAULT 1

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
