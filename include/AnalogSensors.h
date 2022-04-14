#ifndef _ANALOG_SENSORS_H_
#define _ANALOG_SENSORS_H_

#ifdef __cplusplus
extern "C" {
#endif

extern void adc_calibration_init(void);

extern float read_PH_sensorValue();

extern float read_Temp_sensorValue();

extern float read_DO_sensorValue(uint32_t temperature_c);

#ifdef __cplusplus
}
#endif

#endif