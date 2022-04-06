#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nvs_flash.h"

#include "Wificonnection.h"
#include "main_functions.h"

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    wifi_init();
    obtain_time();
    
    xTaskCreate(vMainTask, "vMainTask", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
    xTaskCreate(vHeater, "vTemperature", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
    xTaskCreate(vAerator, "vAerator", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
    xTaskCreate(vFishFeed, "vFishFeed", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
}