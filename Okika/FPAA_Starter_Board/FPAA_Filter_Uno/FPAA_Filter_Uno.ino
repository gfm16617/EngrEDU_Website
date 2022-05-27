#include <SPI.h>
#include "LP_Test_4MHz.h"

#define SELb 10
#define ACLK 3

int incomingByte; // for incoming serial data

void resetConfig(){
  // State Reconfiguration
  byte msg[9] = {0, 0, 0, 0, 0, 213, 1, 111, 0};

  digitalWrite(SELb, HIGH);
  for(int i=0; i<9; i++){  
    SPI.transfer(msg[i]);
  }
  digitalWrite(SELb, LOW);
}

void sendConfig(){
  // Send Configuration
  digitalWrite(SELb, HIGH);
  /* sizeof is six greater than the size given at the top of the 
   *  text file. This is because six dummy bytes have been added to 
   *  the data, five at the start and one at the end.
   *  */
  for(int i=0; i<(sizeof(data)+6); i++){
    SPI.transfer(data[i]);
  }
  digitalWrite(SELb, LOW);
}

void setup() {
  // Initialize ACLK
  pinMode(ACLK, OUTPUT);
  TCCR2A = 0x23;
  TCCR2B = 0x09;
  OCR2A = 3;
  OCR2B = 1;
  
  // Initialize SPI
  pinMode(SELb, OUTPUT); // set the SS pin as an output

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  
  resetConfig();
  sendConfig();
  
  // Initialize Serial
  Serial.begin(57600);
}

void loop() {
}
