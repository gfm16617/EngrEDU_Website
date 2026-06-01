/*
 * WS2812.c
 *
 *  Created on: May 10, 2026
 *      Author: gfm
 */
#include <ENGREDU_WS2812.h>

//extern TIM_HandleTypeDef htim5;
TIM_HandleTypeDef *ghtim;

#define LED_HIGH 70
#define LED_LOW  35

uint32_t pwmData[24 + 50];
uint8_t ledData[3];

void WS2812_Init(TIM_HandleTypeDef *htim){
	ghtim = htim;
}

void WS2812_SetPixel(uint8_t r, uint8_t g, uint8_t b)
{
    // WS2812 expects GRB order
    ledData[0] = g;
    ledData[1] = r;
    ledData[2] = b;

    uint32_t indx = 0;

    for(int i = 0; i < 3; i++)
    {
        for(int bit = 7; bit >= 0; bit--)
        {
            if(ledData[i] & (1 << bit))
            {
                pwmData[indx] = LED_HIGH;
            }
            else
            {
                pwmData[indx] = LED_LOW;
            }

            indx++;
        }
    }

    // Reset pulse (>50us low)
    for(int i = 0; i < 50; i++)
    {
        pwmData[indx++] = 0;
    }

    HAL_TIM_PWM_Start_DMA(ghtim,
                           TIM_CHANNEL_1,
                           pwmData,
                           indx);
}

