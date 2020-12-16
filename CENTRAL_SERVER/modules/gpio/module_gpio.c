#include <stdio.h>
#include <stdlib.h>
#include <bcm2835.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/mman.h>
#include "central_server.h"
#include "alarm.h"
#include "module_gpio.h"

void *polling(void *params);
void turn_on_component(int index);
void turn_off_component(int index);
void config_pins();

RPiGPIOPin sensores[QNT_SENSORS] = {PIN_SP_ROOM, PIN_SP_KITCHEN, PIN_SA_DOOR_KITCHEN, PIN_SA_WINDOW_KITCHEN, PIN_SA_DOOR_ROOM, PIN_SA_WINDOW_ROOM};
RPiGPIOPin outputs[QNTD_OUTPUTS] = {PIN_L_KITCHEN, PIN_L_ROOM};
pthread_t tTempControl;

void initGpio(){
    if (!bcm2835_init())
        printf("ERROR");
    config_pins();
}

void *polling(void *params) {
    int *index = (int *)params;
    RPiGPIOPin sensor = sensores[*index];
    bcm2835_gpio_fsel(sensor, BCM2835_GPIO_FSEL_INPT);

    int i;
    while (1) {
        sleep(1);
        while (1 == bcm2835_gpio_lev(sensor)){
            sleep(1);
        };
        turn_down_alarm();
        while (0 == bcm2835_gpio_lev(sensor)){
            sleep(1);
        };
        for (i = 0; i < 5000; i++) {
            if (0 == bcm2835_gpio_lev(sensor)) 
                break;
        }
        turn_on_alarm();
        fflush(stdout);
        usleep(500);
    }

    return (EXIT_SUCCESS);
}

void config_pins(){
    bcm2835_gpio_fsel(PIN_L_KITCHEN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(PIN_L_ROOM, BCM2835_GPIO_FSEL_OUTP);
}

void initialize_components() {
    bcm2835_gpio_write(PIN_SP_ROOM, OFF);
    bcm2835_gpio_write(PIN_SP_KITCHEN, OFF);
    bcm2835_gpio_write(PIN_SA_DOOR_KITCHEN, OFF);
    bcm2835_gpio_write(PIN_SA_WINDOW_KITCHEN, OFF);
    bcm2835_gpio_write(PIN_SA_WINDOW_KITCHEN, OFF);
    bcm2835_gpio_write(PIN_SA_DOOR_ROOM, OFF);
    bcm2835_gpio_write(PIN_SA_WINDOW_ROOM, OFF);
}

void turn_off_component(int index) {
    bcm2835_gpio_write(outputs[index], OFF);
}

void turn_on_component(int index) {
    bcm2835_gpio_write(outputs[index], ON);
}