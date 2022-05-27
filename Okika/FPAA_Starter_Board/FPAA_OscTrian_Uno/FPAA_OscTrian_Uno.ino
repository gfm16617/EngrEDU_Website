#include <SPI.h>
#include "OscTrian.h"
#include "OscTrian_Freq.h"

#define SELb 10
#define ACLK 3

const int num_configs = 11;
const int size_data = 15; //sizeof(data) + 1
byte *osc_freq[num_configs] = { freq_C1, freq_C2, freq_C3, freq_C4, freq_C5, 
                                freq_C6, freq_C7, freq_C8, freq_C9, freq_C10, freq_C11 };

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
  SPI.setClockDivider(SPI_CLOCK_DIV2); // 16MHz / SPI_CLOCK_DIV2 = 8MHz
  
  resetConfig();
  sendConfig();
}

void loop(){

  for(int i=0; i<num_configs; i++){
    digitalWrite(SELb, HIGH);
    for(int j=0; j<size_data; j++){
      SPI.transfer(osc_freq[i][j]);
    }
    digitalWrite(SELb, LOW); 
    delay(200); //wait
  }
  
}
