#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "nvs_flash.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>

#include "Wificonnection.h"
#include "main_functions.h"
#include "HiveMQBroker.h"

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
    mqtt_app_start();
    
    xTaskCreatePinnedToCore(vMainTask, "vMainTask", configMINIMAL_STACK_SIZE * 5, NULL, 10 | portPRIVILEGE_BIT, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vHeater, "vTemperature", configMINIMAL_STACK_SIZE * 5, NULL, 7 | portPRIVILEGE_BIT, NULL,tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vAerator, "vAerator", configMINIMAL_STACK_SIZE * 5, NULL,  6 | portPRIVILEGE_BIT, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vPeristalticPump, "vPeristalticPump", configMINIMAL_STACK_SIZE * 8, NULL, 3 | portPRIVILEGE_BIT, NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vFishFeed, "vFishFeed", configMINIMAL_STACK_SIZE * 5, NULL, 9 | portPRIVILEGE_BIT, NULL,tskNO_AFFINITY);
}