#define LED  13 

// Volatile Variables
volatile unsigned char gISRFlag1   = 0;

unsigned int gTimerCounter = 62500;
char toggled = 0;

/**
 * @brief Setup peripherals and timers
 * @param
 * @return
 */
void setup() {
  // LEDs Pins
  pinMode(LED, OUTPUT);
  
  // Initialize Timer1 (16bit) -> Used for clock
  // Speed of Timer1 = 16MHz/256 = 62.5 KHz
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = gTimerCounter; // compare match register 16MHz/256
  TCCR1B |= (1<<WGM12);   // CTC mode
  // Start Timer by setting the prescaler
  TCCR1B |= (1<<CS12);    // 256 prescaler 
  TIMSK1 |= (1<<OCIE1A);  // enable timer compare interrupt
  interrupts();
}

/**
 * @brief Timer 1 ISR
 * @param TIMER1_COMPA_vect
 * @return
 */
ISR(TIMER1_COMPA_vect)  // Timer1 interrupt service routine (ISR)
{
  gISRFlag1 = 1;
}

/**
 * @brief Main Loop
 * @param
 * @return
 */
void loop() 
{
  
  // Attend Button 2 ISR
  if(gISRFlag1 == 1)
  {
    // Clear Flag
    gISRFlag1 = 0;  

    // Toggle LED
    toggled = !toggled;
    digitalWrite(LED, toggled);
  }
}
