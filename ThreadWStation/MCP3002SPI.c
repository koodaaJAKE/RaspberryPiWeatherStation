/*
 * MCP3002SPI.c
 *
 *  This library is for reading analog humidity HIH4030 and temperature TMP36 sensors with Raspberry Pi.
 *  The sensors are attached to 10-bit SPI MCP3002 A/D converter.
 */

#include "MCP3002SPI.h"

/* Static local functions */
static int spiWriteRead( unsigned char *data, int length);
static void TMP36CalcTemp(int adc_value, float* tmp);
static void HIH4030CalcHum(int adc_val, float *hum, float *temp);

/* Static local SPI file descriptor variable */
static int spifd;

int spiOpen(void)
{
	unsigned char mode = SPI_MODE_0;
	unsigned int speed = 1000000;
	unsigned char bits = 8;

	int statusVal = -1;
    spifd = open("/dev/spidev0.0", O_RDWR);

    if(spifd < 0)
    {
        perror("could not open SPI device");
        exit(1);
    }

    statusVal = ioctl (spifd, SPI_IOC_WR_MODE, &mode);

    if(statusVal < 0)
    {
    	perror("Could not set SPIMode (WR)...ioctl fail");
        exit(1);
    }

    statusVal = ioctl (spifd, SPI_IOC_RD_MODE, &mode);

    if(statusVal < 0)
    {
    	perror("Could not set SPIMode (RD)...ioctl fail");
    	exit(1);
    }

    statusVal = ioctl (spifd, SPI_IOC_WR_BITS_PER_WORD, &bits);

    if(statusVal < 0)
    {
    	perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
    	exit(1);
    }

    statusVal = ioctl (spifd, SPI_IOC_RD_BITS_PER_WORD, &bits);

    if(statusVal < 0)
    {
    	perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
    	exit(1);
    }

    statusVal = ioctl (spifd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

    if(statusVal < 0)
    {
    	perror("Could not set SPI speed (WR)...ioctl fail");
    	exit(1);
    }

    statusVal = ioctl (spifd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    if(statusVal < 0)
    {
    	perror("Could not set SPI speed (RD)...ioctl fail");
    	exit(1);
    }

    return statusVal;
}

/********************************************************************
 * This function writes data "data" of length "length" to the spidev
 * device. Data shifted in from the spidev device is saved back into
 * "data".
 * ******************************************************************/
static int spiWriteRead( unsigned char *data, int length)
{
	struct spi_ioc_transfer spi[length];
    int i = 0;
    int retVal = -1;

    /* ioctl struct must be initialized to zero */
    bzero(spi, sizeof spi);

    /* One spi transfer for each byte */
    for (i = 0 ; i < length ; i++)
    {
    	spi[i].tx_buf        = (unsigned long)(data + i); // transmit from "data"
    	spi[i].rx_buf        = (unsigned long)(data + i); // receive into "data"
    	spi[i].len           = sizeof(*(data + i));
    	spi[i].delay_usecs   = 0;
    	spi[i].speed_hz      = 1000000;
    	spi[i].bits_per_word = 8;
    	spi[i].cs_change = 0;
    }

    retVal = ioctl (spifd, SPI_IOC_MESSAGE(length), &spi);

    if(retVal < 0)
    {
    	perror("Problem transmitting SPI data..ioctl");
    	exit(1);
    }

    return retVal;
}

int spiClose(void)
{
    int statusVal = -1;
    statusVal = close(spifd);

    if(statusVal < 0)
    {
    	perror("Could not close SPI device");
    	exit(1);
    }

    return statusVal;
}

static void TMP36CalcTemp(int adc_value, float *tmp)
{
	//ADC Value		Temp	Volts
	//	1			-50		0.00
	//	79			-25		0.25
	//	156			0		0.50
	//	234			25		0.75
	//	311			50		1.00
	//	466			100		1.50
	//	776			200		2.50
	//  1023		280		3.30

    *tmp = ((INPUT_VOLTAGE * (double)adc_value / 1023.0) - TMP36_OFFSET) * 100.0;
}

static void HIH4030CalcHum(int adc_val, float *hum, float *temp)
{
	/* The max voltage value drops down 0.006705882 for each degree C over 0C.
	   The voltage at 0C is 3.27 (corrected for zero percent voltage) */
	float max_voltage = (3.27-(0.006706*(*temp)));
	*hum = (((((float)adc_val/1023)*5) - ZERO_PERCENT_VOLTAGE)/max_voltage)*100;
	//*hum = ((0.0004*(*h_temp)+0.149)*adc_val)-(0.0617*(*h_temp)+24.436);
}

void readTMP36Temperature(float *temperature)
{
	unsigned char temperatureADCdata[2] = { 0 };
	int ADCvalue = 0;

	temperatureADCdata[0] = 0x68;	//Analog input CH0
	temperatureADCdata[1] = 0x00;

	spiWriteRead(temperatureADCdata, sizeof(temperatureADCdata));
	usleep(1000);

	ADCvalue = (temperatureADCdata[0]<< 8) | temperatureADCdata[1]; //merge data bytes

	TMP36CalcTemp(ADCvalue, temperature);
}

void readHIH4030Humidity(thread_data_t *mcp3002SPI_Data)
{
	unsigned char humidityADCdata[2] = { 0 };
	int ADCvalue = 0;

	humidityADCdata[0] = 0x78;	//Analog input CH1
	humidityADCdata[1] = 0x00;

	spiWriteRead(humidityADCdata, sizeof(humidityADCdata));
	usleep(1000);

	ADCvalue = (humidityADCdata[0]<< 8) | humidityADCdata[1]; //merge data bytes

	HIH4030CalcHum(ADCvalue, &mcp3002SPI_Data->humidity, &mcp3002SPI_Data->MPL3115A2temperature);

	/* Get the minimum and maximum values */
	if(mcp3002SPI_Data->humidity < mcp3002SPI_Data->minHumidity) {
		mcp3002SPI_Data->minHumidity = mcp3002SPI_Data->humidity;
		//printf("Min humidity: %0.2f\n", mcp3002SPI_Data->minHumidity);

	}
	if(mcp3002SPI_Data->humidity > mcp3002SPI_Data->maxHumidity) {
		mcp3002SPI_Data->maxHumidity = mcp3002SPI_Data->humidity;
		//printf("Max humidity: %0.2f\n", mcp3002SPI_Data->maxHumidity);
	}
}
