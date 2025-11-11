#include "ultrasonic.h"
#include "tim.h"
#include "gpio.h"
#include "main.h"

volatile uint32_t echo_start_L = 0;
volatile uint32_t echo_end_L = 0;
volatile uint8_t echo_captured_L = 0;

volatile uint32_t echo_start_R = 0;
volatile uint32_t echo_end_R = 0;
volatile uint8_t echo_captured_R = 0;

void delay_us(uint16_t us) {

    __HAL_TIM_SET_COUNTER(&htim3, 0);
    while (__HAL_TIM_GET_COUNTER(&htim3) < us);

}

void send_trigger_pulse(void) {
    // Left sensor - PA7
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);

    HAL_Delay(60);  // wait for sensor response

    // Right sensor - PA4
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
}

float get_distance_L(void) {
    return (echo_end_L - echo_start_L) * 0.0343f / 2.0f;
}

float get_distance_R(void) {
    return (echo_end_R - echo_start_R) * 0.0343f / 2.0f;
}
