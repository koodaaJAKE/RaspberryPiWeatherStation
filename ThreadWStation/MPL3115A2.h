/*
 * MPL3115A2.h
 */

#ifndef MPL3115A2_H_
#define MPL3115A2_H_

#include <unistd.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* Definitions of MPL3115A2 commands */
#define MPL3115A2_ADDR			0x60		//MPL3115A2 internal address
#define MPL3115A2_WRITE			0xC0		//MPL3115A2 write address
#define MPL3115A2_READ			0xC1		//MPL3115A2 read address
#define MPL3115A2_WHOAMI		0x0C		//Device identification register. Default 0xC4
#define MPL3115A2_STATUS		0x00		//Data ready status register
#define MPL3115A2_P_DATA1		0x01		//Pressure and altitude data out MSB
#define MPL3115A2_P_DATA2		0x02		//Pressure and altitude data out CSB
#define MPL3115A2_P_DATA3		0x03		//Pressure and altitude data out LSB
#define MPL3115A2_T_DATA1		0x04		//Temperature data out MSB
#define MPL3115A2_T_DATA2		0x05		//Temperature data out LSB
#define MPL3115A2_CTRL_REG1 	0x26		//Control register
#define MPL3115A2_PT_DATA_CFG	0x13		//Data event flag register
#define ENABLE_EVENT_FLAGS		0x07		//Enables all data event flags
#define PDR 					0x04		//PDR bits indicates if new pressure/altitude data is available.
#define TDR 					0x02		//TDR bit indicates if new temperature data is available

/* Function prototypes */
int initMPL3115A2_I2C(void);
int closeI2C(void);
void readMPL3115A2Pressure(float *pressure);
void readMPL3115A2Temperature(float *temperature);
void readMPL3115A2Altitude(float *altitude);

#endif /* MPL3115A2_H_ */
