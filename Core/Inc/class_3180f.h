/*
 * class_3180f.h
 *
 *  Created on: May 11, 2025
 *      Author: JenogWoo
 */

#ifndef INC_CLASS_3180F_H_
#define INC_CLASS_3180F_H_

#include <stdint.h>
#include "main.h"  // main.h 헤더 파일 포함
#include <stdio.h>  // 표준 입출력 헤더 추가
#include <stdlib.h> // 표준 라이브러리 헤더 추가

extern float x;  // 목표 X 좌표
extern float y;  // 목표 Y 좌표
extern uint8_t bt_rx_buffer[100];  // bt_rx_buffer를 extern으로 선언

void class_3180f(void);

#endif /* INC_CLASS_3180F_H_ */
