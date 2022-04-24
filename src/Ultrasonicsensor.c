#include <Ultrasonicsensor.h>

#define MAX_DISTANCE_CM 100 // 1m max
#define ULTRA_SONIC_SENSOR_HEIGHT 70 // 70 cm

#if defined(CONFIG_IDF_TARGET_ESP8266)
#define TRIGGER_GPIO 4
#define ECHO_GPIO 5
#else
#define TRIGGER_GPIO 26
#define ECHO_GPIO 25
#endif

void ultrasonic_test(void *pvParameters)
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };

    ultrasonic_init(&sensor);

    while (true)
    {
        uint32_t distance;
        esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance);
        if (res != ESP_OK)
        {
            printf("Error %d: ", res);
            switch (res)
            {
                case ESP_ERR_ULTRASONIC_PING:
                    printf("Cannot ping (device is in invalid state)\n");
                    break;
                case ESP_ERR_ULTRASONIC_PING_TIMEOUT:
                    printf("Ping timeout (no device found)\n");
                    break;
                case ESP_ERR_ULTRASONIC_ECHO_TIMEOUT:
                    printf("Echo timeout (i.e. distance too big)\n");
                    break;
                default:
                    printf("%s\n", esp_err_to_name(res));
            }
        }
        else
            printf("Distance: %d cm\n", distance);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

uint32_t get_water_height()
{
    ultrasonic_sensor_t sensor = {
        .trigger_pin = TRIGGER_GPIO,
        .echo_pin = ECHO_GPIO
    };

    ultrasonic_init(&sensor);

    uint32_t distance;

    esp_err_t res = ultrasonic_measure_cm(&sensor, MAX_DISTANCE_CM, &distance);

    uint32_t height = ULTRA_SONIC_SENSOR_HEIGHT - distance;

    if ( res != ESP_OK )
    {
        return 60;
    }

    if ( height == 0 || height > ULTRA_SONIC_SENSOR_HEIGHT )
    {
        return 60;
    }
        
    return height;
}