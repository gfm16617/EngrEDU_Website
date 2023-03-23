#define BUTTON_1  2
#define BUTTON_2  3
#define GREEN_LED 4
#define RED_LED   5
//#define BUZZER    6

#define DATA      9   //74HC595  pin 8 DS
#define LATCH     8   //74HC595  pin 9 STCP
#define CLOCK     7  //74HC595  pin 10 SHCP

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13
 
unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};

volatile unsigned char isr_1_flag = 0;
volatile unsigned char isr_2_flag = 0;
unsigned char count = 0;

const int debounceTime = 150;  // debounce in milliseconds

void setup() {
  // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, FALLING);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, FALLING);

  // Buzer Pins
  //pinMode(BUZZER, OUTPUT);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);
}

void Display(unsigned char num)
{
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num]);
  digitalWrite(LATCH, HIGH);
}

void Display_Numbers()
{
  Display(count);

  // Reset Counter
  if(count>=16)
  {
    count = 0;
  }
}

void Button_1_ISR()
{
  isr_1_flag = 1;
}

void Button_2_ISR()
{ 
  isr_2_flag = 1;
}

void deBounce()
{
  unsigned long now = millis ();
  do
  {
    // on bounce, reset time-out
    if (digitalRead(BUTTON_1) == HIGH)
      now = millis ();
  } 
  while (digitalRead(BUTTON_1) == HIGH || (millis () - now) <= debounceTime);

}  // end of deBounce

void loop() {
  unsigned int buttonState = 0;  // variable for reading the pushbutton status

  if(isr_1_flag == 1)
  {
    isr_1_flag = 0;

    digitalWrite(RED_LED, HIGH);
    deBounce();
    buttonState = digitalRead(BUTTON_1);
    digitalWrite(RED_LED, LOW);
    
    count++;
    Display_Numbers();  
  }

  if(isr_2_flag == 1)
  {
    isr_2_flag = 0;

    buttonState = digitalRead(BUTTON_2);
    digitalWrite(GREEN_LED, buttonState);
  }
}
