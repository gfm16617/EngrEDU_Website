#define LED  13 

// Volatile Variables
volatile unsigned char gISRFlag1   = 0;

byte gTimerCounter = 125;  //2ms
char toggled = 0;

/**
 * @brief Setup peripherals and timers
 * @param
 * @return
 */
void setup() {
  // LEDs Pins
  pinMode(LED, OUTPUT);

  Serial.begin(115200);
  uint32_t cpuFrequency = F_CPU;
  Serial.println(cpuFrequency);
  
  // Initialize Timer2 (8bit)
  // Speed of Timer2 = 16MHz/256 = 62.5 KHz
  noInterrupts();
  TCCR2A = 0;
  TCCR2B = 0;
  OCR2A = gTimerCounter; // compare match register 16MHz/256
  TCCR2A |= (1<<WGM21);   // CTC mode
  // Start Timer by setting the prescaler
  TCCR2B |= (1<<CS22) | (1<<CS21);    // 256 prescaler 
  TIMSK2 |= (1<<OCIE2A);  // enable timer compare interrupt
  interrupts();
}

/**
 * @brief Timer 1 ISR
 * @param TIMER1_COMPA_vect
 * @return
 */
ISR(TIMER2_COMPA_vect)  // Timer1 interrupt service routine (ISR)
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
