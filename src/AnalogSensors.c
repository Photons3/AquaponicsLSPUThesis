#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include <esp_log.h>

#include <ds18x20.h>


// // Set up the temperature sensor pins and adress -------------
#if defined(CONFIG_IDF_TARGET_ESP8266)
    static const gpio_num_t tempSensorPin = 4;
#else
    static const gpio_num_t tempSensorPin = GPIO_NUM_4;
#endif

static const ds18x20_addr_t SENSOR_ADDR = 0x0701215cb8cf4128;
// ----------------------------------------------------------- 

//ADC Calibration
#if CONFIG_IDF_TARGET_ESP32
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_VREF
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32C3
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP
#elif CONFIG_IDF_TARGET_ESP32S3
#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE_TP_FIT
#endif

// Define VoltageDividerMultiplier Value
static const float VoltageDividerMultipler = 1.00F; // 120K+220K/220K

//ADC Channels
#define DOSensorPin          ADC1_CHANNEL_6      /*!< ADC1 channel 6 is GPIO34 */
#define pHSensorPin          ADC1_CHANNEL_7      /*!< ADC1 channel 7 is GPIO35 */
//#define tempSensorPin        ADC1_CHANNEL_3      /*!< ADC1 channel 3 is GPIO39 */

//static const char *TAG_CH[2][10] = {{"ADC1_CH3"},{"ADC1_CH2"}};

static int adc_raw[2];
static const char *TAG = "ADC SINGLE";

static esp_adc_cal_characteristics_t adc1_chars;

// Run this function first before doing anything about analog sensors
void adc_calibration_init(void)
{
    esp_err_t ret;

    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }
}

#define OFFSET_PH_SENSOR 0.25

// Reads the PH sensors and returns the value as float
float read_PH_sensorValue()
{
    uint32_t voltage = 0;
    float pHValue;

    adc_raw[0] = adc1_get_raw(pHSensorPin);
    //ESP_LOGI(TAG_CH[0][0], "raw  data: %d", adc_raw[0][0]);

    // This formula comes from wiki of PH sensor
    voltage = VoltageDividerMultipler * esp_adc_cal_raw_to_voltage(adc_raw[0], &adc1_chars);
    pHValue = 0.0035 * voltage + OFFSET_PH_SENSOR;
    //ESP_LOGI(TAG_CH[0][0], "PH Value: %f", pHValue);

    if ( (pHValue < 2) || (pHValue > 12) )
    {
        return 7.00f;
    }

    return pHValue;
}

float read_Temp_sensorValue()
{
    
    float temperature;
    esp_err_t res;

    res = ds18x20_measure_and_read(tempSensorPin, SENSOR_ADDR, &temperature);
    if (res != ESP_OK)
    {
        temperature = 25;
        // ESP_LOGE(TAG, "Could not read from sensor %08x%08x: %d (%s)",
        // (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, res, esp_err_to_name(res));
    }

    if ( temperature < 0 || temperature > 40 )
    {
        temperature = 25.00f;
    }
    // else
    //     ESP_LOGI(TAG, "Sensor %08x%08x: %.2f°C",
    //             (uint32_t)(SENSOR_ADDR >> 32), (uint32_t)SENSOR_ADDR, temperature);

    return temperature;
}

#define CAL1_V (1600) //mv
#define CAL1_T (25)   //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (1300) //mv
#define CAL2_T (15)   //℃

static const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410}; 

float read_DO_sensorValue(uint32_t temperature_c)
{
    uint32_t voltage = 0;
    float dissolvedOxygenValue;

    adc_raw[1] = adc1_get_raw(DOSensorPin);
    //ESP_LOGI(TAG_CH[0][0], "raw  data: %d", adc_raw[0][0]);

    // This formula comes from wiki of DO sensor
    voltage = VoltageDividerMultipler * esp_adc_cal_raw_to_voltage(adc_raw[1], &adc1_chars);

    uint16_t V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
    
    dissolvedOxygenValue = (voltage * DO_Table[temperature_c] / V_saturation);

    if ((dissolvedOxygenValue < 0))
    {
        return 0.00f;
    }

    return dissolvedOxygenValue;
}


void read_analog_sensorValues(void *pvParameters)
{
    adc_calibration_init();

    while (1)
    {
        printf("temperature: %f ", read_Temp_sensorValue());

        printf("PH: %f ", read_PH_sensorValue());

        vTaskDelay(pdMS_TO_TICKS(5000));
    }

}