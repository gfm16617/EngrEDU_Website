/*
 * WS2812.h
 *
 *  Created on: May 10, 2026
 *      Author: gfm
 */

#ifndef INC_ENGREDU_WS2812_H_
#define INC_ENGREDU_WS2812_H_

#include "stm32f4xx_hal.h"

#define NUM_LEDS 1

void WS2812_Init(TIM_HandleTypeDef *htim);
void WS2812_SetPixel(uint8_t r, uint8_t g, uint8_t b);

#endif /* INC_ENGREDU_WS2812_H_ */
