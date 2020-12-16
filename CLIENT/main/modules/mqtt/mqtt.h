#ifndef MQTT_H
#define MQTT_H

struct Information{    
    char topic_temperature[40];
    char topic_state[40];
    char topic_humidity[40];
};

void mqtt_start();
char * get_temperature_topic();
void mqtt_envia_mensagem(char * topico, char * mensagem);
char* get_umidade_topic();
char* get_estado_topic();

#endif