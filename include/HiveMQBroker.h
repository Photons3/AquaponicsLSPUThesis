#ifndef _HIVEMQ_BROKER_H_
#define _HIVEMQ_BROKER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <time.h>
#include "lwip/apps/sntp.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>
#include "esp_event.h"
#include "esp_netif.h"

extern void mqtt_app_start(void);

//extern void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

extern esp_mqtt_client_handle_t client;

#ifdef __cplusplus
}
#endif

#endif