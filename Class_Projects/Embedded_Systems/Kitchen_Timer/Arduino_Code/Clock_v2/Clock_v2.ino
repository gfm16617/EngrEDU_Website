// https://simple-circuit.com/arduino-7-segment-74hc595-shift-register/
// https://community.robotshop.com/forum/t/arduino-101-timers-and-interrupts/13072
// https://www.arduino.cc/reference/de/language/functions/external-interrupts/attachinterrupt/
// https://microcontrollerslab.com/arduino-timer-interrupts-tutorial/

#define BUTTON_2  2  // Inc Timer
#define BUTTON_1  3  // Start/Stop Timer and Stop Buzzer
#define GREEN_LED 4
#define RED_LED   5
#define BUZZER    6

#define DATA      9 // DS
#define LATCH     8 // ST_CP
#define CLOCK     7 // SH_CP

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13

// 7-Seg Display Variables
unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};
byte current_digit;

// Volatile Variables
volatile unsigned char isr_1_flag = 0;
volatile unsigned char buzzer_flag = 0;

// Timer Variables
#define DEFAULT_COUNT 30     // default value is 30secs
volatile int  count = DEFAULT_COUNT;
unsigned char timer_running = 0; 

unsigned int reload_timer_1 = 62500; // corresponds to 1 second
byte reload_timer_2 = 10;  // display refresh time

void setup() {
  // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // LEDs -> Timer Stopped
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);

  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, RISING);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, RISING);

  // Buzer Pins
  pinMode(BUZZER, OUTPUT);

  // Buzzer -> Off
  digitalWrite(BUZZER,LOW);

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
  //startTimer1(); // this is done using the start button

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

void Display(unsigned char num, unsigned char dp)
{
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num] | (dp<<7));
  digitalWrite(LATCH, HIGH);
}

void disp_off()
{
   digitalWrite(DIGIT_1, HIGH);
   digitalWrite(DIGIT_2, HIGH);
   digitalWrite(DIGIT_3, HIGH);
   digitalWrite(DIGIT_4, HIGH);
}

void Button_2_ISR()
{
  // Increment Clock
  count++;
}

void Button_1_ISR()
{ 
  // Set ISR Flag
  isr_1_flag = 1;
}

ISR(TIMER2_COMPA_vect)   // Timer2 interrupt service routine (ISR)
{
  disp_off();  // turn off the display
  OCR2A = reload_timer_2;
 
  switch (current_digit)
  {
    case 1: //0x:xx
      Display( int((count/60) / 10) % 6, 0 );   // prepare to display digit 1 (most left)
      digitalWrite(DIGIT_1, LOW);  // turn on digit 1
      break;
 
    case 2: //x0:xx
      Display( int(count / 60) % 10, 1 );   // prepare to display digit 2
      digitalWrite(DIGIT_2, LOW);     // turn on digit 2
      break;
 
    case 3: //xx:0x
      Display( (count / 10) % 6, 0 );   // prepare to display digit 3
      digitalWrite(DIGIT_3, LOW);    // turn on digit 3
      break;
 
    case 4: //xx:x0
      Display(count % 10, 0); // prepare to display digit 4 (most right)
      digitalWrite(DIGIT_4, LOW);  // turn on digit 4
  }
 
  current_digit = (current_digit % 4) + 1;
}

ISR(TIMER1_COMPA_vect)  // Timer1 interrupt service routine (ISR)
{
  count--;
  OCR1A = reload_timer_1;

  if(count == 0)
  {
      // Stop Timer
      stopTimer1();
      
      // Raise Alarm
      buzzer_flag = 1;
      timer_running = 0;
  }
}

void stopTimer1()
{
  // Stop Timer
  TCCR1B = 0; // stop clock
  TIMSK1 = 0; // cancel clock timer interrupt
}

void startTimer1()
{
  // Initialize Timer1 (16bit) -> Used for clock
  // Speed of Timer1 = 16MHz/256 = 62.5 KHz
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = reload_timer_1; // compare match register 16MHz/256
  TCCR1B |= (1<<WGM12);   // CTC mode
  TCCR1B |= (1<<CS12);    // 256 prescaler 
  TIMSK1 |= (1<<OCIE1A);  // enable timer compare interrupt
  interrupts();
}

void Active_Buzzer()
{
  unsigned char i;
  unsigned char sleep_time = 1; // ms
  
  for(i=0;i<100;i++)
   {
    digitalWrite(BUZZER,HIGH);
    delay(sleep_time);//wait for 1ms
    digitalWrite(BUZZER,LOW);
    delay(sleep_time);//wait for 1ms
    }
}

void loop() 
{
  // Attend Button 2 ISR
  if(isr_1_flag == 1)
  {
    // Reset ISR Flag
    isr_1_flag = 0;

    if(timer_running == 0)
    {
      // Start Timer
      timer_running = 1;

      if(count == 0)
        count = DEFAULT_COUNT;

      if(buzzer_flag == 1)
      {
        buzzer_flag = 0;

        // LEDs -> Timer Stopped
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, HIGH);
      }
      else
      {
        startTimer1();
        // LEDs -> Timer Running
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
      }
    }
    else
    {
      // Stop Timer
      stopTimer1();
      timer_running = 0;

      // LEDs -> Timer Running
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, HIGH);
    }
  }

  // Attend buzzer_flag
  if(buzzer_flag == 1)
  {
    // Make Noise...
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    Active_Buzzer();
  }
}
