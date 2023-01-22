#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include "esp_log.h"

#define APositivo 23 //IN1
#define ANegativo 22 //IN2
#define BPositivo 19 //IN3
#define BNegativo 18 //IN4
static const char *TAG = "motor";
//OUT1=AZUL OUT2=ROJO OUT3=VERDE OUT4=NEGRO

void startMotor(){
    gpio_set_direction(APositivo, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(ANegativo, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(BPositivo, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(BNegativo, GPIO_MODE_DEF_OUTPUT);
    
    gpio_set_level(APositivo,0);
    gpio_set_level(ANegativo,0);
    gpio_set_level(BPositivo,0);
    gpio_set_level(BNegativo,0);
}

void movimientoAntihorario(int ciclos){
    int i=0;
    do{
        gpio_set_level(APositivo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(APositivo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BPositivo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BPositivo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(ANegativo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(ANegativo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BNegativo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BNegativo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        i++;
        ESP_LOGI(TAG, "1 ciclo");
    }
    while(i<ciclos);
}

void movimientoHorario(int ciclos){
    int i=0;
    do{
        gpio_set_level(APositivo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(APositivo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BNegativo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BNegativo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(ANegativo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(ANegativo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BPositivo,1);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(BPositivo,0);
        vTaskDelay(pdMS_TO_TICKS(10));
        i++;
        ESP_LOGI(TAG, "1 ciclo");
    }
    while(i<ciclos);
}

void app_main(void){
    startMotor();
    //movimientoAntihorario(200);
    movimientoHorario(400);
}