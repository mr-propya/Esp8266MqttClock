//
// Created by Tamse, Mahesh on 16/04/23.
//

#include "mqttClient.h"


MqttClientWrapper* mqttInstance = nullptr;


MqttClientWrapper::MqttClientWrapper(char *id) {
    int len = strlen(id);
    deviceId = (char*) malloc(sizeof(char) * (len+1));
    strcpy(deviceId, id);
    deviceId[len] = '\0';

    BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
    bear->setInsecure();

    mqttClient = new PubSubClient(*bear);
    mqttClient->setServer(MQTT_SERVER_HOST, MQTT_SERVER_PORT);
    mqttClient->setCallback(callBack);
    mqttClient->setKeepAlive(GLOBAL_MAX_LOOP_TIMEOUT);
    connectToServer();
}

bool MqttClientWrapper::connectToServer() {
    int tryCounter = 0;
    Serial.println(ESP.getFreeHeap());

    while (tryCounter <= MQTT_SERVER_CONNECT_RETRY && !mqttClient->connected()){
        tryCounter+=1;
        Serial.println("Wifi state ");
        Serial.println(WiFiClient().connected());
        Serial.println("Trying to connect to MQTT server");
        int response = mqttClient->connect(deviceId,MQTT_SERVER_USER, MQTT_SERVER_PASSWORD);
        Serial.print("MQTT server current response code");
        Serial.println(response);
        Serial.println(mqttClient->state());
        delay(500);
        if(mqttClient->connected()){
            String subTopic;
            subTopic.concat(deviceId);
            subTopic.concat("/");
            subTopic.concat(MQTT_SUBSCRIPTION);
            Serial.print("Subscribing to MQTT topic: ");
            Serial.println(subTopic.c_str());
            Serial.println(subTopic.c_str());
            mqttClient->subscribe(subTopic.c_str(), MQTT_SERVER_QOS);
            mqttClient->subscribe("esp8266BroadCast", MQTT_SERVER_QOS);
            Serial.println("Subscribed to MQTT topic successfully");
            return true;
        }
    }
    return mqttClient->connected();
}

void MqttClientWrapper::callBack(char *topic, byte *payload, int len) {
    Serial.print(topic);
    char* data = (char*) malloc(sizeof(char)*(len+1));
    for (int i = 0; i < len; ++i) {
        data[i] = (char)payload[i];
    }
    data[len]='\0';

    Serial.print("MQTT msg received for topic : ");
    Serial.print(topic);
    Serial.print(" and data : ");
    Serial.println(data);

    std::string str(topic);

    std::string topicPath = str.size() > (strlen(mqttInstance->deviceId)+1) && str.rfind(mqttInstance->deviceId) == 0?
            str.substr(strlen(mqttInstance->deviceId)+1) : str.rfind(mqttInstance->deviceId) == 0 ? "": str;
    Serial.print("MQTT msg topic path : ");
    Serial.println(topicPath.c_str());


    DynamicJsonDocument doc(512);
    deserializeJson(doc, data);
    int size = mqttInstance->subscriptionTopicCallback.size();
    for (int i = 0; i < size; ++i) {
        char* interestedTopic = mqttInstance->subscriptionTopicPrefix[i];
        if(interestedTopic == nullptr || strlen(interestedTopic)==0 || topicPath.rfind(interestedTopic) == 0){
            Serial.print("Notifying subscriber with prefix ");
            Serial.println(interestedTopic == nullptr ? "NULL_PTR" : interestedTopic);
            mqttInstance->subscriptionTopicCallback[i](topicPath.data(), &doc, data);
        }
    }

    free(data);
    Serial.println("Finished processing for MQTT Callback");
}

void MqttClientWrapper::poll() {
    mqttClient->loop();
}

void MqttClientWrapper::registerCallback(char* topicPrefix, std::function<void(char*, DynamicJsonDocument*, char*)> callback) {
    subscriptionTopicPrefix.push_back(topicPrefix);
    subscriptionTopicCallback.push_back(callback);
}

MqttClientWrapper *MqttClientWrapper::getMqttInstance() {
    if(mqttInstance== nullptr)
        mqttInstance = new MqttClientWrapper(StorageWrapper::getStorageWrapper()->getKey(PARAM_DEVICE_ID));
    return mqttInstance;
}

void MqttClientWrapper::publish(char *subTopic, char *payload, bool persist) {
    String topic;
    topic.concat(deviceId);
    topic.concat("/");
    topic.concat(subTopic);
    Serial.print("Publishing MQTT msg to topic : ");
    Serial.print(topic);

    Serial.print(" and persist as : ");
    Serial.print(persist);

    Serial.print(" and payload : ");
    Serial.println(payload);
    if(!mqttClient->connected()){
        mqttClient->disconnect();
        connectToServer();
    }
    mqttClient->publish(topic.c_str(), payload, persist);
}

