#include <stdlib.h>
#include <arpa/inet.h> 
#include <stdio.h> 
#include <string.h> 
#include <unistd.h> 
#include <pthread.h>
#include <ncurses.h>
#include <stdint.h>
#include <signal.h>
#include "mqtt.h"
#include "temperature_module_i2c.h"
#include "central_server.h"
#include "menu.h"

pthread_t tMqtt, tMenu, ti2c;
void handle_interruption(int signal);
void *climate_control(void *params);
void handle_alarm();
pthread_mutex_t set_temperature_mutex;

int main(int argc, char* argv[]) 
{ 
    signal(SIGINT, handle_interruption);
    signal(SIGALRM, handle_alarm);
    pthread_mutex_init(&set_temperature_mutex, NULL);
    ualarm(2e5, 2e5);

    struct climate climate;

    setup_i2c();
    pthread_create(&ti2c, NULL, climate_control, &climate);

    struct list_components list_components;
    list_components.total = 0;
    list_components.atual = 0;
    pthread_create(&tMqtt, NULL, mqtt_config, &list_components);
    pthread_create(&tMenu, NULL, menu, &list_components);


    pthread_join (tMqtt, NULL);
    pthread_join (ti2c, NULL);
    return 0; 
} 
  
void handle_interruption(int signal){
    endwin();
    pthread_cancel(tMenu);
    exit(0);
}

void handle_alarm(){
    pthread_mutex_unlock(&set_temperature_mutex);
}

void *climate_control(void *params) {
    while(1){
        pthread_mutex_lock(&set_temperature_mutex);
        send_temperature_and_humidity();
    }
}