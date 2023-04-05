#include <Arduino.h>
#include <avr/power.h>

#include "ATMEGA32u4_Pin_Mapping.h"

#define PF0   A5
#define PF1   A4
#define PF4   A3
#define PF5   A2
#define PF6   A1
#define PF7   A0

#define PC7   13
#define PC6   5
#define PB6   10
#define PB5   9
#define PB4   8
#define PD7   6
#define PD6   12
#define PD4   4

#define PD5   NotMappedPort_PD5   // Not Mapped on Leonardo Board
#define PD3   1   // TX
#define PD2   0   // RX
#define PD1   2
#define PD0   3
#define PB7   11

#define PB3   NotMappedPort_PB3   // MISO -> Not Mapped on Leonardo Board
#define PB2   NotMappedPort_PB2   // MOSI -> Not Mapped on Leonardo Board
#define PB1   NotMappedPort_PB1   // SCK -> Not Mapped on Leonardo Board
#define PB0   NotMappedPort_PB0   // SS -> Not Mapped on Leonardo Board
#define PE6   7

#define SLEEP_TIME  200

void setup() {
  // put your setup code here, to run once:
  pinMode(PD4, OUTPUT);
  pinMode(PD7, OUTPUT);

  pinMode(PD2, OUTPUT);

  /* Non Leonardo Pin */
  //DDRD = DDRD | B00100000;  // PD5 as Output
  my_pinMode(PD5, OUTPUT);
  //DDRB = DDRB | B00001111;  // PB3 to PB0 as Output
  my_pinMode(PB0, OUTPUT);
  my_pinMode(PB1, OUTPUT);
  my_pinMode(PB2, OUTPUT);
  my_pinMode(PB3, OUTPUT);

  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);

  //Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PD4, HIGH);
  digitalWrite(PD7, LOW);
  /* Testing Code */
  digitalWrite(PD2, LOW);
  //PORTD = PORTD & B11011111;  // PD5 - LOW
  my_digitalWrite(PD5, LOW);
  //PORTB = PORTB & B11110000;  // PB3 to PB0 - LOW
  my_digitalWrite(PB0, LOW);
  my_digitalWrite(PB1, LOW);
  my_digitalWrite(PB2, LOW);
  my_digitalWrite(PB3, LOW);
  /* ------------ */
  delay(SLEEP_TIME);
  digitalWrite(PD4, LOW);
  digitalWrite(PD7, HIGH);
  /* Testing Code */
  digitalWrite(PD2, HIGH);
  //PORTD = PORTD | B00100000;  // PD5 - HIGH
  my_digitalWrite(PD5, HIGH);
  //PORTB = PORTB | B00001111;  // PB3 to PB0 - HIGH
  my_digitalWrite(PB0, HIGH);
  my_digitalWrite(PB1, HIGH);
  my_digitalWrite(PB2, HIGH);
  my_digitalWrite(PB3, HIGH);
  /* ------------ */
  delay(SLEEP_TIME);

  //Serial.println("Working");
}
