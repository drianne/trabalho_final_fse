#include <ncurses.h>			/* ncurses.h includes stdio.h */  
#include <string.h> 
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include "central_server.h"
#include "temperature_module_i2c.h"
#include "mqtt.h"
#include "menu.h"

void dataInfo(struct list_components *list_components);
void menuBorders();
void menuUser(struct list_components *list_components);
void componentSelect(struct list_components *list_components);
void componentSelect(struct list_components *list_components);
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
  int row=2;
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Informações do sistema");
  row+=2;
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Temperatura do sistema: %lf", get_temperature());
  mvprintw ((row++)+MARGIN, MARGIN_INFO, "Humidade do sistema: %lf", get_humidity());
  row+=1
  for(int i=0; i<list_components->atual; i++){
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "Nome do cômodo: %s", list_components->components[i].comodo);
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "Temperatura: %.2lf", list_components->components[i].temp);
    mvprintw ((row++)+MARGIN, MARGIN_INFO, "Humidade: %.2lf", list_components->components[i].hum);
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
    row += 5;
    char comodo[40];
    mvprintw (row+MARGIN, 20+MARGIN, "Adicione um componente");
    row += 5;
    mvprintw((row++)+MARGIN,2+MARGIN,"Digite o nome do cômodo do componente");
    getstr(comodo);
    strcpy(list_components->components[list_components->atual].comodo, comodo);
    char topic_subscribe[50] = "fse2020/130126721/";
    strcat(topic_subscribe, comodo);
    strcat(topic_subscribe, "/#");
    subscribe(topic_subscribe);

    char topic_publish[50] = "fse2020/130126721/dispositivos/";
    strcat(topic_publish, list_components->components[list_components->atual].mac);
    publish(topic_publish, "sala");
    list_components->atual++;

    char dispositivo_entrada[40];
    mvprintw((row++)+MARGIN,2+MARGIN,"Digite o nome do dispositivo de entrada");
    getstr(dispositivo_entrada);
    strcpy(list_components->components[list_components->atual].component_in, dispositivo_entrada);

    char dispositivo_saida[40];
    mvprintw((row++)+MARGIN,2+MARGIN,"Digite o nome do dispositivo de saída");
    getstr(dispositivo_saida);
    strcpy(list_components->components[list_components->atual].component_out, dispositivo_saida);

    list_components->components[list_components->atual].component_in_value = 0;
    list_components->components[list_components->atual].component_out_value = 0;
}

void menuUser(struct list_components *list_components) {
    struct pollfd mypoll = { STDIN_FILENO, POLLIN|POLLPRI };
    int row = 2;
    int invertValues[2] = {1, 0}; 
    char mapResponse[2][10] = {"Ligar", "Desligar"};
    char choice[10] = {'$'};
    char controlType;
    mvprintw (row+MARGIN, 20+MARGIN, "Menu do Sistema");
    row += 5;
    mvprintw((row++)+MARGIN,2+MARGIN,"a. Definir temperatura");
    mvprintw((row++)+MARGIN,2+MARGIN,"b. Controlar Ar condicionado");
    mvprintw((row++)+MARGIN,2+MARGIN,"c. Controlar lâmpada");
    mvprintw((row++)+MARGIN,2+MARGIN,"d. Desativar controle de temperatura");
    mvprintw((row++)+MARGIN,2+MARGIN,"Pressione CTRL + C para sair do programa");
    row++;
    mvprintw((row++)+MARGIN,2+MARGIN,"Digite a sua escolha:  ");
    row++;

    if( poll(&mypoll, 1, 0) ){
      getstr(choice);
    }

    if(!strcmp(choice, "a")) {
      mvprintw((row++)+MARGIN,2+MARGIN,"Digite a temperatura desejada: ");
      // send_data(TEMPERATURE_CONTROL, integer, decimal);
    }
    else if(!strcmp(choice, "b")) {
      row++;
      int conditionalAir;
      mvprintw((row++)+MARGIN,2+MARGIN,"Digite o número do ar condicionado: ");
      scanw(" %d", &conditionalAir);
      // mvprintw((row++)+MARGIN,2+MARGIN,"value escolhido = %d, value atual = %d", conditionalAir, list_components->devices->airConditioning1);
      if (conditionalAir == 1) {
        // send_data(AIR_BEDROOM_1, invertValues[list_components->devices->airConditioning1], 0);
      }
      else if(conditionalAir == 2) {
        // send_data(AIR_BEDROOM_2, invertValues[list_components->devices->airConditioning2], 0);
      }
      else {
        mvprintw((row++)+MARGIN,2+MARGIN,"Ativação cancelada");
      }
    }
    else if(!strcmp(choice, "c")) {
      row++;
      int lamp;
      mvprintw((row++)+MARGIN,2+MARGIN,"Digite o número da lâmpada: ");
      scanw(" %d", &lamp);
      if (lamp == 1) {
        // send_data(L_KITCHEN, invertValues[list_components->devices->lampKitchen], 0);
      }
      else if(lamp == 2) {
        // send_data(L_ROOM, invertValues[list_components->devices->lampRoom], 0);
      }
      else if(lamp == 3) {
        // send_data(L_BEDROOM_1, invertValues[list_components->devices->lampBedroom1], 0);
      }
      else if(lamp == 4) {
        // send_data(L_BEDROOM_2, invertValues[list_components->devices->lampBedroom2], 0);
      }
      else {
        mvprintw((row++)+MARGIN,2+MARGIN,"Ativação cancelada");
      }
    }
    else if(!strcmp(choice, "d")) {
      // send_data(TEMPERATURE_CONTROL, -1, 0);
    }
    else if(!strcmp(choice, "$")) {
      mvprintw((row++)+MARGIN,2+MARGIN,"");
    }
    else {
      mvprintw((row++)+MARGIN,2+MARGIN,"Escolha inválida");
    }
}
