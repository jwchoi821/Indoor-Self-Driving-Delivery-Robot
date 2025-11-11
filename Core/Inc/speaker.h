#ifndef __SPEAKER_H
#define __SPEAKER_H

#include "stm32f1xx_hal.h"   // 또는 F3 쓰면 f3xx_hal.h
#include <stdint.h>
#include "main.h"  // main.h 헤더 파일 포함
#include <stdio.h>  // 표준 입출력 헤더 추가
#include <stdlib.h>

void Speaker_Init(void);          // Reset → TF 선택 → 볼륨(25)
void Speaker_SetVolume(uint8_t v);
void Speaker_Play(uint16_t index); // /mp3/000i.mp3 (i=1..3000)
void Speaker_Stop(void);           // 옵션

#endif
