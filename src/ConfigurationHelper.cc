#include "AquaponicsStructs.h"
#include <cstdio>
#include <esp_log.h>

ConfigurationValues* configValues;

static const char* TAG = "CONFIG";

void changeConfiguration(char* message)
{

    //"TempLow":"18","TempHigh":"25","PhLow":"6","PhHigh":"7","DOLow":"4","DOHigh":"5","FishFreq":"2" FORMAT
    sscanf(message, "\"TempLow\":\"%f\",\"TempHigh\":\"%f\",\"PhLow\":\"%f\",\"PhHigh\":\"%f\",\"DOLow\":\"%f\",\"DOHigh\":\"%f\",\"FishFreq\":\"%c\"",
    &(configValues->tempLow),
    &(configValues->tempHigh),
    &(configValues->phLow),
    &(configValues->phHigh),
    &(configValues->doLow),
    &(configValues->doHigh),
    &(configValues->fishFreq)
    );

    ESP_LOGI(TAG, "\"TempLow\":\"%f\",\"TempHigh\":\"%f\",\"PhLow\":\"%f\",\"PhHigh\":\"%f\",\"DOLow\":\"%f\",\"DOHigh\":\"%f\",\"FishFreq\":\"%c\" \n",
     (configValues->tempLow),
     (configValues->tempHigh),
     (configValues->phLow),
     (configValues->phHigh),
     (configValues->doLow),
     (configValues->doHigh),
     (configValues->fishFreq)
    );
}

void computeDelayValues(ConfigurationValues* val, DelayValues* delay, ForecastedValue* forecast)
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
    ESP_LOGI(TAG, "Heater Delay: %f s", delay->heater_delay);
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
    ESP_LOGI(TAG, "Aerator Delay: %fs", delay->aerator_delay);
    }
    
    {
    // PH
    float x_1, x_2, x_3, y_1, y_2, y_3, x;
    x = forecast->PH;
    x_1 = val->phLow;
    x_2 = val->phLow + (val->phHigh - val->phLow)*(1/8);
    x_3 = val->phLow + (val->phHigh - val->phHigh)*(1/4);

    float highDelay = 540 * 1000;

    y_1 = highDelay;
    y_2 = highDelay / 2;
    y_3 = 0;

    uint32_t y = (uint32_t)(y_1*((x-x_2)*(x-x_3))/((x_1-x_2)*(x_1-x_3))+y_2*((x-x_1)*(x-x_3))/((x_2-x_1)*(x_2-x_3))+y_3*((x-x_1)*(x-x_2))/((x_3-x_1)*(x_3-x_2)));
    delay->peristalticPump_delay = y;
    ESP_LOGI(TAG, "Peristaltic Pump Delay: %fs", delay->peristalticPump_delay);
    }
    
}