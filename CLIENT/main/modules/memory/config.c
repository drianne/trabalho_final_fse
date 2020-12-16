#include "esp_log.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include <stdio.h>
#include <string.h>

const char* le_valor_nvs(){
    // Inicia o acesso à partição padrão nvs
    ESP_ERROR_CHECK(nvs_flash_init());

    size_t required_size;
    const char* valor;

    nvs_handle particao_padrao_handle;

    // Abre o acesso à partição nvs
    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);
    
    if(res_nvs == ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    }else{
        nvs_get_str(particao_padrao_handle, "comodo", NULL, &required_size);
    
        const char* valor = malloc(required_size);    
        
        // Abre o acesso à partição nvs
        esp_err_t res_nvs = nvs_open("armazenamento", NVS_READONLY, &particao_padrao_handle);

        if(res_nvs == ESP_ERR_NVS_NOT_FOUND){
            ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
        }else{   
            esp_err_t res = nvs_get_str(particao_padrao_handle, "comodo", valor, &required_size);

            switch (res)
            {
            case ESP_OK:
                printf("Valor armazenado: %s\n", valor);
                break;
            case ESP_ERR_NOT_FOUND:
                ESP_LOGE("NVS", "Valor não encontrado");
                return NULL;
            default:
                ESP_LOGE("NVS", "Erro ao acessar o NVS (%s)", esp_err_to_name(res));
                return NULL;
                break;
            }
            nvs_close(particao_padrao_handle);
        }
    }

    return valor;
}

const char* possui_valor(){
    const char* valor_lido = le_valor_nvs();

    if(valor_lido != NULL){
      return valor_lido;
    }else{
      return NULL;
    }

}

void grava_valor_nvs(char* valor){
    ESP_ERROR_CHECK(nvs_flash_init());

    nvs_handle particao_padrao_handle;

    esp_err_t res_nvs = nvs_open("armazenamento", NVS_READWRITE, &particao_padrao_handle);
    
    if(res_nvs == ESP_ERR_NVS_NOT_FOUND){
        ESP_LOGE("NVS", "Namespace: armazenamento, não encontrado");
    }

    esp_err_t res = nvs_set_str (particao_padrao_handle, "comodo", valor);

    if(res != ESP_OK){
        ESP_LOGE("NVS", "Não foi possível escrever no NVS (%s)", esp_err_to_name(res));
    }
    nvs_commit(particao_padrao_handle);
    nvs_close(particao_padrao_handle);

    printf("\n\nValor foi armazenado!\n\n");
}

