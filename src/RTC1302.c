#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ds1302.h>

#include "RTC1302.h"

#if defined(CONFIG_IDF_TARGET_ESP8266)
#define CE_GPIO 5
#define IO_GPIO 4
#define SCLK_GPIO 0
#else
#define CE_GPIO 16
#define IO_GPIO 17
#define SCLK_GPIO 18
#endif

static ds1302_t RTCdev = {
        .ce_pin = CE_GPIO,
        .io_pin = IO_GPIO,
        .sclk_pin = SCLK_GPIO
    };

void RTC_init(void)
{
    ESP_ERROR_CHECK(ds1302_init(&RTCdev));
}

void ds1302_sync_time_with_ntp(struct tm *time)
{
    ESP_ERROR_CHECK(ds1302_init(&RTCdev));
    ESP_ERROR_CHECK(ds1302_set_write_protect(&RTCdev, false));

    ESP_ERROR_CHECK(ds1302_set_time(&RTCdev, time));
    ESP_ERROR_CHECK(ds1302_start(&RTCdev, true));
    ESP_ERROR_CHECK(ds1302_set_write_protect(&RTCdev, true));
}

void get_time_from_RTC(struct tm *time)
{
    ds1302_get_time(&RTCdev, time);
}