#include <Arduino.h>
#include <avr/power.h>

#define PF0   A5
#define PF1   A4
#define PF4   A3
#define PF5   A2
#define PF6   A1
#define PF7   A0

#define PC7   13
#define PC6   5   // RED LED
#define PB6   10
#define PB5   9
#define PB4   8
#define PD7   6
#define PD6   12
#define PD4   4   // GREEN LED

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

#define SLEEP_TIME  500

void setup() {
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
  
  pinMode(PD4, OUTPUT);
  pinMode(PC6, OUTPUT);  
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PD4, HIGH);
  digitalWrite(PC6, LOW);
  delay(SLEEP_TIME);
  digitalWrite(PD4, LOW);
  digitalWrite(PC6, HIGH);
  delay(SLEEP_TIME);
}
