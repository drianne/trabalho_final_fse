#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "unistd.h"
#include <MQTTClient.h>
#include <cjson/cJSON.h>
#include "mqtt.h"
#include "alarm.h"
#include "central_server.h"

MQTTClient client;
struct list_components *list_components;

void publish(char* topic, char* payload);
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
void subscribe(char * topic);
void *mqtt_config();

void publish(char* topic, char* payload) {
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
  if(strstr(topicName, "dispositivos") != NULL) {
    if (list_components->total < 10) {
      char delim[] = "/";
      char *ptr = strtok(topicName, delim);
      char *esp_id;
      for(;(ptr = strtok(NULL, "/")) != NULL; esp_id = ptr);
      int ja_existe = 0;
      for(int i=0; i<list_components->total; i++){
        if(strcmp(list_components->components[i].mac, esp_id)==0) {
          ja_existe=1;
        }
      }
      if(ja_existe == 0) {
        int pos = list_components->total;
        list_components->components[pos].temp = 30;
        list_components->components[pos].hum = 30;
        strcpy(list_components->components[pos].mac, esp_id);
        list_components->total += 1; 
        char* payload = message->payload;
        MQTTClient_freeMessage(&message);
        MQTTClient_free(topicName); 
      }
    }
  }
  else {
    char delim[] = "/";
    char *ptr = strtok(topicName, delim);
    char comodo[50];
    char topic[50];
    char url_paths[4][50];

    for(int i=0;(ptr = strtok(NULL, "/")) != NULL; i++){
      strcpy(url_paths[i], ptr);
    }
    strcpy(topic, url_paths[2]);
    strcpy(comodo, url_paths[1]);

    cJSON * json = cJSON_Parse (message->payload);

    for(int i=0; i<list_components->total; i++){
      if(strcmp(list_components->components[i].comodo, comodo)==0) {
        if(strcmp(topic, "temperatura") == 0){
          list_components->components[i].temp = cJSON_GetObjectItemCaseSensitive(json, "temperatura")->valuedouble;
        }
        else if(strcmp(topic, "humidade") == 0){
          list_components->components[i].hum = cJSON_GetObjectItemCaseSensitive(json, "humidade")->valuedouble;
        }
        else if(strcmp(topic, "estado") == 0){
          list_components->components[i].component_in_value = cJSON_GetObjectItemCaseSensitive(json, "dispositivo_entrada")->valueint;
          list_components->components[i].component_out_value = cJSON_GetObjectItemCaseSensitive(json, "dispositivo_saida")->valueint;
          if (list_components->components[i].component_in_value == 1 || list_components->components[i].component_out_value) {
            turn_on_alarm();
          }
        }
      }
    }
  }
  return 1;
}

void subscribe(char * topic) {
  MQTTClient_subscribe(client, topic, 0);
}

void *mqtt_config(void *params)
{
  list_components = params;
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
  while(1){
    sleep(1);
  }
  MQTTClient_disconnect(client, 1000);
  MQTTClient_destroy(&client);
}