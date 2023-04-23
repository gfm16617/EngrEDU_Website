/*
 * ATMEGA32U4_Pin_Mapping.h
 * 
 * Created on: 29 Mar 2023
 *      Author: Goncalo Martins
 *      
 * Description:
 * 
 * Copyright © 2023 - Goncalo Martins, EngrEdu
 *   This is a free software under the GNU license, you can redistribute it and/or modify it under the
 *   terms of the GNU General Public License V3.0 as published by the Free Software Foundation.
 * 
 *   This software library is shared with public for educational purposes, without warranty and author
 *   is not liable for any damages caused directly or indirectly by this software, read more about this
 *   on the GNU General Public License.
*/
#ifndef ATMEGA32U4_PIN_MAPPING_H_
#define ATMEGA32U4_PIN_MAPPING_H_

#include <Arduino.h>

typedef enum 
{
  NotMappedPort_PB0 = 0,
  NotMappedPort_PB1,
  NotMappedPort_PB2,
  NotMappedPort_PB3,
  NotMappedPort_PD5
} NotMappedPort_e;

/** 
 * @brief Set Pin Mode for Not Mapped Pins 
 * @param NotMappedPort 
 * @param mode
 * @return  
 */
void my_pinMode(NotMappedPort_e NotMappedPort, uint8_t mode);

/** 
 * @brief Set Not Mapped Pins High or Low
 * @param NotMappedPort 
 * @param val
 * @return  
 */
void my_digitalWrite(NotMappedPort_e NotMappedPort, uint8_t val);

#endif /* ATMEGA32U4_PIN_MAPPING_H_ */
