#include "AquaponicsStructs.h"
#include <cstdio>
#include <esp_log.h>

ConfigurationValues configValues;
ControlsCoefficients controlCoefficients;

static const char* TAG = "CONFIG";

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
}

void recomputeCoefficients(ConfigurationValues* val, ControlsCoefficients* coeff)
{
    //TODO: Recompute using exponential or quadratic function
    coeff->aeratorCoef[0] = 13;
    coeff->aeratorCoef[1] = 14;
    coeff->aeratorCoef[2] = 15;

    coeff->heaterCoef[0] = 0;
    coeff->heaterCoef[1] = 3;
    coeff->heaterCoef[2] = 4;

    coeff->peristalticPumpCoef[0] = 3; 
    coeff->peristalticPumpCoef[1] = 3;
    coeff->peristalticPumpCoef[2] = 3;

}
