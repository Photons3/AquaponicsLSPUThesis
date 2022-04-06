#include <cmath>

#include "AquaponicsStructs.h"

void getScalerValues(float (&data)[10][3], ScalerValues &scaler) 
{
    // Temperature Mean
    float sumTemp = 0.0;
    for(int i = 0; i < 10; ++i) {
        sumTemp += data[i][0];
    }

    // Temperature SD
    float meanTemp = 0.0;
    meanTemp = sumTemp / 10;
    scaler.mean_Temp = meanTemp;

    float standardDeviationTemp = 0.0;
    for(int i = 0; i < 10; ++i) {
        standardDeviationTemp += pow(data[i][0] - meanTemp, 2);
    }

    float sdTemp = sqrt(standardDeviationTemp / 10);
    scaler.std_Temp = sdTemp;

    // DO Mean
    float sumDO = 0.0;
    for(int i = 0; i < 10; ++i) {
        sumDO += data[i][1];
    }

    // DO SD
    float meanDO = 0.0;
    meanDO = sumDO / 10;
    scaler.mean_DO = meanDO;

    float standardDeviationDO = 0.0;
    for(int i = 0; i < 10; ++i) {
        standardDeviationDO += pow(data[i][1] - meanDO, 2);
    }

    float sdDO = sqrt(standardDeviationDO / 10);
    scaler.std_DO = sdDO;

    // PH Mean
    float sumPH = 0.0;
    for(int i = 0; i < 10; ++i) {
        sumPH += data[i][2];
    }

    //PH SD
    float meanPH = 0.0;
    meanPH = sumPH / 10;
    scaler.mean_PH = meanPH;

    float standardDeviationPH = 0.0;
    for(int i = 0; i < 10; ++i) {
        standardDeviationPH += pow(data[i][2] - meanPH, 2);
    }

    float sdPH = sqrt(standardDeviationPH / 10);
    scaler.std_PH = sdPH;
}

void standardScaler(float (&data)[10][3], ScalerValues &scaler)
{
    // Temperature Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][0] - scaler.mean_Temp) / scaler.std_Temp;
        data[i][0] = newValue;
    }

    // DO Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][1] - scaler.mean_DO) / scaler.std_DO;
        data[i][1] = newValue;
    }

    // PH Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][2] - scaler.mean_PH) / scaler.std_PH;
        data[i][2] = newValue;
    }
}

void inverseStandardScaler(float (&data)[10][3], ScalerValues &scaler)
{
    // Temperature Inversed Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][0] * scaler.std_Temp) + scaler.mean_Temp;
        data[i][0] = newValue;
    }

    // DO Inversed Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][1] * scaler.std_DO) + scaler.mean_DO;
        data[i][1] = newValue;
    }

    // DO Inversed Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][2] * scaler.std_PH) + scaler.mean_PH;
        data[i][2] = newValue;
    }
}