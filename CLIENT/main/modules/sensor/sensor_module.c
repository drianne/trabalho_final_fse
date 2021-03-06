#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "dht11.h"
#include "../mqtt/mqtt.h"
#include "../memory/config.h"

static struct dht11_reading last_read;

void get_temp_humidity(){
    DHT11_init(GPIO_NUM_4);

    while(1) {
        last_read = DHT11_read();

        char umidade[50];
        char temperatura[50];
        char estado[50];       
        
        sprintf(temperatura, "{\"temperatura\": %.2lf}", (double)last_read.temperature);
        sprintf(umidade, "{\"umidade\": %.2lf}", (double)last_read.humidity);

        if(possui_valor() != NULL) {
            mqtt_envia_mensagem(get_temperature_topic(), temperatura);
            mqtt_envia_mensagem(get_umidade_topic(), umidade);
            mqtt_envia_mensagem(get_estado_topic(), estado);
        }
        else {
            printf("Não tem comodo!");
        }

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}