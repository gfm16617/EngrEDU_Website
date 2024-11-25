#ifndef ADS8689_H
#define ADS8689_H

#include <SPI.h>

/* ******************************
// ADS8689 COMMANDS
// Table 7-5 on Page 41
********************************************************* */
#define ADS8689_NOP_CMD             0b0000000
#define ADS8689_CLEAR_HWORD_CMD     0b1100000  // 11000_xx_
#define ADS8689_READ_HWORD_CMD      0b1100100  // 11001_xx_
#define ADS8689_READ_CMD            0b0100100  // 01001_xx_
#define ADS8689_WRITE_CMD           0b1101000  // 11010_00_
#define ADS8689_WRITE_MS_CMD        0b1101001  // 11010_01_
#define ADS8689_WRITE_LS_CMD        0b1101010  // 11010_10_
#define ADS8689_SET_HWORD_CMD       0b1101100  // 11011_xx_

/* ******************************
// ADS8689 REGISTERS
// Table 7-10 on Page 47
********************************************************* */
#define ADS8689_NO_OP_REG           0x00
#define ADS8689_DEVICE_ID_REG       0x00  // 00h
#define ADS8689_RST_PWRCTL_REG      0x04  // 04h
#define ADS8689_SDI_CTL_REG         0x08  // 08h
#define ADS8689_SDO_CTL_REG         0x0C  // 0Ch
#define ADS8689_DATAOUT_CTL_REG     0x10  // 10h
#define ADS8689_RANGE_SEL_REG       0x14  // 14h
#define ADS8689_ALARM_REG           0x20  // 20h
#define ADS8689_ALARM_H_TH_REG      0x24  // 24h
#define ADS8689_ALARM_L_TH_REG      0x28  // 28h

/* ******************************
// SPI 
****************************** */
#define CS_PIN 10
#define SPI_SCLK_HZ 16000000  // 16 MHz

/* ******************************
// ADS8689 SPI Functions
****************************** */
uint16_t ADS8689_SendCommand(uint8_t command, uint8_t address, uint16_t data);

#endif /* ADS8689_H */