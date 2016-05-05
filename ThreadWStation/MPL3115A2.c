/*
 * MPL3115A2.c
 *
 * This library is for reading I2C MPL3115A2 sensor with Raspberry Pi. The MPL3115A2 needs repeated start to
 * read data from the sensor. To get the I2C repeated start work on the Raspberry Pi, one need to enable a feature
 * called "combined transaction" in the i2c-bcm2708 device driver. First change the permissions: chmod 666 /dev/i2c-1
 * and chmod 666 /sys/module/i2c_bcm2708/parameters/combined and lastly set the actual combined transaction flag:
 * echo -n 1 > /sys/module/i2c_bcm2708/parameters/combined
 */

#include "MPL3115A2.h"

/* Static function declarations */
static int writeRegister(const unsigned char reg, const unsigned char value);
static unsigned char readRegister(unsigned char reg);
static unsigned char checkData(void);
static int readSensorData(unsigned char *readRegister, const size_t readRegisterLen, unsigned char *dataBuffer, const size_t dataBufferLen);
static void enableEventFlags(void);
static void setModeAltimeter(const unsigned char sampleRate);
static void setModeBarometer(const unsigned char sampleRate);
static void setModeStandby(void);
static void setModeActive(void);
static void setOverSampleRate(unsigned char sampleRate);
static void toggleOneShot(void);
static void clearRegisterBit(const unsigned char reg, const unsigned char bitMask);
static void setRegisterBit(const unsigned char reg, const unsigned char bitMask);

/* Static global i2c file descriptor */
static int g_fd;

/* Initializes the I2C bus and the MPL3115A2 sensor */
int initMPL3115A2_I2C(void)
{
	const char *fileName = "/dev/i2c-1";

	if((g_fd=open(fileName, O_RDWR | O_NONBLOCK)) < 0)
	{
		perror("Failed to open i2c port\n");
		return -1;
	}

	if(ioctl(g_fd, I2C_SLAVE, MPL3115A2_ADDR) < 0)
	{
		perror("Unable to get bus access to talk to slave\n");
		close(g_fd);
		return -1;
	}

	/* Check if the sensor is running by checking the whoami register */
	unsigned char sensorStatus = readRegister(MPL3115A2_WHOAMI);

	if(sensorStatus == 0xC4)
	{
		printf("Sensor is online!\n");
	}
	else{
		printf("Who am I error, sensor is offline\n");
		return -1;
	}

	/* Set the control register to zero and to standby mode  */
	writeRegister(MPL3115A2_CTRL_REG1, 0x00);
	/* Enable data event flags */
	enableEventFlags();

	return 0;
}

int closeI2C(void)
{
    int statusVal;
    statusVal = close(g_fd);

    if(statusVal < 0)
    {
    	perror("Could not close I2C device");
    	return -1;
    }

    return statusVal;
}

/* Writes one byte to the sensor register */
static int writeRegister(const unsigned char reg, const unsigned char value)
{
	unsigned char buf[2] = { 0 };

	if(ioctl(g_fd, I2C_SLAVE, MPL3115A2_ADDR) < 0)
	{
		perror("Unable to get bus access to talk to slave\n");
		close(g_fd);
		return -1;
	}

	buf[0] = reg;
	buf[1] = value;

	if((write(g_fd, buf, 2)) != 2)
	{
		perror("Error writing to i2c slave\n");
		close(g_fd);
		return -1;
	}

	return 0;
}

/* Reads one byte from the sensor register */
static unsigned char readRegister(unsigned char reg)
{
	int result;
	unsigned char readRegisterValue;

	struct i2c_msg rdwr_msg[2];
	struct i2c_rdwr_ioctl_data rdwr_data1;

	if(ioctl(g_fd, I2C_SLAVE, MPL3115A2_ADDR) < 0)
	{
		perror("Unable to get bus access to talk to slave\n");
		close(g_fd);
		return -1;
	}

    rdwr_data1.msgs = rdwr_msg;
    rdwr_data1.nmsgs = 2;

    rdwr_data1.msgs[0].addr = MPL3115A2_ADDR;
    rdwr_data1.msgs[0].flags = 0; //Write
    rdwr_data1.msgs[0].len = 1;
    rdwr_data1.msgs[0].buf = &reg;

    rdwr_data1.msgs[1].addr = MPL3115A2_ADDR;
    rdwr_data1.msgs[1].flags = I2C_M_RD; //Read
    rdwr_data1.msgs[1].len = 1;
    rdwr_data1.msgs[1].buf = &readRegisterValue;

    result = ioctl(g_fd, I2C_RDWR, &rdwr_data1);

    if(result < 0)
    {
    	perror( "rdwr ioctl error: \n");
    }

    return readRegisterValue;
}

/* Checks if there is data waiting for reading */
static unsigned char checkData(void)
{
	unsigned char statusData;

	return statusData = readRegister(MPL3115A2_STATUS);
}

/* Reads multiple bytes (the actual measurement data) from the sensor */
static int readSensorData(unsigned char *readRegister, const size_t readRegisterLen, unsigned char *dataBuffer, const size_t dataBufferLen)
{
	int result;
	struct i2c_msg rdwr_msgs[2];
	struct i2c_rdwr_ioctl_data rdwr_data;

	rdwr_data.msgs = rdwr_msgs;
	rdwr_data.nmsgs = 2;

	rdwr_data.msgs[0].addr = MPL3115A2_ADDR;
	rdwr_data.msgs[0].flags = 0; //Write
	rdwr_data.msgs[0].len = readRegisterLen;
	rdwr_data.msgs[0].buf = readRegister;

	rdwr_data.msgs[1].addr = MPL3115A2_ADDR;
	rdwr_data.msgs[1].flags = I2C_M_RD; //Read
	rdwr_data.msgs[1].len = dataBufferLen;
	rdwr_data.msgs[1].buf = dataBuffer;

	result = ioctl(g_fd, I2C_RDWR, &rdwr_data);

	if(result < 0)
	{
		perror("rdwr ioctl error: \n");
		return -1;
	}
	return 0;
}

void readMPL3115A2Pressure(float *pressure)
{
	unsigned char dataReady = 0;
	const unsigned char overSampleRate = 7;
	unsigned char pressureReadAddress[3] = { MPL3115A2_P_DATA1, MPL3115A2_P_DATA2, MPL3115A2_P_DATA3 };
	unsigned char pressureDataBuffer[3] = { 0 };

    setModeStandby();
    setModeBarometer(overSampleRate);
    setModeActive();

    toggleOneShot();

	/* Check the PDR bit if pressure data is ready */
	while(! (dataReady & PDR))
	{
		dataReady = checkData();
		usleep(1000);
	}

	readSensorData(pressureReadAddress, 3, pressureDataBuffer, 3);

	/* Get pressure, the 20-bit measurement in Pascals is comprised of an unsigned integer component and a fractional component.
	   The unsigned 18-bit integer component is located in RawData[0], RawData[1] and bits 7-6 of RawData[2].
	   The fractional component is located in bits 5-4 of RawData[2]. Bits 3-0 of RawData[2] are not used.*/
	*pressure = (float) ((((pressureDataBuffer[0] << 16) | (pressureDataBuffer[1] << 8) | (pressureDataBuffer[2] & 0xC0)) >> 6) + (float) ((pressureDataBuffer[2] & 0x30) >> 4) * 0.25) * 0.01;
	/* Division by 100 (multiplication by 0,01) to show the value in hPa */
}

void readMPL3115A2Temperature(float *temperature)
{
	 unsigned char dataReady = 0;
	 const unsigned char overSampleRate = 7;
	 unsigned char temperatureReadAddress[2] = { MPL3115A2_T_DATA1, MPL3115A2_T_DATA2 };
	 unsigned char temperatureDataBuffer[2] = { 0 };

	 setModeStandby();
	 setModeAltimeter(overSampleRate);
	 setModeActive();

	 toggleOneShot();

	 /* Check the TDR bit if temperature data is ready */
	 while(! (dataReady & TDR))
	 {
		 dataReady = checkData();
		 usleep(1000);
	 }

	 readSensorData(temperatureReadAddress, 2, temperatureDataBuffer, 2);

	 /* Get temperature, the 12-bit temperature measurement in °C is comprised of a signed integer component and a fractional
		component. The signed 8-bit integer component is located in RawData[3].
		The fractional component is located in bits 7-4 of RawData[4]. Bits 3-0 of OUT_T_LSB are not used. */
	 *temperature = (float) ((short)((temperatureDataBuffer[0] << 8) | (temperatureDataBuffer[1] & 0xF0)) >> 4) * 0.0625;
}

void readMPL3115A2Altitude(float *altitude)
{
	 unsigned char dataReady = 0;
	 const unsigned char overSampleRate = 7;
	 unsigned char altitudeReadAddress[3] = { MPL3115A2_P_DATA1, MPL3115A2_P_DATA2, MPL3115A2_P_DATA3 };
	 unsigned char altitudeDataBuffer[3] = { 0 };

	 setModeStandby();
	 setModeAltimeter(overSampleRate);
	 setModeActive();

	 toggleOneShot();

	 /* Check the PDR bit if altitude data is ready */
	 while(! (dataReady & PDR))
	 {
		 dataReady = checkData();
		 usleep(1000);
	 }

	 readSensorData(altitudeReadAddress, 3, altitudeDataBuffer, 3);

	 /* Get altitude, the 20-bit measurement in meters is comprised of a signed integer component and a fractional component.
		The signed 16-bit integer component is located in RawData[0] and RawData[1].
		The fraction component is located in bits 7-4 of RawData[2]. Bits 3-0 of RawData[2] are not used */
	 *altitude = (float) ((short) ((altitudeDataBuffer[0] << 8) | altitudeDataBuffer[1])) + (float) (altitudeDataBuffer[2] >> 4) * 0.0625;
}

/* Enable all data flags */
static void enableEventFlags(void)
{
	 writeRegister(MPL3115A2_PT_DATA_CFG, ENABLE_EVENT_FLAGS);
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
	writeRegister(MPL3115A2_CTRL_REG1, temp);
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
    writeRegister(reg, temp);
}

static void setRegisterBit(const unsigned char reg, const unsigned char bitMask)
{
    unsigned char temp = readRegister(reg);   // Read the current register value
    temp |= bitMask;               // Clear the bit from the value
    writeRegister(reg, temp);
}
