#include <Arduino.h>
#include <avr/power.h>

#define BUTTON_1  2
#define BUTTON_2  3
#define GREEN_LED 4
#define RED_LED   5

#define DATA      9
#define LATCH     8
#define CLOCK     7

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13
 
unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};

void setup() {

  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
  
  // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, CHANGE);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, CHANGE);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  disp_on();
}

void Display(unsigned char num)
{

  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num]);
  digitalWrite(LATCH, HIGH);
}

void Display_Numbers()
{
  unsigned char sleep_time = 500;
  
  Display(0);
  delay(sleep_time);
  Display(1);
  delay(sleep_time);
  Display(2);
  delay(sleep_time);
  Display(3);
  delay(sleep_time);
  Display(4);
  delay(sleep_time);
  Display(5);
  delay(sleep_time);
  Display(6);
  delay(sleep_time);
  Display(7);
  delay(sleep_time);
  Display(8);
  delay(sleep_time);
  Display(9);
  delay(sleep_time);
  Display(10);
  delay(sleep_time);
  Display(11);
  delay(sleep_time);
  Display(12);
  delay(sleep_time);
  Display(13);
  delay(sleep_time);
  Display(14);
  delay(sleep_time);
  Display(15);
  delay(sleep_time);
  Display(16);
  delay(sleep_time);
}

void disp_on()
{
   digitalWrite(DIGIT_1, LOW);
   digitalWrite(DIGIT_2, LOW);
   digitalWrite(DIGIT_3, LOW);
   digitalWrite(DIGIT_4, LOW);
}

void Button_1_ISR()
{
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
    
  buttonState = digitalRead(BUTTON_1);
  digitalWrite(RED_LED, buttonState);
}

void Button_2_ISR()
{ 
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
   
  buttonState = digitalRead(BUTTON_2);
  digitalWrite(GREEN_LED, buttonState);
}

void loop() {
  Display_Numbers();
}
