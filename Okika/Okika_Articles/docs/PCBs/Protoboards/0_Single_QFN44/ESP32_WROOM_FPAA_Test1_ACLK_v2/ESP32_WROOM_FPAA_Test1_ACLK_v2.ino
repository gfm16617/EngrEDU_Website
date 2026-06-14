#include <Arduino.h>
#include <SPI.h>
#include "FPAA_Config.h"

#define SELb      5
#define PIN_MOSI  23
#define PIN_MISO  19
#define PIN_SCK   18
#define PIN_PORb  27
#define ACLK      26

#define SPI_SPEED 4000000 // 4MHz

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
  // Init ACLK
  pinMode(ACLK, OUTPUT);
  // 16 MHz square wave, 50 % duty
  ledcAttach(ACLK, 16000000, 1); // Configure the pin with frequency and resolution
  ledcWrite(ACLK, 1);             // output HIGH half the time

  // Initialize Serial
  Serial.begin(57600);

  // Initialize GPIO
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
}
