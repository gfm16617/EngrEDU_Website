/*
 * ENGREDU_TB6612.c
 *
 *  Created on: Apr 18, 2026
 *      Author: gfm
 */

//Header files
#include "ENGREDU_TB6612.h"

//Function Definitions
//1- Init Function
void TB6612_Init(TB6612_Motor *m)
{
    // Set standby HIGH (enable driver)
    HAL_GPIO_WritePin(m->STBY_Port, m->STBY_Pin, GPIO_PIN_SET);

    // Start PWM
    HAL_TIM_PWM_Start(m->htim, m->Channel);
}
//2- Forward
void TB6612_Forward(TB6612_Motor *m, uint16_t speed)
{
    HAL_GPIO_WritePin(m->IN1_Port, m->IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(m->IN2_Port, m->IN2_Pin, GPIO_PIN_RESET);

    __HAL_TIM_SET_COMPARE(m->htim, m->Channel, speed);
}
//3- Reverse
void TB6612_Reverse(TB6612_Motor *m, uint16_t speed)
{
    HAL_GPIO_WritePin(m->IN1_Port, m->IN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(m->IN2_Port, m->IN2_Pin, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(m->htim, m->Channel, speed);
}
//4- Drive
void TB6612_Drive(TB6612_Motor *m, int speed)
{
    speed *= m->Offset;

    if (speed >= 0)
        TB6612_Forward(m, speed);
    else
        TB6612_Reverse(m, -speed);
}
//5- Brake
void TB6612_Brake(TB6612_Motor *m)
{
    HAL_GPIO_WritePin(m->IN1_Port, m->IN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(m->IN2_Port, m->IN2_Pin, GPIO_PIN_SET);

    __HAL_TIM_SET_COMPARE(m->htim, m->Channel, 0);
}
//6- Standby
void TB6612_Standby(TB6612_Motor *m)
{
    HAL_GPIO_WritePin(m->STBY_Port, m->STBY_Pin, GPIO_PIN_RESET);
}
//7- Forward Both
void TB6612_Forward_Both(TB6612_Motor *m1, TB6612_Motor *m2, int speed)
{
    TB6612_Drive(m1, speed);
    TB6612_Drive(m2, speed);
}
//8- Reverse Both
void TB6612_Reverse_Both(TB6612_Motor *m1, TB6612_Motor *m2, int speed)
{
    TB6612_Drive(m1, -speed);
    TB6612_Drive(m2, -speed);
}
//9- Left
void TB6612_Left(TB6612_Motor *left, TB6612_Motor *right, int speed)
{
    int temp = speed / 2;
    TB6612_Drive(left, -temp);
    TB6612_Drive(right, temp);
}
//10- Right
void TB6612_Right(TB6612_Motor *left, TB6612_Motor *right, int speed)
{
    int temp = speed / 2;
    TB6612_Drive(left, temp);
    TB6612_Drive(right, -temp);
}

