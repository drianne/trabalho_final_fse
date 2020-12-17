#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include "central_server.h"
#include "module_gpio.h"
#include "temperature_module_i2c.h"
#include "mqtt.h"
#include "menu.h"

void dataInfo(struct list_components *list_components);
void menuBorders();
void menuUser(struct list_components *list_components);
void componentSelect(struct list_components *list_components);
void componentSelect(struct list_components *list_components);
int inverte_estado(int gpio_pin, int value);

int lampada_cozinha[2] = {0, 0}; //estado/indice
int lampada_sala[2] = {0, 0};

void *menu(void *params) 
{
    struct list_components *list_components = params;

    int row,col;
    int c = 0;
    initscr ();
    curs_set (0);
    while (1) {
      if (list_components->total > list_components->atual) {
        char str[80] = "";
        menuBorders();
        componentSelect(list_components);
        usleep(2e5);
        refresh ();
        clear();  
      }
      else {
        char str[80] = "";
        menuBorders();
        dataInfo(list_components);
        menuUser(list_components);
        usleep(2e5);
        refresh ();
        clear();
      }
    }
    /* End ncurses mode */
    endwin();
    return 0;
}

void dataInfo(struct list_components *list_components) {
  char mapResponse[2][10] = {"Desligado", "Ligado"};
  int row=2;
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Informações do sistema");
  row+=2;
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Temperatura do sistema: %.2lf", get_temperature());
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Umidade do sistema: %.2lf", get_humidity());
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Lâmpada cozinha: %s", mapResponse[lampada_cozinha[0]]);
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Lâmpada sala: %s", mapResponse[lampada_sala[0]]);
  row+=1;
  for(int i=0; i<list_components->atual; i++){
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "Nome do cômodo: %s", list_components->components[i].comodo);
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "Temperatura: %.2lf", list_components->components[i].temp);
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "Umidade: %.2lf", list_components->components[i].hum);
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "%s: %s", list_components->components[i].component_in, mapResponse[list_components->components[i].component_in_value]);
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "%s: %s", list_components->components[i].component_out, mapResponse[list_components->components[i].component_out_value]);
    row+=2;
  }

}

void menuBorders() {
    char doubleBordX[200] = {"========================================================================================================"};
    char singleBordx[200] = {"--------------------------------------------------------------------------------------------------------"};
    mvprintw (0+MARGIN, 1+MARGIN, doubleBordX);
    mvprintw (4+MARGIN, 1+MARGIN, doubleBordX);
    mvprintw (35+MARGIN, 1+MARGIN, singleBordx);
    for(int row=1; row<35; row++) {
      mvprintw (row+MARGIN, 0+MARGIN, "|");
      mvprintw (row+MARGIN, 55+MARGIN, "|");
      mvprintw (row+MARGIN, 105+MARGIN, "|");
    }
}

void componentSelect(struct list_components *list_components) {
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
    int row = 2;
    mvprintw (row+MARGIN, 20+MARGIN, "Novo dispositivo encontrado");
    row += 4;
    char comodo[20];
    mvprintw((row++)+MARGIN,2+MARGIN,"Digite o nome do cômodo do componente ");
    scanw(" %s", &comodo);
    strcpy(list_components->components[list_components->atual].comodo, comodo);
    char topic_subscribe[30];
    sprintf(topic_subscribe,"fse2020/130126721/%s/#", comodo);
    subscribe(topic_subscribe);

    char topic_publish[40] = "fse2020/130126721/dispositivos/";
    strcat(topic_publish, list_components->components[list_components->atual].mac);


    char dispositivo_entrada[40];
    mvprintw((row++)+MARGIN,2+MARGIN,"Digite o nome do dispositivo de entrada ");
    scanw(" %s", &dispositivo_entrada);
    strcpy(list_components->components[list_components->atual].component_in, dispositivo_entrada);

    char dispositivo_saida[40];
    mvprintw((row++)+MARGIN,2+MARGIN,"Digite o nome do dispositivo de saída ");
    scanw(" %s", &dispositivo_saida);
    strcpy(list_components->components[list_components->atual].component_out, dispositivo_saida);

    list_components->components[list_components->atual].component_in_value = 0;
    list_components->components[list_components->atual].component_out_value = 0;

    char msg [40];
    sprintf(msg,"{\"comodo\": \"%s\"}", comodo);
    publish(topic_publish, msg);
    list_components->atual++;
}

void menuUser(struct list_components *list_components) {
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
    int row = 2;
    int invertValues[2] = {1, 0}; 
    char mapResponse[2][10] = {"Ligar", "Desligar"};
    char controlType;
    mvprintw (row+MARGIN, 20+MARGIN, "Menu do Sistema");
    row += 5;
    mvprintw((row++)+MARGIN,2+MARGIN,"Escolha um dos dispositivos: ");
    int choice=10;
    int i;
    for(i=0; i<list_components->atual; i++){
      mvprintw ((row++)+MARGIN,2+MARGIN, "%d. %s %s(%s)", i, mapResponse[list_components->components[i].component_out_value],list_components->components[i].component_out, list_components->components[i].comodo);
    }
    i++;
    lampada_cozinha[1] = i;
    mvprintw ((row++)+MARGIN,2+MARGIN, "%d. %s lâmpada da cozinha", i++, mapResponse[lampada_cozinha[0]]);
    lampada_sala[1] = i;
    mvprintw ((row++)+MARGIN,2+MARGIN, "%d. %s lâmpada da sala", i, mapResponse[lampada_sala[0]]);
    row++;
    mvprintw((row++)+MARGIN,2+MARGIN,"Pressione CTRL + C para sair do programa");

    mvprintw ((row++)+MARGIN,2+MARGIN, "Digite a sua escolha: ");
    if( poll(&mypoll, 1, 0) ){
      scanw(" %d", &choice);
    }
    char topico[50];
    char msg[60];
    if (list_components->atual!= 0 && choice < list_components->atual) {
      sprintf(topico,"fse2020/130126721/dispositivos/%s", list_components->components[choice].mac);
      sprintf(msg,"{\"dispositivo_saida\":%d}", invertValues[list_components->components[choice].component_out_value]);
      publish(topico, msg);
    }
    else if(choice == list_components->atual+1) {
      lampada_cozinha[0] = inverte_estado(L_KITCHEN, lampada_cozinha[0]);
    }
    else if(choice == list_components->atual+2) {
      lampada_sala[0] = inverte_estado(L_ROOM, lampada_sala[0]);
    }
}

int inverte_estado(int gpio_pin, int value) {
  if(value == 0) {
    turn_on_component(gpio_pin);
    return 1;
  } else{
    turn_off_component(gpio_pin);
    return 0;
  }
}