#ifndef _CONFIGURATION_HELPERCXX_H_
#define _CONFIGURATION_HELPERCXX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "AquaponicsStructs.h"

extern void initConfigurationValues();

extern void setDelayValues(ConfigurationValues* val, DelayValues* delay, ForecastedValue* forecast);

extern ConfigurationValues configValues;

#ifdef __cplusplus
}
#endif

#endif