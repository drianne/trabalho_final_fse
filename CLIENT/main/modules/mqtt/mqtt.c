#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "cJSON.h"

#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "../mqtt/mqtt.h"
#include "../led/led.h"
#include "../../main.h"
#include "../memory/config.h"

#define TAG "MQTT"
#define INITIAL_TOPIC "fse2020/160047595/dispositivos/"
#define RAIZ_TOPICS "fse2020/130126721/"

char init_message [200];

struct Information general_topic;

extern xSemaphoreHandle conexaoMQTTSemaphore;
esp_mqtt_client_handle_t client;
const char* comodo;

char* get_temperature_topic(){
    return general_topic.topic_temperature;
}

char* get_umidade_topic(){
    return general_topic.topic_humidity;
}

char* get_estado_topic(){
    return general_topic.topic_state;
}

static esp_err_t mqtt_event_handler_cb(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xSemaphoreGive(conexaoMQTTSemaphore);

            // Envia mensagem e se inscreve no tópico
            strcpy(init_message, INITIAL_TOPIC);
            strcat(init_message, get_mac_address());

            comodo = possui_valor();

            char umidade [50];
            char temperatura [50];
            char estado [50];

            if(comodo == NULL){ // Se não registrada, solicita registro
                mqtt_envia_mensagem(init_message, "novo"); // Mensagem inicial
            }else{ // Se registrada resgata os topicos que deve estar cadastrada             
                sprintf(umidade, RAIZ_TOPICS);
                strcat(umidade, comodo);
                strcat(umidade, "/humidade");
                strcpy(general_topic.topic_humidity, umidade);
                
                sprintf(estado, RAIZ_TOPICS);
                strcat(estado, comodo);
                strcat(estado, "/estado");
                strcpy(general_topic.topic_state, estado);

                sprintf(temperatura, RAIZ_TOPICS);
                strcat(temperatura, comodo);
                strcat(temperatura, "/temperatura");
                strcpy(general_topic.topic_temperature, temperatura);
            }            

            msg_id = esp_mqtt_client_subscribe(client, init_message, 0);
            msg_id = esp_mqtt_client_subscribe(client, "servidor/resposta", 0);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            flash_light();

            char topic [200];
            char data [200]; 

            sprintf(topic, "%.*s", event->topic_len, event->topic); 
            sprintf(data, "%.*s", event->data_len, event->data); 

            cJSON * json = cJSON_Parse(data); // Parse to json to access
            
            if (strcmp(topic, "fse2020/160047595/dispositivos/F0:08:D1:CC:76:20") == 0){
                cJSON * format = cJSON_GetObjectItem(json,"comodo");
                char comodo [2048];
                sprintf(comodo, format->valuestring);
                
                char umidade [50];
                char temperatura [50];
                char estado [50];

                sprintf(umidade, RAIZ_TOPICS);
                strcat(umidade, comodo);
                strcat(umidade, "/humidade");
                strcpy(general_topic.topic_humidity, umidade);
                
                sprintf(estado, RAIZ_TOPICS);
                strcat(estado, comodo);
                strcat(estado, "/estado");
                strcpy(general_topic.topic_state, estado);

                sprintf(temperatura, RAIZ_TOPICS);
                strcat(temperatura, comodo);
                strcat(temperatura, "/temperatura");
                strcpy(general_topic.topic_temperature, temperatura);
            }

            // cJSON * format = cJSON_GetObjectItem(json,"oi");
            // double value = format->valuedouble;
            // printf("%lf", value);

            // Mensagens recebidas
            // printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            // printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    mqtt_event_handler_cb(event_data);
}

void mqtt_start()
{
    esp_mqtt_client_config_t mqtt_config = {
        .uri = "mqtt://mqtt.eclipseprojects.io",
    };
    client = esp_mqtt_client_init(&mqtt_config);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    esp_mqtt_client_start(client);

   

    start_led();    
}

void mqtt_envia_mensagem(char * topico, char * mensagem){
    int message_id = esp_mqtt_client_publish(client, topico, mensagem, 0, 1, 0);
    ESP_LOGI(TAG, "Mensagem enviada, ID: %d", message_id);
}