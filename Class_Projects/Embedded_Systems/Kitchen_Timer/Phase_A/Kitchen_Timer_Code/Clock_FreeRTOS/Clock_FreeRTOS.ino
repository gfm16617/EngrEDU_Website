// Include Arduino FreeRTOS library
#include <Arduino_FreeRTOS.h>

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

// Timer Variables
#define DEFAULT_COUNT 30     // default value is 30secs
int  count = DEFAULT_COUNT;

char buzzer_flag = 0;
unsigned char timer_running = 0;

//define task handles
TaskHandle_t TaskClockTimer_Handler;

void setup() {

  /**
   * Set up Pins
   */
   // LEDs Pins
   pinMode(RED_LED, OUTPUT);
   pinMode(GREEN_LED, OUTPUT);

   // LEDs -> Timer Stopped
   digitalWrite(RED_LED, HIGH);
   digitalWrite(GREEN_LED, HIGH);
  
   // Button Pins
   pinMode(BUTTON_1, INPUT);
   pinMode(BUTTON_2, INPUT);
  
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

  // 7-Seg Display Task -> Running time 5 ms
  xTaskCreate(TaskDisplay, // Task function
              "Display", // A name just for humans
              128,  // This stack size can be checked & adjusted by reading the Stack Highwater
              NULL, 
              0, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
              NULL);

  // Clock timer Task -> Running time 1 second
  xTaskCreate(TaskClockTimer, // Task function
              "ClockTimer", // Task name
              128,  // Stack size
              NULL, 
              0, // Priority
              &TaskClockTimer_Handler);

  stop_TaskClockTimer();

  // Buzzer Task -> Running at 250 ms
  xTaskCreate(TaskBuzzer, // Task function
              "Buzzer", // Task name
              128, // Stack size 
              NULL, 
              0, // Priority
              NULL );

  // Read Button 2 Task -> Running at 10 ms
  xTaskCreate(TaskReadButton2, // Task function
              "ReadButton2", // Task name
              128,  // Stack size
              NULL, 
              0, // Priority
              NULL);

  // Read Button 1 Task -> Running at 10 ms
  xTaskCreate(TaskReadButton1, // Task function
              "ReadButton1", // Task name
              128,  // Stack size
              NULL, 
              0, // Priority
              NULL);
}

void loop() {}

// ------
// TASKS
// ------

/**
 * 7-Seg Display Task
 * Refresh display -> Running at 5 ms
 */
void TaskDisplay(void * pvParameters) {
  (void) pvParameters;

  byte current_digit;
  
  for (;;) 
  {
    disp_off();  // turn off the display
   
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

    // 5 ms
    vTaskDelay( 5 / portTICK_PERIOD_MS );
  }
}

/**
 * Clock Timer Task
 * If timer is on, decrements timer by one second.
 */
void TaskClockTimer(void *pvParameters)
{
  (void) pvParameters;
  
  for (;;)
  {
    count--;

    if(count == 0)
    { 
      //raise alarm
      buzzer_flag = 1;
      timer_running = 0;

      //stop timer
      stop_TaskClockTimer();
    }

    // 1 second
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
  }
}

/* 
 * Buzzer Task 
 */
void TaskBuzzer(void *pvParameters)
{
  (void) pvParameters;

  unsigned char state = 0;

  for (;;)
  {
    // Attend buzzer_flag
    if(buzzer_flag == 1)
    {      
      // Make Noise...
      digitalWrite(RED_LED, HIGH);
      digitalWrite(GREEN_LED, LOW);
      
      digitalWrite(BUZZER, HIGH);
      vTaskDelay( 250 / portTICK_PERIOD_MS );
      digitalWrite(BUZZER, LOW);
      vTaskDelay( 250 / portTICK_PERIOD_MS );
    }
    else
      digitalWrite(BUZZER, LOW);

    // One tick delay (15ms) in between reads for stability
    vTaskDelay(1);
  }
}

/**
 * Read Button 2 Task
 * Reads button 2, Inc timer
 */
void TaskReadButton2(void *pvParameters)
{
  (void) pvParameters;
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
  
  for (;;)
  { 
    // Read Button 2 -> pooling method is fine in this case
    buttonState = digitalRead(BUTTON_2);

    // TODO -> implement the code to debounce the button
    if(buttonState == 1)
    {
      vTaskDelay( 250 / portTICK_PERIOD_MS );  // Very simple debounce
      
      count++;
    }

    // One tick delay (15ms) in between reads for stability
    vTaskDelay(1);
  }
}

/**
 * Read Button 1 Task
 * Reads button 1, Start/Stop timer
 */
void TaskReadButton1(void *pvParameters)
{
  (void) pvParameters;

  unsigned int buttonState = 0;  // variable for reading the pushbutton status
  
  for (;;)
  { 
    // Read Button 1 -> pooling method is fine in this case
    buttonState = digitalRead(BUTTON_1);

    if(buttonState == 1)
    {
      vTaskDelay( 250 / portTICK_PERIOD_MS );  // Very simple debounce
      
      if(timer_running == 0)
      {
        // Start Clock Timer
        timer_running = 1;
  
        if(count == 0)
        {
          count = DEFAULT_COUNT;
        }
  
        if(buzzer_flag == 1)
        {
          buzzer_flag = 0;
  
          // LEDs -> Timer Stopped
          digitalWrite(RED_LED, HIGH);
          digitalWrite(GREEN_LED, HIGH);
        }
        else
        {
          start_TaskClockTimer();
          // LEDs -> Timer Running
          digitalWrite(RED_LED, LOW);
          digitalWrite(GREEN_LED, HIGH);
        }
      }
      else
      {
        // Stop Timer
        stop_TaskClockTimer();
        timer_running = 0;
  
        // LEDs -> Timer Running
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, HIGH);
      }
    }

    // One tick delay (15ms) in between reads for stability
    vTaskDelay(1);
  }
}

// --------------------------------------------------

void stop_TaskClockTimer()
{
  vTaskSuspend(TaskClockTimer_Handler);
}

void start_TaskClockTimer()
{
  vTaskResume(TaskClockTimer_Handler);
}

void disp_off()
{
   digitalWrite(DIGIT_1, HIGH);
   digitalWrite(DIGIT_2, HIGH);
   digitalWrite(DIGIT_3, HIGH);
   digitalWrite(DIGIT_4, HIGH);
}

void Display(unsigned char num, unsigned char dp)
{
  digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, table[num] | (dp<<7));
  digitalWrite(LATCH, HIGH);
}
