#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "can.h"
#include "ultrasonic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tof.h"
#include "VL53L1X_api.h"
#include "speaker.h"

// 목적지 함수 헤더
#include "motor_stop.h"
#include "L_chan.h"
#include "K_hun.h"
#include "J_yoo.h"
#include "L_rok.h"
#include "class_3140f.h"
#include "class_3170f.h"
#include "class_3180f.h"

//3170 -11.0f
//3180 -18.5f

extern uint16_t TOF_ReadDistance(void);

#define TOF_ADDR 0x52  // 기본 주소 (0x29 << 1)
#define TOF2_PORT GPIOA
#define TOF2_PIN  GPIO_PIN_7

extern float front;
extern float back;
extern float x;
extern float y;

static inline int Button_PC13_Pressed(void) {
	// 풀업 구성: 평상시 HIGH, 눌리면 LOW
	return (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET);
}

/* 간단 디바운스 포함: '막 눌림(edge)'을 기다렸다가 리턴 */
static void Wait_Button_PC13_Press(void) {
	// 1) 먼저 버튼이 눌려있지 않은 상태(릴리즈)를 기다림 — 이전 눌림 잔상 방지
	while (Button_PC13_Pressed()) {
		HAL_Delay(1);
	}

	// 2) 새로 눌리는 순간을 기다림
	while (!Button_PC13_Pressed()) {
		HAL_Delay(1);
	}

	// 3) 디바운스
	HAL_Delay(30);
	while (!Button_PC13_Pressed()) {
		HAL_Delay(1);
	} // 30ms 후에도 여전히 눌림이 아닐 경우 재대기
}

void class_3180f(void) {
	// PWM 시작

	HAL_UART_Transmit(&huart2, (uint8_t*) "Hello \r\n", 17, HAL_MAX_DELAY);

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

	// TOF1 시작
	TOF_Start(TOF2_PORT, TOF2_PIN);

	static uint8_t spk_inited = 0;   // 함수 재호출 시 중복 초기화 방지
	if (!spk_inited) {
		Speaker_Init();              // Reset -> TF 선택 -> 볼륨(25)
		spk_inited = 1;
	}

	// 필요하면 볼륨 조정(0~30)
	Speaker_SetVolume(28);
	HAL_Delay(1000);

	Speaker_Play(5); // 3180 출발

	HAL_Delay(4000);

	HAL_UART_Transmit(&huart2, (uint8_t*) "TOF_Start \r\n", 17, HAL_MAX_DELAY);

	while (x > -22.0f) {

		uint16_t d1 = TOF_ReadDistance();
		//send_trigger_pulse(); //////초음파///////
		char msg[128];

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 0);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, 1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, 0);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, 1);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 0);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 1);

		snprintf(msg, sizeof(msg), " TOF1: %d mm\r\n"
				" CAN X: %.3f, Y: %.3f\r\n"
				" ----------------------------\r\n", d1, x, y);
		// " CAN Front: %.2f m | Back: %.2f m\r\n" back, front
		HAL_Delay(10);
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart3, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

		// 거리 기반 속도 조절 (좌측 기준 보정) // 기본값 직진 기준
		int left_pwm;
		int right_pwm;  // 기본값 직진 기준

		if (back >= 0.1 && back <= 2.0) {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
			Speaker_Play(17);
			HAL_Delay(7000);
			continue;
		}

		else if (x > -21.0f && x < -19.2f) {
			right_pwm = 5550;
			left_pwm = 7300;
		} else if (d1 < 350) {
			right_pwm = 9900;
			left_pwm = 7000;
		} else if (d1 < 450) {
			right_pwm = 9850;
			left_pwm = 7000;
		} else if (d1 < 600) {
			right_pwm = 9800;
			left_pwm = 7100;
		} else if (d1 < 700) {
			right_pwm = 9800;
			left_pwm = 7300;
		} else if (d1 < 800) {
			right_pwm = 9650;
			left_pwm = 7200;
		} else if (d1 < 820) { //800 ~ 900
			right_pwm = 9470;
			left_pwm = 7600;
		} else if (d1 < 840) { //800 ~ 900
			right_pwm = 9350;
			left_pwm = 8000;
		} else if (d1 < 860) { //800 ~ 900
			right_pwm = 9280;
			left_pwm = 8100;
		} else if (d1 < 880) { //800 ~ 900
			right_pwm = 9200;
			left_pwm = 8200;
		} else if (d1 < 900) { //800 ~ 900
			right_pwm = 8500;
			left_pwm = 8400;
		} else if (d1 < 1000) { //800 ~ 900
			right_pwm = 8300;
			left_pwm = 8400;
		} else if (d1 < 1100) { //800 ~ 900
			right_pwm = 8200;
			left_pwm = 8600;
		} else if (d1 < 1200) {
			right_pwm = 8100;
			left_pwm = 8600;
		} else if (d1 < 1500) {
			right_pwm = 8000;
			left_pwm = 9100;
		} else {
			right_pwm = 9400;
			left_pwm = 7300;
		}
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, right_pwm); // 오른쪽
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, left_pwm);  // 왼쪽

		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 7000); // 오른쪽
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 7000);  // 왼쪽
	}

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); // 오른쪽
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); // 오른쪽
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	HAL_Delay(1000);

	Speaker_Play(6); // 31８0 도착
	HAL_Delay(5000);

	Speaker_Play(15);

	Wait_Button_PC13_Press();
	HAL_Delay(10);

	Speaker_Play(16);
	HAL_Delay(5000);

	while (x < -0.5f) {

		uint16_t d1 = TOF_ReadDistance();
		//send_trigger_pulse();
		char msg[128];

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, 0);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, 1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, 0);

		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, 1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 0);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, 0);

		snprintf(msg, sizeof(msg), " TOF1: %d mm\r\n"
				" CAN X: %.3f, Y: %.3f\r\n"
				" ----------------------------\r\n", d1, x, y);
		// " CAN Front: %.2f m | Back: %.2f m\r\n" back, front
		HAL_Delay(10);
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
		HAL_UART_Transmit(&huart3, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);

		// 거리 기반 속도 조절 (좌측 기준 보정)
		// 기본값 직진 기준
		// 거리 기반 속도 조절 (좌측 기준 보정)

		int left_pwm;
		int right_pwm;  // 기본값 직진 기준

		if (front >= 0.1 && front <= 2.0) {
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
			Speaker_Play(17);
			HAL_Delay(7000);
			continue;
		} else if (x > -21.0f && x < -19.2f) {
			right_pwm = 5550;
			left_pwm = 7300;
		} else if (d1 < 350) {
			right_pwm = 9850;
			left_pwm = 7000;
		} else if (d1 < 450) {
			right_pwm = 9850;
			left_pwm = 7100;
		} else if (d1 < 550) {
			right_pwm = 9700;
			left_pwm = 7200;
		} else if (d1 < 650) {
			right_pwm = 9600;
			left_pwm = 7350;
		} else if (d1 < 750) { //800 ~ 900
			right_pwm = 9400;
			left_pwm = 7350;
		} else if (d1 < 800) { //800 ~ 900
			right_pwm = 9200;
			left_pwm = 7750;
		} else if (d1 < 850) { //800 ~ 900
			right_pwm = 8700;
			left_pwm = 7750;
		} else if (d1 < 900) { //800 ~ 900
			right_pwm = 8300;
			left_pwm = 8000;
			//HAL_Delay(5);
		} else if (d1 < 1000) { //800 ~ 900
			right_pwm = 8050;
			left_pwm = 8200;
			//HAL_Delay(5);
		} else if (d1 < 1100) { //800 ~ 900
			right_pwm = 7800;
			left_pwm = 8400;
			//HAL_Delay(5);
		} else if (d1 < 1200) {
			right_pwm = 7600;
			left_pwm = 8550;
		} else if (d1 < 1600) {
			right_pwm = 7000;
			left_pwm = 9000;
		} else {
			right_pwm = 8800;
			left_pwm = 7500;
		}
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, right_pwm); // 오른쪽
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, left_pwm);  // 왼쪽

		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 7000); // 오른쪽
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 7000);  // 왼쪽
	}

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 0); // 오른쪽
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0); // 오른쪽
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
}

