#include <Arduino_FreeRTOS.h>

#define GREEN_LED   4
#define RED_LED     5

//define task handles
TaskHandle_t TaskBlink_RED_Handler;
TaskHandle_t TaskBlink_GREEN_Handler;
TaskHandle_t TaskSerial_Handler;

// define two tasks for Blink & Serial
void TaskBlink_RED( void *pvParameters );
void TaskBlink_GREEN( void *pvParameters );
void TaskSerial(void* pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }
  
  // Now set up two tasks to run independently.
   xTaskCreate(
    TaskBlink_RED
    ,  "Blink Red"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL //Parameters passed to the task function
    ,  2  // Priority, with 2 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &TaskBlink_RED_Handler );//Task handle

  xTaskCreate(
    TaskBlink_GREEN
    ,  "Blink Green"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL //Parameters passed to the task function
    ,  2  // Priority, with 2 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  &TaskBlink_GREEN_Handler );//Task handle

   xTaskCreate(
    TaskSerial
    ,  "Serial"
    ,  128  // Stack size
    ,  NULL //Parameters passed to the task function
    ,  1  // Priority
    ,  &TaskSerial_Handler );  //Task handle
}
    

void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskSerial(void* pvParameters){
/*
 Serial
 Send "s" or "r" through the serial port to control the suspend and resume of the LED light task.
 This example code is in the public domain.
*/
  (void) pvParameters;
   for (;;) // A Task shall never return or exit.
   {
    while(Serial.available()>0){
      switch(Serial.read()){
        case 's':
          vTaskSuspend(TaskBlink_RED_Handler); 
          Serial.println("Suspend!");
          break;
        case 'r':
          vTaskResume(TaskBlink_RED_Handler);
          Serial.println("Resume!");
          break;
      }
      vTaskDelay(1);
    }
   }
}

void TaskBlink_GREEN(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  pinMode(GREEN_LED, OUTPUT);
  for (;;) // A Task shall never return or exit.
  {
    //Serial.println(11);
    digitalWrite(GREEN_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 200 / portTICK_PERIOD_MS ); // wait for one second
    digitalWrite(GREEN_LED, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 200 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void TaskBlink_RED(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  pinMode(RED_LED, OUTPUT);
  for (;;) // A Task shall never return or exit.
  {
    //Serial.println(11);
    digitalWrite(RED_LED, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for one second
    digitalWrite(RED_LED, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 500 / portTICK_PERIOD_MS ); // wait for one second
  }
}
