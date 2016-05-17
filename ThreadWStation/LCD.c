/*
 * LCD.c
 *
 * This library is for writing to 16x4 LCD display with Serial interface. The 16x4 LCD display is
 * attached to I2C/Serial LCD Backpack and it's controlled with Raspberry Pi.
 */
#include "LCD.h"

/* Static function declarations */
static int createCharacter(const unsigned char memoryLocation, const unsigned char *characterMap);
static int createAwithDots(void);
static int createOwithDots(void);
static int createCapitalAwithDots(void);
static int createCapitalOwithDots(void);
static int printTemperature(const char *pstring, const size_t len);
static int printPressure(const char *pstring, const size_t len);
static int printAltitude(const char *pstring, const size_t len);
static int printHumidity(const char *pstring, const size_t len);

/* Static local variable of serial file descriptor */
static int g_Fd = 0;

/* Static local variable of returned bytes of write function */
static int g_bytesWritten = 0;

/* Static local variable of length of string */
static size_t stringLength = 0;

/* Open serial port and set the settings */
int setup_Serial()
{
	int error;

	/* Open in write mode */
	g_Fd = open("/dev/ttyAMA0", O_WRONLY | O_NOCTTY | O_NDELAY);
	if (g_Fd == -1)
	{
		perror("Error - Unable to open UART!\n");
		return -1;
	}

	struct termios options;

	/* Get the current configuration of the serial interface */
	error = tcgetattr(g_Fd, &options);
	if(error == -1)
	{
		perror("tcgetattr error!\n");
		return -1;
	}

	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //Baud rate 9600, Character size mask CS8, Ignore modem control lines, Enable receiver
	options.c_oflag = 0;
	options.c_lflag = 0;

	/*
	* Flushes (discards) not-send data (data still in the UART send buffer) and/or flushes (discards)
	* received data (data already in the UART receive buffer).
	*/
	error = tcflush(g_Fd, TCOFLUSH);
	if(error == -1)
	{
		perror("tcflush error!\n");
		return -1;
	}

	/* Apply the configuration */
	error = tcsetattr(g_Fd, TCSANOW, &options);
	if(error == -1)
	{
		perror("tcsetattr error!\n");
		return -1;
	}

	/* Create custom made characters */
	createAwithDots();
	createOwithDots();
	createCapitalAwithDots();
	createCapitalOwithDots();

	return 0;
}

/* Close the serial */
int serialLCD_Close(void)
{
    int statusVal;
    statusVal = close(g_Fd);

    if(statusVal < 0)
    {
    	perror("Could not close serial device");
    	return -1;
    }
    return statusVal;
}

/* Creates custom made character */
static int createCharacter(const unsigned char memoryLocation, const unsigned char *characterMap)
{
	/* Memory location for custom made character can be between 0-7 */
	if(memoryLocation <= 7 && memoryLocation >= 0)
	{
		unsigned char characterMapData[8];
		int i;

		/* Populate the character map data array */
		for( i = 0 ; i < 8 ; i++){
			characterMapData[i] = characterMap[i];
		}

		unsigned char data[2], dataEnd[1] = { 0xFF };
		data[0] = 0x40;
		data[1] = memoryLocation;

		g_bytesWritten = write(g_Fd, &data, sizeof(data));

		if (g_bytesWritten < 0)
		{
			perror("Write failed - ");
			return -1;
		}

		g_bytesWritten = write(g_Fd, &characterMapData, sizeof(characterMapData));

		if (g_bytesWritten < 0)
		{
			perror("Write failed - ");
			return -1;
		}

		g_bytesWritten = write(g_Fd, &dataEnd, sizeof(dataEnd));

		if (g_bytesWritten < 0)
		{
			perror("Write failed - ");
			return -1;
		}
		return 0;
	}
	else
		return -1;
}

static int createAwithDots(void)
{
	unsigned char aWithDots[8] = {
		0b01010,
		0b00000,
		0b01110,
		0b00011,
		0b01111,
		0b11011,
		0b01111,
		0b0000
	};

	if(createCharacter(0x00, aWithDots) < 0){
		return -1;
	}
	else
		return 0;
}

static int createOwithDots(void)
{
	unsigned char oWithDots[8] = {
		0b01010,
		0b00000,
		0b01110,
		0b10001,
		0b10001,
		0b10001,
		0b01110,
		0b00000
	};

	if(createCharacter(0x01, oWithDots) < 0){
		return -1;
	}
	else
		return 0;
}

static int createCapitalAwithDots(void)
{
	unsigned char capitalAwithDots[8] = {
		0b01010,
		0b00000,
		0b01110,
		0b10001,
		0b10001,
		0b11111,
		0b10001,
		0b10001
	};

	if(createCharacter(0x02, capitalAwithDots) < 0){
		return -1;
	}
	else
		return 0;
}

static int createCapitalOwithDots(void)
{
	unsigned char capitalOwithDots[8] = {
		0b01010,
		0b00000,
		0b01110,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b01110
	};

	if(createCharacter(0x03, capitalOwithDots) < 0){
		return -1;
	}
	else
		return 0;
}

/* Clear the LCD screen */
int clear_LCD()
{
	unsigned char data[2];
	data[0] = 4;
	data[1] = 0xFF;

	g_bytesWritten = write(g_Fd, &data, sizeof(data));

	if (g_bytesWritten < 0)
	{
		perror("Write failed - ");
		return -1;
	}
	return 0;
}

/* Define number of rows/columns on the display */
int setType_LCD(const unsigned char line, const unsigned char col)
{
	unsigned char data[4];
	data[0] = 0x05;
	data[1] = line;
	data[2] = col;
	data[3] = 0xFF;

	g_bytesWritten = write(g_Fd, &data, sizeof(data));

    if (g_bytesWritten < 0)
    {
            perror("Write failed - ");
            return -1;
    }
    return 0;
}

/* Backlight brightness */
int setBacklight_LCD(const unsigned char brightness)
{
	unsigned char data[3];
	data[0] = 0x07;
	data[1] = brightness;
	data[2] = 0xFF;

	g_bytesWritten =  write(g_Fd, &data, sizeof(data));

    if (g_bytesWritten < 0)
    {
    	perror("Write failed - ");
    	return -1;
    }
    return 0;
}

/* Position the cursor */
int setCursor_LCD(const unsigned char line, const unsigned char col)
{
	unsigned char data[4];
	data[0] = 0x02;
	data[1] = line;
	data[2] = col;
	data[3] = 0xFF;

	g_bytesWritten = write(g_Fd, &data, sizeof(data));

    if (g_bytesWritten < 0)
    {
            perror("Write failed - ");
            return -1;
    }
    return 0;
}

/* Write a single character */
int writeChar_LCD(const unsigned char c)
{
	unsigned char data[3];
	data[0] = 0x0A;
	data[1] = c;
	data[2] = 0xFF;

	g_bytesWritten = write(g_Fd, &data, sizeof(data));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

/* Print long string to a lcd */
int printLongString_LCD(const char *pstring, const size_t len)
{
	size_t i;

		/*
		 * If String is 16 or under characters long
		 */
		if(len <= 16){

			setCursor_LCD(1,1);
			unsigned char data[len+3];
			data[0] = 0x01;
			for(i = 1 ; i < (len+1) ; i++)
			{
				data[i] = *pstring++;
			}

			data[len+1] = '\0';
			data[len+2] = 0xFF;

			g_bytesWritten = write(g_Fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}
		}
		/*
		 * If string is between 16 and 32 characters long
		 */
		else if (len > 16 && len <= 32){

			unsigned char data[19];
			unsigned char data2[len-13];

			setCursor_LCD(1,1);

			data[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data[i] = *pstring++;
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_bytesWritten = write(g_Fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < (len-15) ; i++)
			{
				data2[i] = *pstring++;
			}

			data2[(len-15)] = '\0';
			data2[(len-15)+1] = 0xFF;

			g_bytesWritten = write(g_Fd, &data2, sizeof(data2));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

		}
		/*
		 * If string is between 32-48 characters long
		 */
		else if (len > 32 && len <= 48){

			unsigned char data[19];
			unsigned char data2[19];
			unsigned char data3[len-29];

			setCursor_LCD(1,1);

			data[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data[i] = *pstring++;
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_bytesWritten = write(g_Fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data2[i] = *pstring++;
			}

			data2[17] = '\0';
			data2[18] = 0xFF;

			g_bytesWritten = write(g_Fd, &data2, sizeof(data2));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(3,1);

			data3[0] = 0x01;
			for(i = 1 ; i < (len-31) ; i++)
			{
				data3[i] = *pstring++;
			}
			data3[len-31] = '\0';
			data3[len-31+1] = 0xFF;

			g_bytesWritten = write(g_Fd, &data3, sizeof(data3));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

		}
		/*
		 * If string is more than 48 and max 64 characters long
		 */
		else if(len > 48 && len <= 64){

			unsigned char data[19];
			unsigned char data2[19];
			unsigned char data3[19];
			unsigned char data4[len-45];

			setCursor_LCD(1,1);

			data[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data[i] = *pstring++;
			}

			data[17] = '\0';
			data[18] = 0xFF;

			g_bytesWritten = write(g_Fd, &data, sizeof(data));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(2,1);

			data2[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data2[i] = *pstring++;
			}

			data2[17] = '\0';
			data2[18] = 0xFF;

			g_bytesWritten = write(g_Fd, &data2, sizeof(data2));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(3,1);

			data3[0] = 0x01;
			for(i = 1 ; i < 17 ; i++)
			{
				data3[i] = *pstring++;
			}
			data3[17] = '\0';
			data3[18] = 0xFF;

			g_bytesWritten = write(g_Fd, &data3, sizeof(data3));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}

			//Jump to next line
			setCursor_LCD(4,1);

			data4[0] = 0x01;
			for(i = 1 ; i < (len-47) ; i++)
			{
				data4[i] = *pstring++;
			}
			data4[(len-47)] = '\0';
			data4[(len-47)+1] = 0xFF;

			g_bytesWritten = write(g_Fd, &data4, sizeof(data4));
			if (g_bytesWritten < 0)
			{
				perror("Write failed -");
				return -1;
			}
		} else {
			setCursor_LCD(2,1);
			char tooLongString[] = "Too long string!";
			stringLength = strlen(tooLongString);
			printString_LCD(tooLongString, stringLength);
			printf("Too long string\n");
			sleep(2);
		}

	return 0;
}

/* Print short string */
int printString_LCD(const char *pstring, const size_t len)
{
	unsigned char sData[len+3];
	sData[0] = 0x01;
	size_t i;

	for(i = 1 ; i < (len+1) ; i++)
	{
		sData[i] = *pstring++;
	}

	sData[len+1] = '\0';
	sData[len+2] = 0xFF;

	g_bytesWritten = write(g_Fd, &sData, sizeof(sData));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

//Print temperature to the LCD
static int printTemperature(const char *pstring, const size_t len)
{
	unsigned char sTdata[len+4];
	sTdata[0] = 0x01;
	size_t i;

	for(i = 1 ; i < (len+1) ; i++)
	{
		sTdata[i] = *pstring++;
	}

	sTdata[len+1] = 'C';
	sTdata[len+2] = '\0';
	sTdata[len+3] = 0xFF;

	g_bytesWritten = write(g_Fd, &sTdata, sizeof(sTdata));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

static int printPressure(const char *pstring, const size_t len)
{
	unsigned char sTdata[len+6];
	sTdata[0] = 0x01;
	size_t i;

	for(i = 1 ; i < (len+1) ; i++)
	{
		sTdata[i] = *pstring++;
	}

	sTdata[len+1] = 'h';
	sTdata[len+2] = 'P';
	sTdata[len+3] = 'a';
	sTdata[len+4] = '\0';
	sTdata[len+5] = 0xFF;

	g_bytesWritten = write(g_Fd, &sTdata, sizeof(sTdata));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

static int printAltitude(const char *pstring, const size_t len)
{
	unsigned char sTdata[len+4];
	sTdata[0] = 0x01;
	size_t i;

	for(i = 1 ; i < (len+1) ; i++)
	{
		sTdata[i] = *pstring++;
	}

	sTdata[len+1] = 'm';
	sTdata[len+2] = '\0';
	sTdata[len+3] = 0xFF;

	g_bytesWritten = write(g_Fd, &sTdata, sizeof(sTdata));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

static int printHumidity(const char *pstring, const size_t len)
{
	unsigned char sTdata[len+4];
	sTdata[0] = 0x01;
	size_t i;

	for(i = 1 ; i < (len+1) ; i++)
	{
		sTdata[i] = *pstring++;
	}

	sTdata[len+1] = '%';
	sTdata[len+2] = '\0';
	sTdata[len+3] = 0xFF;

	g_bytesWritten = write(g_Fd, &sTdata, sizeof(sTdata));

	if (g_bytesWritten < 0)
	{
		perror("Write failed -");
		return -1;
	}
	return 0;
}

void printMPL3115A2Temperature_LCD(const float temperature)
{
	setCursor_LCD(2,3);
	const char *strng = "MPL tmp is:";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);

	setCursor_LCD(3,5);
	char buf[6];
	snprintf(buf, sizeof(buf), "%.2f", temperature);
	stringLength = strlen(buf);
	printTemperature(buf, stringLength);
}

void printTMP36Temperature_LCD(const float temperature)
{
	setCursor_LCD(2,3);
	const char *strng = "TMP36 tmp is:";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);

	setCursor_LCD(3,6);
	char buf[6];
	snprintf(buf, sizeof(buf), "%.2f", temperature);
	stringLength = strlen(buf);
	printTemperature(buf, stringLength);
}

void printPressure_LCD(const float pressure)
{
	setCursor_LCD(2,3);
	const char *strng = "MPL pres is:";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);

	setCursor_LCD(3,4);
	char buf[10];
	snprintf(buf, sizeof(buf), "%.2f", pressure);
	stringLength = strlen(buf);
	printPressure(buf, stringLength);
}

void printAltitude_LCD(const float altitude)
{
	setCursor_LCD(2,3);
	const char *strng = "MPL alt is:";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);

	setCursor_LCD(3,4);
	char buf[10];
	snprintf(buf, sizeof(buf), "%.2f", altitude);
	stringLength = strlen(buf);
	printAltitude(buf, stringLength);
}

void printHumidity_LCD(const float humidity)
{
	setCursor_LCD(2,3);
	const char *strng = "Humidity is:";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);

	setCursor_LCD(3,6);
	char buf[6];
	snprintf(buf, sizeof(buf), "%.2f", humidity);
	stringLength = strlen(buf);
	printHumidity(buf, stringLength);
}

int printConnect(void)
{
	setCursor_LCD(2,3);
	const char *strng = "CONNECTED!!";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);
	sleep(2);
	clear_LCD();
	return 0;
}

int printDisconnect(void)
{
	setCursor_LCD(2,2);
	const char *strng = "DISCONNECTED!!";
	stringLength = strlen(strng);
	printString_LCD(strng, stringLength);
	sleep(2);
	clear_LCD();
	return 0;
}
