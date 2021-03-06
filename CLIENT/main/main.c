#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "freertos/semphr.h"

#include "./modules/wifi/wifi.h"
#include "./modules/http/http_client.h"
#include "./modules/mqtt/mqtt.h"
#include "./modules/sensor/sensor_module.h"
#include "./modules/memory/config.h"
#include "./modules/button/button.h"

xSemaphoreHandle conexaoWifiSemaphore;
xSemaphoreHandle conexaoMQTTSemaphore;

void conectadoWifi(void * params){
  while(true)  {
    if(xSemaphoreTake(conexaoWifiSemaphore, portMAX_DELAY))    {
      mqtt_start();
    }
  }
}

const char* get_mac_address(){
  uint8_t mac [6] = {0};
  static char macaddress [20];

  esp_efuse_mac_get_default(mac);
  sprintf(macaddress,"%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return macaddress;
}

void trataComunicacaoComServidor(void * params){
  if(xSemaphoreTake(conexaoMQTTSemaphore, portMAX_DELAY))  {
    while(true){
      char init_message [200];

      strcpy(init_message, "fse2020/130126721/dispositivos/");
      strcat(init_message, get_mac_address());
      printf("%s", init_message);
      mqtt_envia_mensagem(init_message, "novo");

      vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
  }
}

void app_main(void){    
    // Inicializa o NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    conexaoWifiSemaphore = xSemaphoreCreateBinary();
    conexaoMQTTSemaphore = xSemaphoreCreateBinary();

    botao_init();
    wifi_start();    

    xTaskCreate(&conectadoWifi,  "Conexão ao MQTT", 4096, NULL, 1, NULL);
    // xTaskCreate(&trataComunicacaoComServidor, "Comunicação com Broker", 4096, NULL, 1, NULL);
    xTaskCreate(&get_temp_humidity,  "Coleta umidade", 4096, NULL, 1, NULL);
}
