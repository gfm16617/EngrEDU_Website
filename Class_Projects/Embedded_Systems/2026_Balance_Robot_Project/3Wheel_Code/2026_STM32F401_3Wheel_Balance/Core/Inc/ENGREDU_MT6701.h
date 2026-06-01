/*
 * ENGREDU_MT6701.h
 *
 *  Created on: Apr 18, 2026
 *      Author: gfm
 */

#ifndef INC_ENGREDU_MT6701_H_
#define INC_ENGREDU_MT6701_H_

//Header Files
#include "stm32f4xx_hal.h"  //dpending on your board
#include <string.h>

//Define Registers
#define MT6701_ADDR					0x06

#define MT6701_REG_04_ANGLE			0x04
#define MT6701_REG_03_ANGLE			0x03

typedef struct {
    I2C_HandleTypeDef *hi2c;

    uint16_t raw;
    float angle_deg;

    float angle_unwrapped;
    float angle_filtered;

    float velocity_deg_s;

    float prev_angle;
    float prev_unwrapped;

    float zero_angle;          // reference (deg)
    float relative_angle;      // wrapped 0–360
    float relative_unwrapped;  // continuous relative

    uint8_t initialized;

} MT6701_Handle;

//Function Prototype
//1-
void MT6701_Init(MT6701_Handle *h, I2C_HandleTypeDef *hi2c);
//2-
uint16_t MT6701_ReadRaw(MT6701_Handle *h);
//3-
void MT6701_Update(MT6701_Handle *h, float dt);
//4-
void MT6701_SetZero(MT6701_Handle *h);


#endif /* INC_ENGREDU_MT6701_H_ */
