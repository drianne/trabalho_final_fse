#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "driver/gpio.h"

#include "../led/led.h"

#define LED 2
int led_value;

void start_led(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void change_led(int value){
    gpio_set_level(LED, value);
    led_value = value;
}

void flash_light(){
    change_led(1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    change_led(0);
}

int get_led_value() {
    return led_value;
}