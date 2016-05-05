/*
 * MCP3002SPI.h
 */

#ifndef MCP3002SPI_H_
#define MCP3002SPI_H_

#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <strings.h>

#define ZERO_PERCENT_VOLTAGE	0.8
#define TMP36_OFFSET      		0.5
#define INPUT_VOLTAGE			3.3

/* Function prototypes */
int spiOpen(void);
int spiClose(void);
void readTMP36Temperature(float *TMP36temperature);
void readHIH4030Humidity(float *HIH4030humidity, float *temperature);

#endif /* MCP3002SPI_H_ */
