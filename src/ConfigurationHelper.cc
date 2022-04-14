#include "AquaponicsStructs.h"
#include "main_functions.h"
#include "ConfigurationHelper.h"
#include "ConfigurationHelperCxx.h"
#include <esp_log.h>

#include "nvs_flash.h"
#include "nvs.h"
#include "nvs_handle.hpp"
#include "esp_system.h"

static const char* TAG = "CONFIG";

ConfigurationValues configValues;

void initConfigurationValues(DelayValues* delay)
{

    // Open
    ESP_LOGI(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    esp_err_t result;
    // Handle will automatically close when going out of scope or when it's reset.
    std::shared_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle("storage", NVS_READWRITE, &result);
    
    // Read
    ESP_LOGI(TAG, "Reading contents from NVS ... ");
    // -------------------------- DEFAULT VALUES ----------------------------------- //
    int32_t tempLow = (int32_t) 17;                   
    int32_t tempHigh = (int32_t) 33;
    int32_t doLow = (int32_t) 10;
    int32_t doHigh = (int32_t) 40;
    int32_t phLow = (int32_t) 5;
    int32_t phHigh = (int32_t) 8;
    uint8_t fishFreq = (uint8_t) 3;

    handle->get_item("tempLow", tempLow);
    handle->get_item("tempHigh", tempHigh);
    handle->get_item("doLow", doLow);
    handle->get_item("doHigh", doHigh);
    handle->get_item("phLow", phLow);
    handle->get_item("phHigh", phHigh);
    handle->get_item("fishFreq", fishFreq);

    ESP_LOGI(TAG, "Config Values\n");
    ESP_LOGI(TAG, "TEMP: %d - %d \n", tempLow, tempHigh);
    ESP_LOGI(TAG, "DO: %d - %d \n", doLow, doHigh);
    ESP_LOGI(TAG, "PH: %d - %d \n", phLow, phHigh);

    configValues.tempLow = (float) tempLow;
    configValues.tempHigh = (float) tempHigh;
    configValues.doLow = (float) doLow;
    configValues.doHigh = (float) doHigh;
    configValues.phLow = (float) phLow;
    configValues.phHigh = (float) phHigh;
    configValues.fishFreq = (uint8_t) fishFreq;

    // START OF DELAY INITIALIZATION
    uint32_t submersiblePumpDelay = 5 * 60 * 1000;

    handle->get_item("submersiblePumpDelay", submersiblePumpDelay);

    delay->heater_delay = 0;
    delay->peristalticPump_delay = 0;
    delay->aerator_delay = 5 * 60 * 1000;

    delay->fishfeed_delay = 10 * 1000;
    delay->submersiblePump_delay = submersiblePumpDelay;
}

void setDelayValues(ConfigurationValues* val, DelayValues* delay, ForecastedValue* forecast)
{   
    {
    // Temperature
    float x_1, x_2, x_3, y_1, y_2, y_3, x;
    x = forecast->temperature;
    x_1 = val->tempLow - (val->tempLow)/4.5;
    x_2 = val->tempLow + (val->tempHigh - val->tempLow)/5;
    x_3 = val->tempLow + (val->tempHigh - val->tempLow)/2;

    float highDelay = 20 * 1000;

    y_1 = highDelay;
    y_2 = highDelay / 2;
    y_3 = 0;

    uint32_t y = (uint32_t)(y_1*((x-x_2)*(x-x_3))/((x_1-x_2)*(x_1-x_3))+y_2*((x-x_1)*(x-x_3))/((x_2-x_1)*(x_2-x_3))+y_3*((x-x_1)*(x-x_2))/((x_3-x_1)*(x_3-x_2)));
    delay->heater_delay = y;
    ESP_LOGI(TAG, "Heater Delay: %d s", delay->heater_delay);
    }

    {
    // DO
    float x_1, x_2, x_3, y_1, y_2, y_3, x;
    x = forecast->DO;
    x_1 = val->doLow;
    x_2 = val->doLow + (val->doHigh - val->doHigh)*(2/5);
    x_3 = val->doLow + (val->doHigh - val->doLow)*(3/4);

    float highDelay = 540 * 1000;

    y_1 = highDelay;
    y_2 = highDelay / 2;
    y_3 = 0;

    uint32_t y = (uint32_t)(y_1*((x-x_2)*(x-x_3))/((x_1-x_2)*(x_1-x_3))+y_2*((x-x_1)*(x-x_3))/((x_2-x_1)*(x_2-x_3))+y_3*((x-x_1)*(x-x_2))/((x_3-x_1)*(x_3-x_2)));
    delay->aerator_delay = y;
    ESP_LOGI(TAG, "Aerator Delay: %ds", delay->aerator_delay);
    }
    
    {
    // PH
    float x_1, x_2, x_3, y_1, y_2, y_3, x;
    x = forecast->PH;
    x_1 = val->phLow;
    x_2 = val->phLow + (val->phHigh - val->phLow)*(1/8);
    x_3 = val->phLow + (val->phHigh - val->phHigh)*(1/4);

    float highDelay = 20 * 1000;

    y_1 = highDelay;
    y_2 = highDelay / 2;
    y_3 = 0;

    uint32_t y = (uint32_t)(y_1*((x-x_2)*(x-x_3))/((x_1-x_2)*(x_1-x_3))+y_2*((x-x_1)*(x-x_3))/((x_2-x_1)*(x_2-x_3))+y_3*((x-x_1)*(x-x_2))/((x_3-x_1)*(x_3-x_2)));
    delay->peristalticPump_delay = y;
    ESP_LOGI(TAG, "Peristaltic Pump Delay: %ds", delay->peristalticPump_delay);
    }
    
}

void changeConfiguration(char* message)
{
    //"TempLow":"18","TempHigh":"25","PhLow":"6","PhHigh":"7","DOLow":"4","DOHigh":"5","FishFreq":"2" FORMAT
    sscanf(message, "\"TempLow\":\"%f\",\"TempHigh\":\"%f\",\"PhLow\":\"%f\",\"PhHigh\":\"%f\",\"DOLow\":\"%f\",\"DOHigh\":\"%f\",\"FishFreq\":\"%c\"",
    &(configValues.tempLow),
    &(configValues.tempHigh),
    &(configValues.phLow),
    &(configValues.phHigh),
    &(configValues.doLow),
    &(configValues.doHigh),
    &(configValues.fishFreq)
    );

    ESP_LOGI(TAG, "\"TempLow\":\"%f\",\"TempHigh\":\"%f\",\"PhLow\":\"%f\",\"PhHigh\":\"%f\",\"DOLow\":\"%f\",\"DOHigh\":\"%f\",\"FishFreq\":\"%c\" \n",
     (configValues.tempLow),
     (configValues.tempHigh),
     (configValues.phLow),
     (configValues.phHigh),
     (configValues.doLow),
     (configValues.doHigh),
     (configValues.fishFreq)
    );

    esp_err_t result;
    // Handle will automatically close when going out of scope or when it's reset.
    std::shared_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle("storage", NVS_READWRITE, &result);

    handle->set_item("tempLow", (int32_t) configValues.tempLow);
    handle->set_item("tempHigh", (int32_t) configValues.tempHigh);
    handle->set_item("doLow", (int32_t) configValues.doLow);
    handle->set_item("doHigh", (int32_t) configValues.doHigh);
    handle->set_item("phLow", (int32_t) configValues.phLow);
    handle->set_item("phHigh", (int32_t) configValues.phHigh);
    handle->set_item("fishFreq", (uint8_t) configValues.fishFreq);
}