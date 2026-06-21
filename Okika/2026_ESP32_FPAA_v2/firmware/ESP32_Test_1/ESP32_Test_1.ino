#include <Arduino.h>
#include <SPI.h>
#include "FPAA_Config.h"

#define SELb      15
#define PIN_MOSI  13
#define PIN_MISO  9
#define PIN_SCK   14
#define PIN_PORb  11
#define PIN_ACT   12

#define SPI_SPEED 4000000 // 4MHz

#define USER_LED  48

void hardwareReset(){
  digitalWrite(PIN_PORb, LOW);
  delay(20); // 20ms
  digitalWrite(PIN_PORb, HIGH);
  delay(100); // 100ms
}

void softwareReset(){
  // State Reconfiguration
  byte msg[9] = {0, 0, 0, 0, 0, 213, 1, 111, 0};

  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(SELb, LOW);
  for(int i=0; i<9; i++){  
    SPI.transfer(msg[i]);
  }
  digitalWrite(SELb, HIGH);
  SPI.endTransaction();
}

void primaryConfig(){
  // Send Configuration
  SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE3));
  digitalWrite(SELb, LOW);
  for(int i=0; i<(sizeof(primary_config)); i++){
    SPI.transfer(primary_config[i]);
  }
  digitalWrite(SELb, HIGH);
  SPI.endTransaction();
}

void setup() {
  // Initialize Serial
  Serial.begin(57600);

  // Initialize GPIO
  pinMode(USER_LED, OUTPUT);
  
  pinMode(PIN_ACT, OUTPUT); // Activate pin
  digitalWrite(PIN_ACT, HIGH);

  pinMode(PIN_PORb, OUTPUT); // hardware reset pin
  digitalWrite(PIN_PORb, HIGH);

  pinMode(SELb, OUTPUT); // set the SS pin as an output
  digitalWrite(SELb, HIGH);

  // Init SPI
  SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, SELb);

  delay(500);  // Allow SPI to stabilize
  
  // Program FPAA
  //hardwareReset();
  softwareReset();
  delay(10);
  primaryConfig(); // Send FPAA Primary Configuration
}

void loop() {
  digitalWrite(USER_LED, HIGH);
  delay(200);
  digitalWrite(USER_LED, LOW);
  delay(200);
}
