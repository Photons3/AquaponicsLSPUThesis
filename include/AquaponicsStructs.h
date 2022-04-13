#ifndef _AQUAPONICS_STRUCTS_H_
#define _AQUAPONICS_STRUCTS_H_

#ifdef __cplusplus
extern "C" {
#include <cstdio>
#else
#include <stdio.h>
#endif


struct DelayValues
{
    uint32_t heater_delay;
    uint32_t aerator_delay;
    uint32_t peristalticPump_delay;
    uint32_t watervalve_delay;
    uint32_t fishfeed_delay;
};

struct LatestSensorValues 
{
    float temperature_value[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float dissolveOxygen_value[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float pH_value[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t water_height = 10;
};

struct ForecastedValue
{
    float temperature;
    float DO;
    float PH;
};

struct ScalerValues
{
    float mean_Temp;
    float std_Temp;
    float mean_DO;
    float std_DO;
    float mean_PH;
    float std_PH;
};

struct ConfigurationValues
{
    //"TempLow":"18","TempHigh":"25","PhLow":"6","PhHigh":"7","DOLow":"4","DOHigh":"5","FishFreq":"2" FORMAT
    float tempLow;
    float tempHigh;
    float phLow;
    float phHigh;
    float doLow;
    float doHigh;
    uint8_t fishFreq;
};

#ifdef __cplusplus
}
#endif

#endif