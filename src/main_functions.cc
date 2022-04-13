#include "main_functions.h" 

#include <math.h>

#include <esp_log.h>
#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "model.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"

#include "AnalogSensors.h"
#include "AquaponicsStructs.h"
#include "ConfigurationHelperCxx.h"
#include "HiveMQBroker.h"
#include "RTC1302.h"
#include "Ultrasonicsensor.h"

static const char* CONTROLS = "CONTROLS";
static const char* SENSORS = "SENSORS";

namespace {
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  constexpr int kTensorArenaSize = 2496; // 3 * 1024 2496 according to app
  uint8_t tensor_arena[kTensorArenaSize];   // Set the size of Tensor arena
}

static DelayValues delays;
static ForecastedValue forecast;
static struct tm currentTime;

static LatestSensorValues sensors;

// LOAD THE INPUT DATA TO THE INPUT TENSORS
static void load_data(float (&inputData)[10][3])
{
  // THIS FUNCTION WILL APPEND ALL THE VALUES OF inputData
  // TO THE RIGHT INPUT TENSOR IN FLOAT FORM
  int index = 0;
  for (int i = 0; i < 10; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      input->data.f[index] = static_cast<float>(inputData[i][j]);
      ++index;
    } 
  }
}

// This function will run inference on our data
// The output will be also in the input data
static void runInference(float (&inputData)[10][3])
{
  // Requires the pointer to input and output tensors
  
  // Run the model 10 times to get a 10 minute forecast
  for (int i = 0; i < 10; i++)
  {
    load_data(inputData);
    TfLiteStatus invoke_status = interpreter->Invoke();
    if (invoke_status != kTfLiteOk) {
      // if invoke failed just repeat the last values
      output->data.f[0] = inputData[9][0];
      output->data.f[1] = inputData[9][1];
      output->data.f[2] = inputData[9][2];
    }
    // This will move the second element of the timeseries to the first
    // and append the output values of the NN to the last element of the array
    memmove((void*) &inputData[0][0], (void*) &inputData[1][0], 9 * 3 * sizeof(float));
    memcpy((void*) &inputData[9][0], (void*) &output->data.f[0], 3 * sizeof(float));
  }
}

static void getScalerValues(float (&data)[10][3], ScalerValues &scaler) 
{
    // Temperature Mean
    double sumTemp = 0.0;
    for(int i = 0; i < 10; ++i) {
        sumTemp += data[i][0];
    }

    // Temperature SD
    double meanTemp = 0.0;
    meanTemp = sumTemp / 10;
    scaler.mean_Temp = meanTemp;

    double standardDeviationTemp = 0.0;
    for(int i = 0; i < 10; ++i) {
        standardDeviationTemp += pow(data[i][0] - meanTemp, 2);
    }

    double sdTemp = sqrt(standardDeviationTemp / 10);
    scaler.std_Temp = sdTemp;

    // DO Mean
    double sumDO = 0.0;
    for(int i = 0; i < 10; ++i) {
        sumDO += data[i][1];
    }

    // DO SD
    double meanDO = 0.0;
    meanDO = sumDO / 10;
    scaler.mean_DO = meanDO;

    double standardDeviationDO = 0.0;
    for(int i = 0; i < 10; ++i) {
        standardDeviationDO += pow(data[i][1] - meanDO, 2);
    }

    double sdDO = sqrt(standardDeviationDO / 10);
    scaler.std_DO = sdDO;

    // PH Mean
    double sumPH = 0.0;
    for(int i = 0; i < 10; ++i) {
        sumPH += data[i][2];
    }

    //PH SD
    double meanPH = 0.0;
    meanPH = sumPH / 10;
    scaler.mean_PH = meanPH;

    double standardDeviationPH = 0.0;
    for(int i = 0; i < 10; ++i) {
        standardDeviationPH += (data[i][2] - meanPH) * (data[i][2] - meanPH);
    }

    double sdPH = sqrt(standardDeviationPH / 10);
    scaler.std_PH = sdPH;
}

static void standardScaler(float (&data)[10][3], ScalerValues &scaler)
{
    // Temperature Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][0] - scaler.mean_Temp) / scaler.std_Temp;
        if (isnan(newValue))
        {
          newValue = 0;
        }
        data[i][0] = newValue;
    }

    // DO Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][1] - scaler.mean_DO) / scaler.std_DO;
        if (isnan(newValue))
        {
          newValue = 0;
        }
        data[i][1] = newValue;
    }

    // PH Scaling
    for(int i = 0; i < 10; ++i) {
        float newValue = (data[i][2] - scaler.mean_PH) / scaler.std_PH;
        if (isnan(newValue))
        {
          newValue = 0;
        }
        data[i][2] = newValue;
    }
}

static void inverseStandardScaler(float (&data)[10][3], ScalerValues &scaler)
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

static void getMedianValues(float (&data)[10][3], ForecastedValue &forecast)
{
  // TEMPERATURE
  {
    float temperature[10];
    for (int i = 0; i < 10; i++)
    {
      temperature[i] = data[i][0];
    }
    int size = sizeof(temperature)/sizeof(temperature[0]);
    float tempForecast = (float)(temperature[(size-1)/2] + temperature[size/2])/2.0;

    forecast.temperature = tempForecast;
  }

  // DO
  {
    float DO[10];
    for (int i = 0; i < 10; i++)
    {
      DO[i] = data[i][1];
    }
    int size = sizeof(DO)/sizeof(DO[0]);
    float DOForecast = (float)(DO[(size-1)/2] + DO[size/2])/2.0;

    forecast.DO = DOForecast;
  }

  // PH
  {
    float PH[10];
    for (int i = 0; i < 10; i++)
    {
      PH[i] = data[i][1];
    }
    int size = sizeof(PH)/sizeof(PH[0]);
    float PHForecast = (float)(PH[(size-1)/2] + PH[size/2])/2.0;

    forecast.PH = PHForecast;
  }
}

void vHeater(void *params) 
{ 
  gpio_pad_select_gpio(GPIO_NUM_21);
  gpio_set_direction (GPIO_NUM_21,GPIO_MODE_OUTPUT);  
  vTaskDelay( pdMS_TO_TICKS(30 * 1000) ); // DELAY FOR 30 SECS
  // Loop forever
  while (1) {
    uint32_t vDelay = delays.heater_delay;
    gpio_set_level(GPIO_NUM_21, 1);
    ESP_LOGI(CONTROLS, "HEATER ON: %ds", vDelay);
    vTaskDelay( pdMS_TO_TICKS(vDelay) );

    gpio_set_level(GPIO_NUM_21, 0);
    ESP_LOGI(CONTROLS, "HEATER OFF");
    vTaskDelay( pdMS_TO_TICKS(10000 - vDelay));
  }
}

void vPeristalticPump(void *params)
{
  vTaskDelay( pdMS_TO_TICKS(30 * 1000) ); // DELAY FOR 30 SECS
  // Loop forever
  while (1) {
    // Blink

    ESP_LOGI(CONTROLS, "PERISTALTIC PUMP ON: %ds", delays.peristalticPump_delay);
    vTaskDelay( pdMS_TO_TICKS(delays.peristalticPump_delay) );

    ESP_LOGI(CONTROLS, "PERISTALTIC PUMP OFF");
    vTaskDelay( pdMS_TO_TICKS(10000 - delays.peristalticPump_delay));
  }
}

void vAerator(void *params) 
{
  gpio_set_level(GPIO_NUM_3, 1);
  vTaskDelay( pdMS_TO_TICKS(30 * 1000) ); // DELAY FOR 30 SECS
  // Loop forever
  while (1) {
    // Blink
    uint32_t vDelay = delays.aerator_delay;
    gpio_set_level(GPIO_NUM_3, 1);
    ESP_LOGI(CONTROLS, "AERATOR ON: %ds", vDelay);
    vTaskDelay( pdMS_TO_TICKS( vDelay ) );
    
    gpio_set_level(GPIO_NUM_3, 0);
    ESP_LOGI(CONTROLS, "AERATOR OFF");
    vTaskDelay( pdMS_TO_TICKS(10000 - vDelay) );
  }
}

void vFishFeed(void *params) 
{

  vTaskDelay( pdMS_TO_TICKS(30 * 1000) ); // DELAY FOR 30 SECS
  // Loop forever
  int lastFeedTime = 0;
  uint8_t fishFreq;
  uint8_t feedFreq;
  int nextFeedTime;
  while (1) {
    fishFreq = configValues.fishFreq;
    feedFreq = ceil(24/fishFreq);
    nextFeedTime = lastFeedTime + feedFreq;
    //Compensate for 24 hours
    if (((currentTime.tm_hour < lastFeedTime) && (currentTime.tm_hour + 24) >= nextFeedTime) || (currentTime.tm_hour >= nextFeedTime))
    {
      //Open the FishFeeder
      lastFeedTime = currentTime.tm_hour;
    }
    // Blink
    //digitalWrite(led_pin, HIGH);
    ESP_LOGI(CONTROLS, "FISHFEED ON: %ds", delays.fishfeed_delay);
    vTaskDelay( pdMS_TO_TICKS( delays.fishfeed_delay ) );
    //digitalWrite(led_pin, LOW);
    ESP_LOGI(CONTROLS, "FISHFEED OFF");
    vTaskDelay( pdMS_TO_TICKS(10000 - delays.fishfeed_delay) );
  }
}

void vMainTask(void* params)
{
  adc_calibration_init();
  initConfigurationValues();

  // ---------------------SETUP ESP-NN---------------------------------//
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    TF_LITE_REPORT_ERROR(error_reporter,
                        "Model provided is schema version %d not equal "
                        "to supported version %d.",
                        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  //static tflite::AllOpsResolver resolver;

  tflite::MicroMutableOpResolver<4> micro_op_resolver;
  micro_op_resolver.AddQuantize();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddDequantize();


  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk) {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);
  // ---------------------SETUP ESP-NN---------------------------------//

  uint8_t iterationCount = 0;
  while(1)
  {
    get_time_from_RTC(&currentTime);
    sensors.water_height = get_water_height();
    sensors.temperature_value[iterationCount] = read_Temp_sensorValue();
    sensors.pH_value[iterationCount] = read_PH_sensorValue();
    sensors.dissolveOxygen_value[iterationCount] = read_DO_sensorValue( (uint32_t) sensors.temperature_value[iterationCount] ) / 1000;

    //SEND VALUES TO HIVEMQ      
    char payload[75];
    memset(payload, '\0', 75 - 1);
    sprintf(payload, "{\"TEMP\": %f, \"DO\": %f, \"PH\": %f, \"WH\": %d}",
              sensors.temperature_value[iterationCount],
              sensors.dissolveOxygen_value[iterationCount],
              sensors.pH_value[iterationCount],
              sensors.water_height
              );
    const uint8_t payloadLen = strlen(payload);
    esp_mqtt_client_enqueue(client, "/aquaponics/lspu/sensors", payload, payloadLen, 0, 0, true);

    ESP_LOGI(SENSORS, "Waterheight: %d", sensors.water_height);
    ESP_LOGI(SENSORS, "Temperature[%d]: %f", iterationCount, sensors.temperature_value[iterationCount]);
    ESP_LOGI(SENSORS, "Dissolved Oxygen[%d]: %f", iterationCount, sensors.dissolveOxygen_value[iterationCount]);
    ESP_LOGI(SENSORS, "PH[%d]: %f", iterationCount, sensors.pH_value[iterationCount]);


    if (iterationCount == 9)
    {
      // Do the inference here
      float data[10][3];

      for (int i = 0; i < 10; i++)
      {
        data[i][0] = sensors.temperature_value[i];
        data[i][1] = sensors.dissolveOxygen_value[i];
        data[i][2] = sensors.pH_value[i];
      }

      //Do the inference by doing a standard scaler and outputing the values to data
      ScalerValues scaler;
      getScalerValues( data, scaler );
      standardScaler( data, scaler );
      runInference( data );
      inverseStandardScaler( data, scaler );

      getMedianValues(data, forecast);
      setDelayValues(&configValues, &delays, &forecast);

      // SEND PREDICTIONS TO HIVEMQ
      char predictionPayload[400];
      memset(predictionPayload, '\0', 400 - 1);
      sprintf(predictionPayload, "{\"temperature\": [ %f, %f, %f, %f, %f, %f, %f, %f, %f, %f], \"DOLevel\": [ %f, %f, %f, %f, %f, %f, %f, %f, %f, %f], \"pHLevel\": [ %f, %f, %f, %f, %f, %f, %f, %f, %f, %f]}",
              data[0][0], data[1][0], data[2][0], data[3][0], data[4][0], data[5][0], data[6][0], data[7][0], data[8][0], data[9][0],
              data[0][1], data[1][1], data[2][1], data[3][1], data[4][1], data[5][1], data[6][1], data[7][1], data[8][1], data[9][1],
              data[0][2], data[1][2], data[2][2], data[3][2], data[4][2], data[5][2], data[6][2], data[7][2], data[8][2], data[9][2]
              );
      const uint8_t predictionPayloadLen = strlen(&(predictionPayload[250]));
      esp_mqtt_client_enqueue(client, "/aquaponics/lspu/predictions", predictionPayload, 250 + predictionPayloadLen, 0, 0, true);
    }

    vTaskDelay( pdMS_TO_TICKS(1000) );
    
    iterationCount++;

    if (iterationCount > 9)
    {
      iterationCount = 0;
    }
  }
}