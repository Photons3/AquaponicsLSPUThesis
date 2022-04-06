#ifndef _AQUAPONICS_STRUCTS_H_
#define _AQUAPONICS_STRUCTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

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


#ifdef __cplusplus
}
#endif

#endif