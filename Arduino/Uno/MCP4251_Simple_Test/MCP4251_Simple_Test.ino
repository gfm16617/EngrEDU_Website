#include <SPI.h>

// Volatile Memory (RAM)
#define MCP4251_WIPER_0_WRITE_ADD   0x00
#define MCP4251_WIPER_1_WRITE_ADD   0x10

#define CS_PIN  10

void setup() {
 
  // initialize SPI:
  SPI.begin(); 

  // Configure Pot
  command_16Bit(MCP4251_WIPER_0_WRITE_ADD, 0); // wiper values from 0 to 255
  command_16Bit(MCP4251_WIPER_1_WRITE_ADD, 128); // wiper values from 0 to 255
}

void loop() {
}

void command_8Bit(uint8_t cmd){
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(cmd);
  digitalWrite(CS_PIN, HIGH);
}

void command_16Bit(uint8_t cmd, uint8_t data){
  digitalWrite(CS_PIN, LOW);
  SPI.transfer(cmd);
  SPI.transfer(data);
  digitalWrite(CS_PIN, HIGH); 
}

