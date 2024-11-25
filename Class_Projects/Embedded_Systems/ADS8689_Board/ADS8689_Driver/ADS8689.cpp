#include "ADS8689.h"

/* ******************************
// ADS8689 SPI Functions
****************************** */
uint16_t ADS8689_SendCommand(uint8_t command, uint8_t address, uint16_t data) {
  uint8_t _transmit_bytes[4];
  uint16_t _received_bytes;

  // Format message to sent over SPI
  _transmit_bytes[0] = (command<<1) | ((address>>8)&1);
  _transmit_bytes[1] = (address&0xFF);
  _transmit_bytes[2] = ((data>>8)&0xFF);
  _transmit_bytes[3] = (data&0xFF);

  // Send message over SPI
  digitalWrite(CS_PIN, LOW);
  SPI.beginTransaction(SPISettings(SPI_SCLK_HZ, MSBFIRST, SPI_MODE0));
  // Send Data
  SPI.transfer(_transmit_bytes, 4);
  SPI.endTransaction();
  digitalWrite(CS_PIN, HIGH);

  // Receive message over SPI
  digitalWrite(CS_PIN, LOW);
  SPI.beginTransaction(SPISettings(SPI_SCLK_HZ, MSBFIRST, SPI_MODE0));
  // Receive 2 Bytes
  _received_bytes = SPI.transfer16(0x0000);
  SPI.endTransaction();
  digitalWrite(CS_PIN, HIGH);

  return _received_bytes;
}