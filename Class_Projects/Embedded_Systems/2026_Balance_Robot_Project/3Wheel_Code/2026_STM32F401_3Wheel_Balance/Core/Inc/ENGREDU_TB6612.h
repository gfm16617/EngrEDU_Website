/*
 * ENGREDU_TB6612.h
 *
 *  Created on: Apr 18, 2026
 *      Author: gfm
 */

#ifndef INC_ENGREDU_TB6612_H_
#define INC_ENGREDU_TB6612_H_

//Header Files
#include "stm32f4xx_hal.h"  //depending on your board

//TypeDefs and Enums
//1- TB6612 Configuration
typedef struct {
    GPIO_TypeDef *IN1_Port;
    uint16_t IN1_Pin;

    GPIO_TypeDef *IN2_Port;
    uint16_t IN2_Pin;

    TIM_HandleTypeDef *htim;
    uint32_t Channel;

    GPIO_TypeDef *STBY_Port;
    uint16_t STBY_Pin;

    int Offset;   // +1 or -1
} TB6612_Motor;

//Function Prototype
//1- Init Function
void TB6612_Init(TB6612_Motor *m);
//2- Forward
void TB6612_Forward(TB6612_Motor *m, uint16_t speed);
//3- Reverse
void TB6612_Reverse(TB6612_Motor *m, uint16_t speed);
//4- Drive
void TB6612_Drive(TB6612_Motor *m, int speed);
//5- Brake
void TB6612_Brake(TB6612_Motor *m);
//6- Standby
void TB6612_Standby(TB6612_Motor *m);
//7- Forward Both
void TB6612_Forward_Both(TB6612_Motor *m1, TB6612_Motor *m2, int speed);
//8- Reverse Both
void TB6612_Reverse_Both(TB6612_Motor *m1, TB6612_Motor *m2, int speed);
//9- Left
void TB6612_Left(TB6612_Motor *left, TB6612_Motor *right, int speed);
//10- Right
void TB6612_Right(TB6612_Motor *left, TB6612_Motor *right, int speed);


#endif /* INC_ENGREDU_TB6612_H_ */
