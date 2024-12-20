/*!
* @brief Simple Test
*
* Date: 2/7/2024
*/
#define RED_LED   8
#define GREEN_LED 4
#define BLUE_LED  7

#define RGB_BLUE  5
#define RGB_RED   9
#define RGB_GREEN 6

#define BUTTON_1  2
#define BUTTON_2  3

#define POT_1     A0
#define POT_2     A1

int gPot_1 = 0;
int gPot_2 = 0;

/*!
* @brief Setup Function
*/
void setup() {
  // Initialize LEDs
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // Initialize RGB LED
  pinMode(RGB_RED, OUTPUT);
  pinMode(RGB_GREEN, OUTPUT);
  pinMode(RGB_BLUE, OUTPUT);

  // Initialize Buttons
  pinMode(BUTTON_1, INPUT);
  pinMode(BUTTON_2, INPUT);

  // Initialize serial port
  Serial.begin(9600);

}

/*!
* @brief Loop Function
*/
void loop() {
  // Flash LEDs at 200ms
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
  delay(100);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  delay(100);

  // Read Potentiometers
  gPot_1 = analogRead(POT_1);  // read the input pin
  gPot_2 = analogRead(POT_2);  // read the input pin

  // Print value over serial
  Serial.print("Val 1: ");
  Serial.print(gPot_1);
  Serial.print(" Val 2: ");
  Serial.println(gPot_2);

  // Read buttons and turn on/off RGB LED
  if(digitalRead(BUTTON_1))
    digitalWrite(RGB_BLUE, HIGH);
  else
    digitalWrite(RGB_BLUE, LOW);

  if(digitalRead(BUTTON_2))
    digitalWrite(RGB_RED, HIGH);
  else
    digitalWrite(RGB_RED, LOW);

}
