/*
 * bluetooth.h
 *
 *  Created on: May 28, 2025
 *      Author: Asus User
 */

#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "main.h"

extern uint8_t bt_rx_buffer[100];
extern volatile uint8_t bt_command_ready;

void Bluetooth_Init(void);
void Bluetooth_Process_CAN(float x, float y, uint8_t* can_data_ready_flag);

#endif

