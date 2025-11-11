#ifndef INC_ULTRASONIC_H_
#define INC_ULTRASONIC_H_

#include "main.h"  // TIM, GPIO 등을 사용하기 위해 필요

extern volatile uint32_t echo_start_L;
extern volatile uint32_t echo_end_L;
extern volatile uint8_t echo_captured_L;

extern volatile uint32_t echo_start_R;
extern volatile uint32_t echo_end_R;
extern volatile uint8_t echo_captured_R;

void delay_us(uint16_t us);
void send_trigger_pulse(void);
float get_distance_L(void);
float get_distance_R(void);

#endif /* INC_ULTRASONIC_H_ */
