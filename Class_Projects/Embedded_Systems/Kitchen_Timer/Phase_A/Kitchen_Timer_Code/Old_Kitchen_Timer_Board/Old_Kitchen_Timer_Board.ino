#include <avr/io.h>

#define LED       3
#define BUZZER    2
#define BUTTON_B  5
#define BUTTON_A  13

#define COM1 0
//#define COM2 22   // Not defined in the board
#define COM3 12
#define COM4 A5

#define SEGMENT_DP  A2
#define SEGMENT_A   1
#define SEGMENT_B   6
#define SEGMENT_C   A1
#define SEGMENT_D   A3
#define SEGMENT_E   A4
#define SEGMENT_F   4
#define SEGMENT_G   A0

unsigned char button_state_A;
unsigned char button_state_B;

void setup() {

  // Disable JTAG
  MCUCR = (1<<JTD); // Disable JTAG
  MCUCR = (1<<JTD); // Disable JTAG

  //DDRE = 0;
  //PRR1 = PRR1 | 0b00000001;
  UCSR1B = UCSR1B & 0b11101111;
  
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_A, INPUT_PULLUP);

  pinMode(COM1, OUTPUT);
  //pinMode(COM2, OUTPUT);
  DDRD = DDRD | B00100000; // PD5 as Output 
  pinMode(COM3, OUTPUT);
  pinMode(COM4, OUTPUT); 

  pinMode(SEGMENT_DP, OUTPUT);
  pinMode(SEGMENT_A, OUTPUT);
  pinMode(SEGMENT_B, OUTPUT);
  pinMode(SEGMENT_C, OUTPUT);
  pinMode(SEGMENT_D, OUTPUT);
  pinMode(SEGMENT_E, OUTPUT);
  pinMode(SEGMENT_F, OUTPUT);
  pinMode(SEGMENT_G, OUTPUT);
}

void enable_digit_x(int digit) 
{
  switch (digit) 
  {
    case 1:
      digitalWrite(COM1, HIGH);
      //PORTD = B00100000;      // COM2 - HIGH
      PORTD = B00000000;        // COM2 - LOW
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, LOW);
      break;
      
    case 2:
      digitalWrite(COM1, LOW);
      PORTD = B00100000;      // COM2 - HIGH
      //PORTD = B00000000;        // COM2 - LOW
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, LOW);
      break;
      
    case 3:
      digitalWrite(COM1, LOW);
      //PORTD = B00100000;      // COM2 - HIGH
      PORTD = B00000000;        // COM2 - LOW
      digitalWrite(COM3, HIGH);
      digitalWrite(COM4, LOW);
      break;
      
    case 4:
      digitalWrite(COM1, LOW);
      //PORTD = B00100000;      // COM2 - HIGH
      PORTD = B00000000;        // COM2 - LOW
      digitalWrite(COM3, LOW);
      digitalWrite(COM4, HIGH);
      break;
    
  }
}

void set_digit(int number)
{
   // The following switch statement turns on the correct segments for the number to show
  switch(number) {
    case 0 :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, LOW);
      digitalWrite(SEGMENT_F, LOW);
      digitalWrite(SEGMENT_G, HIGH);
      break;   
    
    case 1  :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, HIGH);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, HIGH);
      digitalWrite(SEGMENT_E, HIGH);
      digitalWrite(SEGMENT_F, HIGH);
      digitalWrite(SEGMENT_G, HIGH);
      break; 
  
    case 2  :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, HIGH);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, LOW);
      digitalWrite(SEGMENT_F, HIGH);
      digitalWrite(SEGMENT_G, LOW);
      break;
    
    case 3  :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, HIGH);
      digitalWrite(SEGMENT_F, HIGH);
      digitalWrite(SEGMENT_G, LOW);
      break;  

    case 4  :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, HIGH);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, HIGH);
      digitalWrite(SEGMENT_E, HIGH);
      digitalWrite(SEGMENT_F, LOW);
      digitalWrite(SEGMENT_G, LOW);
      break; 

    case 5 :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, HIGH);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, HIGH);
      digitalWrite(SEGMENT_F, LOW);
      digitalWrite(SEGMENT_G, LOW);
      break;      
  
    case 6 :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, HIGH);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, LOW);
      digitalWrite(SEGMENT_F, LOW);
      digitalWrite(SEGMENT_G, LOW);
      break;      

    case 7 :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, HIGH);
      digitalWrite(SEGMENT_E, HIGH);
      digitalWrite(SEGMENT_F, HIGH);
      digitalWrite(SEGMENT_G, HIGH);
      break;
 
    case 8 :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, LOW);
      digitalWrite(SEGMENT_F, LOW);
      digitalWrite(SEGMENT_G, LOW);
      break;     
    
    case 9 :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, LOW);
      digitalWrite(SEGMENT_C, LOW);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, HIGH);
      digitalWrite(SEGMENT_F, LOW);
      digitalWrite(SEGMENT_G, LOW);
      break;  
    
    // sets E for Error
    default :
      digitalWrite(SEGMENT_DP, LOW);
      digitalWrite(SEGMENT_A, LOW);
      digitalWrite(SEGMENT_B, HIGH);
      digitalWrite(SEGMENT_C, HIGH);
      digitalWrite(SEGMENT_D, LOW);
      digitalWrite(SEGMENT_E, LOW);
      digitalWrite(SEGMENT_F, LOW);
      digitalWrite(SEGMENT_G, LOW);  
  }
}

void loop() {
  
  // put your main code here, to run repeatedly:
  button_state_B = digitalRead(BUTTON_B);
  digitalWrite(LED, !button_state_B);

  button_state_A = digitalRead(BUTTON_A);
  digitalWrite(BUZZER, !button_state_A);

  for(int i=0; i<4; i++){
    enable_digit_x(i+1);
    
    for(int j=0; j<10; j++){
      set_digit(j);
      delay(10);
    }       
  }
}
