#ifndef __ULTRASONIC_SENSOR__
#define __ULTRASONIC_SENSOR__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ultrasonic.h>
#include <esp_err.h>

//void ultrasonic_test(void *pvParameters);

uint32_t get_water_height();

#ifdef __cplusplus
}
#endif

#endif