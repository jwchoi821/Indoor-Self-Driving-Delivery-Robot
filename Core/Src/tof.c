#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "can.h"
#include "ultrasonic.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "VL53L1X_api.h"

#define TOF_ADDR 0x52  // 기본 주소 (0x29 << 1)
#define TOF1_PORT GPIOA
#define TOF1_PIN  GPIO_PIN_5
#define TOF2_PORT GPIOA
#define TOF2_PIN  GPIO_PIN_7

void TOF_Enable(GPIO_TypeDef *port, uint16_t pin) {
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

void TOF_Disable(GPIO_TypeDef *port, uint16_t pin) {
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	HAL_Delay(10);
}

// 센서 초기화 및 시작
void TOF_Start(GPIO_TypeDef *port, uint16_t pin) {
	//HAL_UART_Transmit(&huart2, (uint8_t*)"TOF_S\r\n", 7, HAL_MAX_DELAY);

	HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	HAL_Delay(10);

	//HAL_UART_Transmit(&huart2, (uint8_t*)"XSHUT HIGH\r\n", 12, HAL_MAX_DELAY);

	uint8_t booted = 0;
	int retry = 0;
	while (!booted && retry++ < 50) {
		int ret = VL53L1X_BootState(TOF_ADDR, &booted);
		char msg[32];
		// snprintf(msg, sizeof(msg), "boot:%d ret:%d\r\n", booted, ret);
		HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
		HAL_Delay(50);
	}

	if (!booted) {
		//HAL_UART_Transmit(&huart2, (uint8_t*)"Boot failed\r\n", 13, HAL_MAX_DELAY);
		return;// 센서 안 켜졌으니 중단
	}

	//HAL_UART_Transmit(&huart2, (uint8_t*)"Boot OK\r\n", 9, HAL_MAX_DELAY);
	VL53L1X_SensorInit(TOF_ADDR);
	VL53L1X_SetDistanceMode(TOF_ADDR, 2);
	VL53L1X_SetTimingBudgetInMs(TOF_ADDR, 50);
	VL53L1X_SetInterMeasurementInMs(TOF_ADDR, 100);
	VL53L1X_StartRanging(TOF_ADDR);
}

uint16_t TOF_ReadDistance(void) {
	uint8_t ready = 0;
	uint16_t distance = 0;

	while (!ready) {
		VL53L1X_CheckForDataReady(TOF_ADDR, &ready);
		HAL_Delay(2);
	}

	VL53L1X_GetDistance(TOF_ADDR, &distance);
	VL53L1X_ClearInterrupt(TOF_ADDR);

	return distance;
}

void TOF_test(void) {
	// TOF1만 활성화 및 반복 출력
	/*TOF_Start(TOF1_PORT, TOF1_PIN);

	 uint16_t d = TOF_ReadDistance();  // mm 단위

	 send_trigger_pulse();  // 초음파 트리거
	 HAL_Delay(10);
	 float dist_L = get_distance_L();
	 float dist_R = get_distance_R();

	 char msg[128];
	 //snprintf(msg, sizeof(msg)," TOF1: %d mm \r\n US_L: %.2f cm | US_R: %.2f cm \r\n CAN X: %.3f Y: %.3f \r\n ----------------------------\r\n",d, dist_L, dist_R, x, y);
	 HAL_UART_Transmit(&huart3, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
	 HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen(msg), HAL_MAX_DELAY);
	 HAL_Delay(300);  // 출력 속도 조절

	 VL53L1X_StopRanging(TOF_ADDR);
	 TOF_Disable(TOF1_PORT, TOF1_PIN);*/

	//TOF2 비활성화 코드 (현재 미사용)
	TOF_Start(TOF1_PORT, TOF1_PIN); //왼쪽 tof
	uint16_t d = TOF_ReadDistance();

	/*send_trigger_pulse();  // 초음파 트리거
	 HAL_Delay(10);
	 float dist_L = get_distance_L();
	 float dist_R = get_distance_R();*/

	char msg[128];
	snprintf(msg, sizeof(msg), "LEFT TOF1: %d \r\n", d);
	HAL_UART_Transmit(&huart2, (uint8_t*) msg, strlen (msg), HAL_MAX_DELAY);

	/*if (d < 200) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 8500); //오른쪽
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999); //왼쪽
	} else if (d < 400) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 8700);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	} else if (d < 600) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 8900);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	} else if (d < 800) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 9100);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	} else if (d < 1000) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 9300);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	} else if (d < 1200) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 9100);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	} else if (d < 1400) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 8900);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	} else if (d < 1600) {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 9999);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	} else {
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 9300);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 9999);
	}*/

	VL53L1X_StopRanging(TOF_ADDR);
	TOF_Disable(TOF1_PORT, TOF1_PIN);
	HAL_Delay(10);
}

