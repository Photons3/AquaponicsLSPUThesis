#include "driver/mcpwm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SERVO_MIN_PULSEWIDTH_US (500) // Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH_US (2400) // Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE        (90)   // Maximum angle in degree upto which servo can rotate

#define SERVO_PULSE_GPIO        GPIO_NUM_27   // FISH FEEDER PIN

static inline uint32_t convert_servo_angle_to_duty_us(int angle)
{
    return (angle + SERVO_MAX_DEGREE) * (SERVO_MAX_PULSEWIDTH_US - SERVO_MIN_PULSEWIDTH_US) / (2 * SERVO_MAX_DEGREE) + SERVO_MIN_PULSEWIDTH_US;
}

void servoMotorInit(void)
{
    mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, SERVO_PULSE_GPIO); // To drive a RC servo, one MCPWM generator is enough

      mcpwm_config_t pwm_config = {
          .frequency = 50, // frequency = 50Hz, i.e. for every servo motor time period should be 20ms
          .cmpr_a = 0,     // duty cycle of PWMxA = 0
          .counter_mode = MCPWM_UP_COUNTER,
          .duty_mode = MCPWM_DUTY_MODE_0,
      };
    mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config);

    ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, convert_servo_angle_to_duty_us(55)));
}

void fishFeederOn(void)
{
    for (int angle = 55; angle > 0; angle--) {
        //ESP_LOGI(CONTROLS, "Angle of rotation: %d", angle);
        ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, convert_servo_angle_to_duty_us(angle)));
        vTaskDelay(pdMS_TO_TICKS(75)); //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation under 5V power supply
    }
}

void fishFeederOff(void)
{
    for (int angle = 0; angle < 55; angle++) {
        //ESP_LOGI(CONTROLS, "Angle of rotation: %d", angle);
        ESP_ERROR_CHECK(mcpwm_set_duty_in_us(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_OPR_A, convert_servo_angle_to_duty_us(angle)));
        vTaskDelay(pdMS_TO_TICKS(75)); //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation under 5V power supply
    }
}