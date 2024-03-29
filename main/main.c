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
    
    wifi_init_sta();
    obtain_time();
    mqtt_app_start();
    
    xTaskCreatePinnedToCore(vMainTask, "vMainTask", configMINIMAL_STACK_SIZE * 5, NULL, 12 , NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vPeristalticPump, "vPeristalticPump", configMINIMAL_STACK_SIZE * 8, NULL, 11 , NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vFishFeed, "vFishFeed", configMINIMAL_STACK_SIZE * 5, NULL, 10 , NULL,tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vHeater, "vHeater", configMINIMAL_STACK_SIZE * 5, NULL, 9 , NULL,tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vAerator, "vAerator", configMINIMAL_STACK_SIZE * 5, NULL,  8 , NULL, tskNO_AFFINITY);
    xTaskCreatePinnedToCore(vSubmersiblePump, "vSubmersiblePump", configMINIMAL_STACK_SIZE * 5, NULL, 7 , NULL,tskNO_AFFINITY);
}