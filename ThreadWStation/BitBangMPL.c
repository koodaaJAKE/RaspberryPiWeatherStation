/*
 * BitBangMPL.c
 *
 * This library reads MPL3115A2 (repeated start) I2C sensor with Raspberry Pi. The library utilizes bcm2835 library
 * to bit bang the I2C bus.
 */
#include "BitBangMPL.h"
#include "MPL3115A2.h"

/* Static function declarations */
static void MPL3115A2_InitPins(void);
static void transmissionStart(void);
static void transmissionStop(void);
static unsigned char sendByte(const unsigned char value);
static unsigned char readByte(const unsigned char send_ack);
static void setRegister(const unsigned char reg, const unsigned char value);
static unsigned char readRegister(const unsigned char reg);
static void readStatus(void);
static unsigned char checkData(void);
static void setModeAltimeter(const unsigned char sampleRate);
static void setModeBarometer(const unsigned char sampleRate);
static void setModeStandby(void);
static void setModeActive(void);
static void setOverSampleRate(unsigned char sampleRate);
static void enableEventFlags(void);
static void toggleOneShot(void);
static void clearRegisterBit(const unsigned char reg, const unsigned char bitMask);
static void setRegisterBit(const unsigned char reg, const unsigned char bitMask);

static void MPL3115A2_InitPins(void)
{
	// SCK line as output but set to low first
	bcm2835_gpio_write(RPI_GPIO_MPL3115A2_SCK, LOW);
	bcm2835_gpio_fsel(RPI_GPIO_MPL3115A2_SCK, BCM2835_GPIO_FSEL_OUTP);

	// DATA line as output and in low state. External pull up.
	bcm2835_gpio_set_pud(RPI_GPIO_MPL3115A2_DATA, BCM2835_GPIO_PUD_OFF);
	bcm2835_gpio_write(RPI_GPIO_MPL3115A2_DATA, LOW);
	bcm2835_gpio_fsel(RPI_GPIO_MPL3115A2_DATA, BCM2835_GPIO_FSEL_OUTP);
}

static void transmissionStart(void)
{
	MPL3115A2_SCK_HI;
	MPL3115A2_DELAY;
	MPL3115A2_DATA_HI;
	MPL3115A2_DELAY;
	MPL3115A2_DATA_LO;
	MPL3115A2_DELAY;
	MPL3115A2_SCK_LO;
	MPL3115A2_DELAY;
}

static void transmissionStop(void)
{
	MPL3115A2_SCK_HI;
	MPL3115A2_DELAY;
	MPL3115A2_DATA_LO;
	MPL3115A2_DELAY;
	MPL3115A2_DATA_HI;
	MPL3115A2_DELAY;
	MPL3115A2_SCK_LO;
	MPL3115A2_DELAY;
}

static unsigned char sendByte(const unsigned char value)
{
	unsigned char mask;
	unsigned char ack;

	for(mask = 0x80 ; mask ; mask >>= 1)
	{
		MPL3115A2_SCK_LO;
		MPL3115A2_DELAY;

		if( value & mask )
		{
			MPL3115A2_DATA_HI;
			MPL3115A2_DELAY;
		}
		else
		{
			MPL3115A2_DATA_LO;
			MPL3115A2_DELAY;
		}

		MPL3115A2_SCK_HI;	// SCK hi => sensor reads data
		MPL3115A2_DELAY;
	}

	MPL3115A2_SCK_LO;
	MPL3115A2_DELAY;

	// Release DATA line
	MPL3115A2_DATA_HI;
	MPL3115A2_DELAY;
	MPL3115A2_SCK_HI;
	MPL3115A2_DELAY;

	ack = 0;

	if(!MPL3115A2_GET_BIT)
		ack = 1;

	MPL3115A2_SCK_LO;
	MPL3115A2_DELAY;

	return ack;
}

static unsigned char readByte(const unsigned char send_ack)
{
	unsigned char value = 0;
	unsigned char mask;

	// SCK is low here !
	for(mask = 0x80 ; mask ; mask >>= 1 )
	{
		MPL3115A2_SCK_HI;
		MPL3115A2_DELAY;  	// SCK hi

		if( MPL3115A2_GET_BIT != 0 )  	// and read data
			value |= mask;

		MPL3115A2_SCK_LO;	// SCK lo => sensor puts new data
		MPL3115A2_DELAY;
	}

	/* send ACK if required */
	if ( send_ack )
	{
		MPL3115A2_DATA_LO;
		MPL3115A2_DELAY; // Get DATA line
	}

	MPL3115A2_SCK_HI;
	MPL3115A2_DELAY;    // give a clock pulse
	MPL3115A2_SCK_LO;
	MPL3115A2_DELAY;

	if ( send_ack )
	{
		// Release DATA line
		MPL3115A2_DATA_HI;
		MPL3115A2_DELAY;
	}

	return value;
}

void initMPL3115A2()
{
	MPL3115A2_InitPins();
	usleep(1000);
	readStatus();
	usleep(1000);
	/* Set control register to zero and to standby mode */
	setRegister(MPL3115A2_CTRL_REG1, 0x00);
	enableEventFlags();
}

static void setRegister(const unsigned char reg, const unsigned char value)
{
	transmissionStart();
	sendByte(MPL3115A2_WRITE);
	sendByte(reg);
	sendByte(value);
	transmissionStop();
	usleep(1000);
}

static unsigned char readRegister(const unsigned char reg)
{
	unsigned char regValue;

	transmissionStart();
	sendByte(MPL3115A2_WRITE);
	sendByte(reg);
	transmissionStart();
	sendByte(MPL3115A2_READ);

	regValue = readByte(FALSE);

	transmissionStop();

	usleep(1000);

	return regValue;
}

static void readStatus(void)
{
	unsigned char status;

	transmissionStart();
	sendByte(MPL3115A2_WRITE);
	sendByte(MPL3115A2_WHOAMI);
	transmissionStart();
	sendByte(MPL3115A2_READ);

	status = readByte(FALSE);

	transmissionStop();

	if(status == 0xC4 )
		printf("Sensor is online! \n");
	else
		printf("Sensor is offline! \n");
}

static unsigned char checkData(void)
{
	unsigned char statusData;

	transmissionStart();
	sendByte(MPL3115A2_WRITE);
	sendByte(MPL3115A2_STATUS);
	transmissionStart();
	sendByte(MPL3115A2_READ);

	return statusData = readByte(FALSE);

	transmissionStop();
}

void readPressure(float *pressure)
{
	const unsigned char overSampleRate = 1;
	setModeStandby();
	setModeBarometer(overSampleRate);
	setModeActive();

	toggleOneShot();

	unsigned char status = 0;
	unsigned char pData[3];

	//Wait for data to come available
	while( ! (status & PDR))
	{
		status = checkData();
		usleep(1000);
	}

	transmissionStart();
	sendByte(MPL3115A2_WRITE);
	sendByte(MPL3115A2_P_DATA1);
	sendByte(MPL3115A2_P_DATA2);
	sendByte(MPL3115A2_P_DATA3);
	transmissionStart();
	sendByte(MPL3115A2_READ);

	pData[0] = readByte(TRUE);
	pData[1] = readByte(TRUE);
	pData[2] = readByte(FALSE);
	transmissionStop();
	/* Get pressure, the 20-bit measurement in Pascals is comprised of an unsigned integer component and a fractional component.
	   The unsigned 18-bit integer component is located in RawData[0], RawData[1] and bits 7-6 of RawData[2].
	   The fractional component is located in bits 5-4 of RawData[2]. Bits 3-0 of RawData[2] are not used.*/
	*pressure = ((float) (((pData[0] << 16) | (pData[1] << 8) | (pData[2] & 0xC0)) >> 6) + (float) ((pData[2] & 0x30) >> 4) * 0.25) * 0.01;
	/* Division by 100 (multiplication by 0,01) to show the pressure in hPa */
}

void readTemperature(float *temperature)
{
	const unsigned char overSampleRate = 1;
	setModeStandby();
	setModeAltimeter(overSampleRate);
	setModeActive();

	toggleOneShot();

	unsigned char status = 0;
	unsigned char tData[2];

	//Wait for data to come available
	while( ! (status & TDR))
	{
		status = checkData();
		usleep(1000);
	}

	transmissionStart();
	sendByte(MPL3115A2_WRITE);
	sendByte(MPL3115A2_T_DATA1);
	sendByte(MPL3115A2_T_DATA2);
	transmissionStart();
	sendByte(MPL3115A2_READ);

	tData[0] = readByte(TRUE);
	tData[1] = readByte(FALSE);
	transmissionStop();

	/* Get temperature, the 12-bit temperature measurement in °C is comprised of a signed integer component and a fractional
	   component. The signed 8-bit integer component is located in RawData[3].
	   The fractional component is located in bits 7-4 of RawData[4]. Bits 3-0 of OUT_T_LSB are not used. */
	*temperature = (float) ((short)((tData[0] << 8) | (tData[1] & 0xF0)) >> 4) * 0.0625;
}

void readAltitude(float *altitude)
{
	const unsigned char overSampleRate = 1;
	setModeStandby();
	setModeAltimeter(overSampleRate);
	setModeActive();

	toggleOneShot();

	unsigned char status = 0;
	unsigned char aData[3];

	//Wait for data to come available
	while( ! (status & PDR))
	{
		status = checkData();
		usleep(1000);
	}

	transmissionStart();
	sendByte(MPL3115A2_WRITE);
	sendByte(MPL3115A2_P_DATA1);
	sendByte(MPL3115A2_P_DATA2);
	sendByte(MPL3115A2_P_DATA3);
	transmissionStart();
	sendByte(MPL3115A2_READ);

	aData[0] = readByte(TRUE);
	aData[1] = readByte(TRUE);
	aData[2] = readByte(FALSE);
	transmissionStop();

	/* Get altitude, the 20-bit measurement in meters is comprised of a signed integer component and a fractional component.
	   The signed 16-bit integer component is located in RawData[0] and RawData[1].
	   The fraction component is located in bits 7-4 of RawData[2]. Bits 3-0 of RawData[2] are not used */
	*altitude = (float) ((short) ((aData[0] << 8) | aData[1])) + (float) (aData[2] >> 4) * 0.0625;
}

static void setModeAltimeter(const unsigned char sampleRate)
{
	setOverSampleRate(sampleRate);
	setRegisterBit(MPL3115A2_CTRL_REG1, 0x80);
}

static void setModeBarometer(const unsigned char sampleRate)
{
	setOverSampleRate(sampleRate);
	clearRegisterBit(MPL3115A2_CTRL_REG1, 0x80);
}

static void setModeStandby(void)
{
	clearRegisterBit(MPL3115A2_CTRL_REG1, 0x01);
}

static void setModeActive(void)
{
	setRegisterBit(MPL3115A2_CTRL_REG1, 0x01);
}

/********************************************************/
/* Sets the number of samples from 1 to 128             */
/********************************************************/
static void setOverSampleRate(unsigned char sampleRate)
{
	if(sampleRate > 7)
		sampleRate = 7;	//OSR can't be larger than 7

	sampleRate <<= 3;	//Align it for the CTRL_REG1 register

	unsigned char temp = readRegister(MPL3115A2_CTRL_REG1);
	temp &= 0xC7;
	temp |= sampleRate;
	setRegister(MPL3115A2_CTRL_REG1, temp);
}

static void enableEventFlags(void)
{
	setRegister(MPL3115A2_PT_DATA_CFG, ENABLE_EVENT_FLAGS);
}

/***************************************************************/
/* Toggle one shot bit to take immediately another measurement */
/***************************************************************/
static void toggleOneShot(void)
{
	clearRegisterBit(MPL3115A2_CTRL_REG1, 0x02);	//Clear OST bit
	setRegisterBit(MPL3115A2_CTRL_REG1, 0x02);		//Set OST bit
}

static void clearRegisterBit(const unsigned char reg, const unsigned char bitMask)
{
    unsigned char temp = readRegister(reg);   // Read the current register value
    temp &= ~bitMask;               // Clear the bit from the value
    setRegister(reg, temp);
}

static void setRegisterBit(const unsigned char reg, const unsigned char bitMask)
{
    unsigned char temp = readRegister(reg);   // Read the current register value
    temp |= bitMask;               // Clear the bit from the value
    setRegister(reg, temp);
}
