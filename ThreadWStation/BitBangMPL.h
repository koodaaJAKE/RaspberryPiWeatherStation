/*
 * BitBangMPL.h
 */

#ifndef BITBANGMPL_H_
#define BITBANGMPL_H_

#include <bcm2835.h>
#include <unistd.h>
#include <stdio.h>

// Defines
#define	TRUE	1
#define	FALSE	0
#define MPL3115A2_DELAY delayMicroseconds(5)

// Define the Raspberry Pi GPIO Pins for the MPL3115A2
#define RPI_GPIO_MPL3115A2_SCK RPI_BPLUS_GPIO_J8_31
#define RPI_GPIO_MPL3115A2_DATA RPI_BPLUS_GPIO_J8_29

/* Macros to toggle port state of SCK and DATA lines. */
#define MPL3115A2_SCK_LO	bcm2835_gpio_write(RPI_GPIO_MPL3115A2_SCK, LOW)
#define MPL3115A2_SCK_HI	bcm2835_gpio_write(RPI_GPIO_MPL3115A2_SCK, HIGH)
#define MPL3115A2_DATA_LO 	bcm2835_gpio_write(RPI_GPIO_MPL3115A2_DATA, LOW);\
							bcm2835_gpio_fsel(RPI_GPIO_MPL3115A2_DATA, BCM2835_GPIO_FSEL_OUTP)
#define	MPL3115A2_DATA_HI 	bcm2835_gpio_fsel(RPI_GPIO_MPL3115A2_DATA, BCM2835_GPIO_FSEL_INPT)
#define MPL3115A2_GET_BIT 	bcm2835_gpio_lev(RPI_GPIO_MPL3115A2_DATA)

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
#define PDR 					0x04		//PDR bits indicates if new pressure/altitude data is available.
#define TDR 					0x02		//TDR bit indicates if new temperature data is available

/* Function prototypes */
void initMPL3115A2(void);
void readPressure(float *pressure);
void readTemperature(float *temperature);
void readAltitude(float *altitude);

#endif /* BITBANGMPL_H_ */
