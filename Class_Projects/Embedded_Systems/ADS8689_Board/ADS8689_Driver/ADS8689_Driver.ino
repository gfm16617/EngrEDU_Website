#include "ADS8689.h"

/* ******************************
// Global Variables
****************************** */
uint16_t gADC_Data = 0;
float gADC_Voltage = 0.0;

void setup() {
  uint16_t answer = 0;

  // Init User Pins
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);

  // Start Serial
  Serial.begin(115200);

  // Start SPI
  SPI.begin();

  // ADS8689_DEVICE_ID_REG
  answer = ADS8689_SendCommand(ADS8689_READ_HWORD_CMD, ADS8689_DEVICE_ID_REG, 0x0000);
  Serial.print("Device ID: ");
  Serial.println(answer);

  // ADS8689_RANGE_SEL_REG
  answer = ADS8689_SendCommand(ADS8689_READ_HWORD_CMD, ADS8689_RANGE_SEL_REG, 0x0000);
  Serial.print("RANGE_SEL_REG: ");
  Serial.println(answer);

  // INTREF_DIS[6] = 0b = Internal Reference is enabled -- RANGE_SEL[3:0] = 0000b = +/- 3xVref 
  ADS8689_SendCommand(ADS8689_WRITE_CMD, ADS8689_RANGE_SEL_REG, 0b0000000000000000);
  // INTREF_DIS[6] = 0b = Internal Reference is enabled -- RANGE_SEL[3:0] = 1000b = 3xVref
  //ADS8689_SendCommand(ADS8689_WRITE_CMD, ADS8689_RANGE_SEL_REG, 0b0000000000001000);
}

void loop() {

  gADC_Data = ADS8689_SendCommand(ADS8689_NOP_CMD, ADS8689_NO_OP_REG, 0x0000);
  //Serial.println(gADC_Data);

  gADC_Voltage = (gADC_Data - 32768) * (24.576/65536); // RANGE_SEL[3:0] = 0000b = +/- 3xVref
  //gADC_Voltage = (gADC_Data) * (12.288/65536);  // RANGE_SEL[3:0] = 1000b = 3xVref
  
  //Serial.print("15.1, "); Serial.print("-15.1, ");
  Serial.println(gADC_Voltage); 

  delay(1);  // 10ms
}
