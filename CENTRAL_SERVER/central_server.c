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
#include "central_server.h"
#include "menu.h"

pthread_t tMqtt, tMenu;
void handle_interruption(int signal);
int main(int argc, char* argv[]) 
{ 
    signal(SIGINT, handle_interruption);
    struct list_components list_components;
    list_components.total = 0;
    list_components.atual = 0;
    pthread_create(&tMqtt, NULL, mqtt_config, &list_components);
    pthread_create(&tMenu, NULL, menu, &list_components);
    pthread_join (tMqtt, NULL);
    return 0; 
} 
  
void handle_interruption(int signal){
    endwin();
    pthread_cancel(tMenu);
    exit(0);
}