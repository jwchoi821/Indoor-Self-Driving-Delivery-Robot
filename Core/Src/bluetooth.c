#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "can.h"
#include "ultrasonic.h"
#include "bluetooth.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t bt_rx_data=0;
uint8_t bt_rx_buffer[100];
uint8_t bt_rx_index = 0;

extern uint8_t bt_data_received;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if (huart->Instance == USART3) {
        if (bt_rx_data != '\n' && bt_rx_index < sizeof(bt_rx_buffer) - 1) {
            bt_rx_buffer[bt_rx_index++] = bt_rx_data;
        } else {
            bt_rx_buffer[bt_rx_index] = '\0';
            bt_data_received = 1;

            HAL_UART_Transmit(&huart2, (uint8_t*)"Bluetooth Received: ", 20, HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart2, bt_rx_buffer, bt_rx_index, HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
            bt_rx_index = 0;
        }
        HAL_UART_Receive_IT(&huart3, &bt_rx_data, 1);
    }
}

