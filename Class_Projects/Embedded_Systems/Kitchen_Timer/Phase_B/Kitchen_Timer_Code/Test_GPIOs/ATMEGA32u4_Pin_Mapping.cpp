/*
 * ATMEGA32U4_Pin_Mapping.c
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

#include "ATMEGA32u4_Pin_Mapping.h"

void my_pinMode(NotMappedPort_e NotMappedPort, uint8_t mode)
{
  switch(NotMappedPort)
  {
    case PB0:
      if(mode == OUTPUT)
        DDRB = DDRB | B00000001;  // PD5 as Output
      else
        DDRB = DDRB & B11111110;  // PD5 as Input
      break;
      
    case PB1:
      if(mode == OUTPUT)
        DDRB = DDRB | B00000010;  // PD5 as Output
      else
        DDRB = DDRB & B11111101;  // PD5 as Input
      break;
      
    case PB2:
      if(mode == OUTPUT)
        DDRB = DDRB | B00000100;  // PD5 as Output
      else
        DDRB = DDRB & B11111011;  // PD5 as Input
      break;
      
    case PB3:
      if(mode == OUTPUT)
        DDRB = DDRB | B00001000;  // PD5 as Output
      else
        DDRB = DDRB & B11110111;  // PD5 as Input
      break;
      
    case PD5:
      if(mode == OUTPUT)
        DDRD = DDRD | B00100000;  // PD5 as Output
      else
        DDRD = DDRD & B11011111;  // PD5 as Input
      break;
  }
}

void my_digitalWrite(NotMappedPort_e NotMappedPort, uint8_t val)
{
  switch(NotMappedPort)
  {
    case PB0:
      if(val == HIGH)
        PORTB = PORTB | B00000001;  // PB0 - HIGH
      else
        PORTB = PORTB & B11111110;  // PB0 - LOW
      break;
      
    case PB1:
      if(val == HIGH)
        PORTB = PORTB | B00000010;  // PB1 - HIGH
      else
        PORTB = PORTB & B11111101;  // PB1 - LOW
      break;
      
    case PB2:
      if(val == HIGH)
        PORTB = PORTB | B00000100;  // PB2 - HIGH
      else
        PORTB = PORTB & B11111011;  // PB2 - LOW
      break;
      
    case PB3:
      if(val == HIGH)
        PORTB = PORTB | B00001000;  // PB3 - HIGH
      else
        PORTB = PORTB & B11110111;  // PB3 - LOW
      break;
      
    case PD5:
      if(val == HIGH)
        PORTD = PORTD | B00100000;  // PD5 - HIGH
      else
        PORTD = PORTD & B11011111;  // PD5 - LOW
      break;
  }
}
