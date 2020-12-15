#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "dht11.h"

static struct dht11_reading last_read;

void get_temp_humidity(){
    DHT11_init(GPIO_NUM_4);

    while(1) {
        last_read = DHT11_read();

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}