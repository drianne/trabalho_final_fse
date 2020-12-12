#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "dht11.h"

static struct dht11_reading last_read;

void get_temp_humidity(){
    DHT11_init(GPIO_NUM_4);

    while(1) {
        printf("Reading...\n");
        last_read = DHT11_read();

        if(last_read.temperature > -1){
          printf("Temperature is %d \n", last_read.temperature);
          printf("Humidity is %d\n", last_read.humidity);
          printf("Status code is %d\n", last_read.status);
        }else{
          printf(">> Error\n");
        }      
        vTaskDelay(1);
    }
}