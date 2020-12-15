#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "driver/gpio.h"

#include "../led/led.h"

#define LED 2

void start_led(){
    gpio_pad_select_gpio(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
}

void off_led(){
    gpio_set_level(LED, 0);
}

void on_led(){
    gpio_set_level(LED, 1);
}

void flash_light(){
    on_led();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    off_led();
}