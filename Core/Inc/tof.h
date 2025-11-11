/*
 * tof.h
 *
 *  Created on: Jun 1, 2025
 *      Author: Asus User
 */

#ifndef INC_TOF_H_
#define INC_TOF_H_

#include "main.h"

void TOF_Enable(GPIO_TypeDef *port, uint16_t pin);
void TOF_Disable(GPIO_TypeDef *port, uint16_t pin);

// 센서 초기화 및 시작
void TOF_Start(GPIO_TypeDef *port, uint16_t pin);

uint16_t TOF_ReadDistance(void);

void TOF_test(void);
#endif /* INC_TOF_H_ */
