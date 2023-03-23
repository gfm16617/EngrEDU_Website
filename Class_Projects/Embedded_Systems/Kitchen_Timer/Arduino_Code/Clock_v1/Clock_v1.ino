// https://simple-circuit.com/arduino-7-segment-74hc595-shift-register/
// https://community.robotshop.com/forum/t/arduino-101-timers-and-interrupts/13072
// https://www.arduino.cc/reference/de/language/functions/external-interrupts/attachinterrupt/
// https://microcontrollerslab.com/arduino-timer-interrupts-tutorial/

#define BUTTON_1  2
#define BUTTON_2  3
#define GREEN_LED 4
#define RED_LED   5
//#define BUZZER    6

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

unsigned int buttonState = 0;  // variable for reading the pushbutton status

volatile unsigned char isr_1_flag = 0;
volatile unsigned char isr_2_flag = 0;

// variable declarations
byte current_digit;
volatile int  count = 0;

unsigned int reload_timer_1 = 62500;
byte reload_timer_2 = 10;

void setup() {
  // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, CHANGE);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, CHANGE);

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

  disp_off();  // turn off the display

  // Initialize Timer1 (16bit) -> Used for clock
  // Speed of Timer1 = 16MHz/256 = 62.5 KHz
  noInterrupts();         // disable all interrupts  
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = reload_timer_1; // compare match register 16MHz/256
  TCCR1B |= (1<<WGM12);   // CTC mode
  TCCR1B |= (1<<CS12);    // 256 prescaler 
  TIMSK1 |= (1<<OCIE1A);  // enable timer compare interrupt

  // Initialize Timer2 (8bit) -> Used to refresh display
  // Speed of Timer2 = 16MHz/1024 = 15.625 KHz
  TCCR2A = 0;
  TCCR2B = 0;
  OCR2A = reload_timer_2;                     // max value 2^8 - 1 = 255
  TCCR2A |= (1<<WGM21);
  TCCR2B = (1<<CS22) | (1<<CS21) | (1<<CS20); // 1204 prescaler
  TIMSK2 |= (1<<OCIE2A);
  interrupts();                               // enable all interrupts
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
  if(count == 16)
  {
    count = 0;
  }
}

void disp_off()
{
   digitalWrite(DIGIT_1, HIGH);
   digitalWrite(DIGIT_2, HIGH);
   digitalWrite(DIGIT_3, HIGH);
   digitalWrite(DIGIT_4, HIGH);
}

void Button_1_ISR()
{
  // Set ISR Flag
  isr_1_flag = 1;
}

void Button_2_ISR()
{ 
  // Set ISR Flag
  isr_2_flag = 1;
}

ISR(TIMER2_COMPA_vect)   // Timer1 interrupt service routine (ISR)
{
  disp_off();  // turn off the display
  OCR2A = reload_timer_2;
 
  switch (current_digit)
  {
    case 1:
      Display(count / 1000);   // prepare to display digit 1 (most left)
      digitalWrite(DIGIT_1, LOW);  // turn on digit 1
      break;
 
    case 2:
      Display( (count / 100) % 10 );   // prepare to display digit 2
      digitalWrite(DIGIT_2, LOW);     // turn on digit 2
      break;
 
    case 3:
      Display( (count / 10) % 10 );   // prepare to display digit 3
      digitalWrite(DIGIT_3, LOW);    // turn on digit 3
      break;
 
    case 4:
      Display(count % 10); // prepare to display digit 4 (most right)
      digitalWrite(DIGIT_4, LOW);  // turn on digit 4
  }
 
  current_digit = (current_digit % 4) + 1;
}

ISR(TIMER1_COMPA_vect)
{
  count++;
  OCR1A = reload_timer_1;

  if(count > 9999)
      count = 0;
}

void loop() 
{
  // Attend Button 1
  if(isr_1_flag == 1)
  {
    // Reset ISR Flag
    isr_1_flag = 0;

    // Code
    buttonState = digitalRead(BUTTON_1);
    digitalWrite(RED_LED, buttonState);

    // Increment Clock
    count++;
  }

  // Attend Button 2
  if(isr_2_flag == 1)
  {
    // Reset ISR Flag
    isr_2_flag = 0;

    // Code
    buttonState = digitalRead(BUTTON_2);
    digitalWrite(GREEN_LED, buttonState);

    // Reset Clock
    count = 0;
    TCCR1B = 0; // stop clock
    TIMSK1 = 0; // cancel clock timer interrupt  
  }
}
