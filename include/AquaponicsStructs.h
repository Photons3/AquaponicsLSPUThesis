#ifndef _AQUAPONICS_STRUCTS_H_
#define _AQUAPONICS_STRUCTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdio>

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

struct ControlsCoefficients
{
    float heaterCoef[3];
    float aeratorCoef[3];
    float peristalticPumpCoef[3];
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
    float tempLow = 17.0f;
    float tempHigh = 33.0f;
    float phLow = 5.5f;
    float phHigh = 8.0f;
    float doLow = 10.0f;
    float doHigh = 40.0f;
    uint8_t fishFreq = 3;
};


#ifdef __cplusplus
}
#endif

#endif