#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define PLANTA0 25 //PIN_G25
#define PLANTA1 ADC1_CHANNEL_4 //PIN_G32
#define PLANTA2 ADC1_CHANNEL_5 //PIN_G33
#define PLANTA3 26 //PIN_G26

#define INFRARROJO 22 //PIN_G22
#define UMBRAL 2500

static const char *INFO = "INFO";
static const char *TAG0 = "VALOR P0";
static const char *TAG1 = "VALOR P1";
static const char *TAG2 = "VALOR P2";
static const char *TAG3 = "VALOR P3";

static uint16_t valorPLANTA0=0;
static uint16_t valorPLANTA1=0;
static uint16_t valorPLANTA2=0;
static uint16_t valorPLANTA3=0;

void burstInfrarrojo(void){
   
    gpio_set_level(INFRARROJO,1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(INFRARROJO,0);
}

void lectorInfrarrojo(uint16_t valorPLANTA0, uint16_t valorPLANTA1, uint16_t valorPLANTA2, uint16_t valorPLANTA3){

    if (gpio_get_level(PLANTA0) == 1)
        valorPLANTA0 = 4000;
    ESP_LOGI(TAG0, "%d", valorPLANTA0);

    valorPLANTA1 = adc1_get_raw(PLANTA1);
    ESP_LOGI(TAG1, "%d", valorPLANTA1);

    valorPLANTA2 = adc1_get_raw(PLANTA2);
    ESP_LOGI(TAG2, "%d", valorPLANTA2);

    if (gpio_get_level(PLANTA3) == 1)
        valorPLANTA3 = 4000;
    ESP_LOGI(TAG3, "%d", valorPLANTA3);

    vTaskDelay(pdMS_TO_TICKS(1000));
    printf("\n\n\n\n");
}

void lecturaSensores(){
    if (gpio_get_level(PLANTA0) == 1)
        valorPLANTA0 = 1;
    else valorPLANTA0 = 0;
    valorPLANTA1 = adc1_get_raw(PLANTA1);

    valorPLANTA2 = adc1_get_raw(PLANTA2);

    if (gpio_get_level(PLANTA3) == 1)
        valorPLANTA3 = 1;
    else valorPLANTA3 = 0;
    vTaskDelay(10);
    
}

void detenerAscensorEnPlantaX(int X, uint16_t valorPLANTA0, uint16_t valorPLANTA1, uint16_t valorPLANTA2, uint16_t valorPLANTA3){// X sera un disparador que vendra de telegram con el numero que ha pulsado el usuario
    
    lecturaSensores();

    if ((valorPLANTA0 == 1) && X == 0)
    {
        ESP_LOGI(INFO, "Detenido en PLANTA 0 1000ms %d",valorPLANTA0);//aqui parariamos el motor
        vTaskDelay(pdMS_TO_TICKS(1000));
        
    }
    else
    {
        if ((valorPLANTA1 > UMBRAL) && X == 1)
        {
            ESP_LOGI(INFO, "Detenido en PLANTA 1 1000ms");//y aqui
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        else
        {
            if ((valorPLANTA2 > UMBRAL) && X == 2)
            {
                ESP_LOGI(INFO, "Detenido en PLANTA 2 1000ms");//y aqui
                vTaskDelay(pdMS_TO_TICKS(1000));
            }
            else
            {
                if ((valorPLANTA3 == 1) && X == 3)
                {
                    valorPLANTA3=0;
                    ESP_LOGI(INFO, "Detenido en PLANTA 3 1000ms");//y aqui
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    
                }
            }
        }
    }
}
void app_main(void)
{
    gpio_set_direction(INFRARROJO, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(PLANTA0, GPIO_MODE_DEF_INPUT);
    gpio_set_direction(PLANTA3, GPIO_MODE_DEF_INPUT);


    


    printf("Valores ascensor\n");
    printf("---------------------------\n");
    
    while(1){
        burstInfrarrojo();
        //lectorInfrarrojo(valorPLANTA0, valorPLANTA1, valorPLANTA2, valorPLANTA3); 
        detenerAscensorEnPlantaX(0,valorPLANTA0,valorPLANTA1,valorPLANTA2,valorPLANTA3);
    }
}
