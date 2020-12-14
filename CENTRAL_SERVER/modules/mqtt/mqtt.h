#include <MQTTClient.h>

#define MQTT_ADDRESS   "tcp://mqtt.eclipseprojects.io:1883"
#define CLIENTID       "MQTTCClientID_TESTE"  

#define MQTT_PUBLISH_TOPIC     "teste/123/publish"
#define MQTT_SUBSCRIBE_TOPIC   "fse2020/130126721/dispositivos/#"
 
 
void publish(char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void subscribe(char * topic);
void *mqtt_config(void *params);
