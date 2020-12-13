#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <MQTTClient.h>
#include <cjson/cJSON.h>

#define MQTT_ADDRESS   "tcp://mqtt.eclipseprojects.io:1883"
#define CLIENTID       "MQTTCClientID_TESTE"  

#define MQTT_PUBLISH_TOPIC     "teste/123/publish"
#define MQTT_SUBSCRIBE_TOPIC   "teste/123/subscribe"
 
MQTTClient client;
 
void publish(MQTTClient client, char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
 
void publish(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
 
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = 2;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
}
 
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
    char* payload = message->payload;
    printf("Mensagem recebida! \n\rTopico: %s Mensagem: %s\n", topicName, payload);
    publish(client, MQTT_PUBLISH_TOPIC, payload);
 
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName); 
    return 1;
}
 
void mqtt_config()
{
   int rc;
   MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

   MQTTClient_create(&client, MQTT_ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
   MQTTClient_setCallbacks(client, NULL, NULL, on_message, NULL);
 
   rc = MQTTClient_connect(client, &conn_opts);
  
   if (rc != MQTTCLIENT_SUCCESS)
   {
       printf("\n\rFalha na conexao ao broker MQTT. Erro: %d\n", rc);
       exit(-1);
   }
 
   MQTTClient_subscribe(client, MQTT_SUBSCRIBE_TOPIC, 0);
 
   while(1){}
}