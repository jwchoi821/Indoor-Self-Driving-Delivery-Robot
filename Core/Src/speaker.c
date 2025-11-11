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

static HAL_StatusTypeDef DF_SendFrame(uint8_t cmd, uint16_t param)
{
  uint8_t ph = (param >> 8) & 0xFF;
  uint8_t pl = (param & 0xFF);
  uint16_t sum = 0xFF + 0x06 + cmd + 0x00 + ph + pl;
  uint16_t cks = (uint16_t)(0x10000 - sum);
  uint8_t f[10] = {0x7E,0xFF,0x06,cmd,0x00,ph,pl,
                   (uint8_t)(cks>>8),(uint8_t)cks,0xEF};
  return HAL_UART_Transmit(&huart1, f, sizeof(f), 100);
}

static inline void DF_Reset(void)    { DF_SendFrame(0x3F, 0x0000); }
static inline void DF_SelectTF(void) { DF_SendFrame(0x09, 0x0002); }

void Speaker_Init(void)
{
  HAL_Delay(200);
  DF_Reset();
  HAL_Delay(1200);
  DF_SelectTF();
  HAL_Delay(200);
  Speaker_SetVolume(25);
}

void Speaker_SetVolume(uint8_t vol)
{
  if (vol > 30) vol = 30;
  DF_SendFrame(0x06, vol);
}

void Speaker_Play(uint16_t index)
{
  if (index == 0) index = 1;
  DF_SendFrame(0x12, index);     // /mp3/000i.mp3
}

void Speaker_Stop(void)           // 옵션: 일부 클론은 0x0D,0도 사용
{
  DF_SendFrame(0x16, 0x0000);     // STOP(A)
  HAL_Delay(50);
  // DF_SendFrame(0x0D, 0x0000);  // STOP(B) 필요시
}
