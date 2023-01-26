#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"
#include "esp_attr.h"
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"
#include <esp_https_server.h>
#include "esp_tls.h"

#define PLANTA0 25 //PIN_G25
#define PLANTA1 ADC1_CHANNEL_4 //PIN_G32
#define PLANTA2 ADC1_CHANNEL_5 //PIN_G33
#define PLANTA3 26 //PIN_G26

#define INFRARROJO 27 //PIN_G27
#define UMBRAL 2500

static const char *INFO = "INFO";
static const char *TAG0 = "VALOR P0";
static const char *TAG1 = "VALOR P1";
static const char *TAG2 = "VALOR P2";
static const char *TAG3 = "VALOR P3";

#define APositivo 23 //IN1
#define ANegativo 22 //IN2
#define BPositivo 19 //IN3
#define BNegativo 18 //IN4
static const char *TAG = "motor";
//OUT1=AZUL OUT2=ROJO OUT3=VERDE OUT4=NEGRO

static uint16_t valorPLANTA0=0;
static uint16_t valorPLANTA1=0;
static uint16_t valorPLANTA2=0;
static uint16_t valorPLANTA3=0;

static int objetivo=4;

void burstInfrarrojo(void){
   
    gpio_set_level(INFRARROJO,1);
    vTaskDelay(pdMS_TO_TICKS(10));
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

    vTaskDelay(pdMS_TO_TICKS(100));
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

void movimientoAbajo(){
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
        ESP_LOGI(TAG, "1 ciclo");
}

void movimientoArriba(){
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
        ESP_LOGI(TAG, "1 ciclo");
}

void pararMotor(){
        gpio_set_level(APositivo,0);
        gpio_set_level(BNegativo,0);
        gpio_set_level(ANegativo,0);
        gpio_set_level(BPositivo,0);
}

int irAPlanta0(){
    while(gpio_get_level(PLANTA0)==0) {
        movimientoAbajo();
        lectorInfrarrojo(valorPLANTA0, valorPLANTA1, valorPLANTA2, valorPLANTA3);
    }
    return 0;
}

int irAPlanta1(int plantaActual){
     
        while ( adc1_get_raw(PLANTA1)<UMBRAL)
        {
            if(plantaActual>1){
            movimientoAbajo();
            lectorInfrarrojo(valorPLANTA0, valorPLANTA1, valorPLANTA2, valorPLANTA3);
            }else{
            movimientoArriba();
            lectorInfrarrojo(valorPLANTA0, valorPLANTA1, valorPLANTA2, valorPLANTA3);
            }
        }
    return 1;
}

int irAPlanta2(int plantaActual){
     
        while ( adc1_get_raw(PLANTA2)<UMBRAL)
        {
            if(plantaActual>2){
            movimientoAbajo();
            lectorInfrarrojo(valorPLANTA0, valorPLANTA1, valorPLANTA2, valorPLANTA3);
            }else{
            movimientoArriba();
            lectorInfrarrojo(valorPLANTA0, valorPLANTA1, valorPLANTA2, valorPLANTA3);
            }
        }
    return 2;
}

int irAPlanta3(){
    while(gpio_get_level(PLANTA3)==0) {
        movimientoArriba();
        lectorInfrarrojo(valorPLANTA0, valorPLANTA1, valorPLANTA2, valorPLANTA3);
    }
    return 3;
}


//------------------------------------------
static esp_err_t root_get_handler(httpd_req_t *req)
{
    extern unsigned char view_start[] asm("_binary_view_html_start");
    extern unsigned char view_end[] asm("_binary_view_html_end");
    size_t view_len = view_end - view_start;
    char viewHtml[view_len];
    memcpy(viewHtml, view_start, view_len);
    ESP_LOGI(TAG, "URI: %s", req->uri);

    

    if (strcmp(req->uri, "/?planta0") == 0)
    {
        objetivo=0;
    }
     if (strcmp(req->uri, "/?planta1") == 0)
    {
        objetivo=1;
    }
     if (strcmp(req->uri, "/?planta2") == 0)
    {
        objetivo=2;
    }
     if (strcmp(req->uri, "/?planta3") == 0)
    {
        objetivo=3;
    }

    char *viewHtmlUpdated;
    int formattedStrResult = asprintf(&viewHtmlUpdated, viewHtml);

    httpd_resp_set_type(req, "text/html");

    if (formattedStrResult > 0)
    {
        httpd_resp_send(req, viewHtmlUpdated, view_len);
        free(viewHtmlUpdated);
    }
    else
    {
        ESP_LOGE(TAG, "Error updating variables");
        httpd_resp_send(req, viewHtml, view_len);
    }

    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();
    conf.transport_mode = HTTPD_SSL_TRANSPORT_INSECURE;
    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret)
    {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    return server;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_ssl_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server)
    {
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;
    if (*server == NULL)
    {
        *server = start_webserver();
    }
}
//------------------------------------------

void app_main(void)
{
    gpio_set_direction(INFRARROJO, GPIO_MODE_DEF_OUTPUT);
    gpio_set_direction(PLANTA0, GPIO_MODE_DEF_INPUT);
    gpio_set_direction(PLANTA3, GPIO_MODE_DEF_INPUT);

    //--------------------------------------
    static httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
    ESP_ERROR_CHECK(example_connect());
    //--------------------------------------
    int plantaActual=0;
    startMotor();
    printf("Valores ascensor\n");
    printf("---------------------------\n");
    while(1){
        switch(objetivo){
            case 0:
                plantaActual = irAPlanta0();
            break;
            case 1:
                plantaActual = irAPlanta1(plantaActual);
            break;
            case 2:
                plantaActual = irAPlanta2(plantaActual);
            break;
            case 3:
                plantaActual = irAPlanta3();
            break;
            default:
            break;
        }
    }
  
    printf("\nFIN"); 
    
}
