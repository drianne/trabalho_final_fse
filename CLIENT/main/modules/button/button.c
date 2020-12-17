#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "../mqtt/mqtt.h"
#include "../led/led.h"

#define BOTAO 0

int valor_botao;
xQueueHandle filaDeInterrupcao;

static void IRAM_ATTR gpio_isr_handler(void *args)
{
  int pino = (int)args;
  xQueueSendFromISR(filaDeInterrupcao, &pino, NULL);
}

void trataInterrupcaoBotao(void *params)
{
  int pino;

  while(true)
  {
    if(xQueueReceive(filaDeInterrupcao, &pino, portMAX_DELAY))
    {
      // De-bouncing
      int estado;
      char msg[70];
      estado = gpio_get_level(pino);
      if(estado == 1)
      {
        valor_botao = 0;
        sprintf(msg, "{\"dispositivo_entrada\": 0, \"dispositivo_saida\": %d}", get_led_value()); 
        mqtt_envia_mensagem(get_estado_topic(), msg);
        gpio_isr_handler_remove(pino);
        while(gpio_get_level(pino) == estado)
        {
          vTaskDelay(50 / portTICK_PERIOD_MS);
        }
        // Habilitar novamente a interrupção
        vTaskDelay(50 / portTICK_PERIOD_MS);
        sprintf(msg, "{\"dispositivo_entrada\": %d, \"dispositivo_saida\": %d}", estado, get_led_value()); 
        mqtt_envia_mensagem(get_estado_topic(), msg);
        valor_botao = 1;
        gpio_isr_handler_add(pino, gpio_isr_handler, (void *) pino);
      }

    }
  }
}

void botao_init()
{
  // Configuração do pino do Botão
  gpio_pad_select_gpio(BOTAO);
  // Configura o pino do Botão como Entrada
  gpio_set_direction(BOTAO, GPIO_MODE_INPUT);
  // Configura o resistor de Pulldown para o botão (por padrão a entrada estará em Zero)
  gpio_pulldown_en(BOTAO);
  // Desabilita o resistor de Pull-up por segurança.
  gpio_pullup_dis(BOTAO);

  // Configura pino para interrupção
  gpio_set_intr_type(BOTAO, GPIO_INTR_POSEDGE);

  filaDeInterrupcao = xQueueCreate(10, sizeof(int));
  xTaskCreate(trataInterrupcaoBotao, "TrataBotao", 2048, NULL, 1, NULL);

  gpio_install_isr_service(0);
  gpio_isr_handler_add(BOTAO, gpio_isr_handler, (void *) BOTAO);
}

int get_valor_botao(){
    return valor_botao;
}