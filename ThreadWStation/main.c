/*
 * main.c
 */
#include "LCD.h"
#include "BitBangMPL.h"
//#include "MPL3115A2.h"
#include "MCP3002SPI.h"
#include "thread.h"

int main(void)
{
	/* Structure of sensor measurement data */
	thread_data_t sensorData;
	memset(&sensorData, 0, sizeof(sensorData));

	sensorData.minMPL3115A2temperature = FLT_MAX;
	sensorData.maxMPL3115A2temperature = FLT_MIN;
	sensorData.minHumidity = FLT_MAX;
	sensorData.maxHumidity = FLT_MIN;

	pthread_t measureMPL3115A2Thread, measureMCP3002Thread, printToLCDThread, bluetoothRFCOMMThread;
	int iret, iret1, iret2, iret3;

	/* Initialize the signal handler */
	signal(SIGINT, sigHandler);

	/* Initialize the Raspberry Pi GPIO */
	if(!bcm2835_init())
		return 1;

	/* MPL3115A2 BitBang setup */
	initMPL3115A2();

#ifdef MPL3115A2_H_
	/* MPL3115A2 setup */
	initMPL3115A2_I2C();
#endif

	/* LCD setup */
	setup_Serial();
	clear_LCD();
	setType_LCD(4, 16);
	setBacklight_LCD(250);

	/* MCP3002SPI setup */
	spiOpen();

	/* Initialize mutex */
	initMutex(&sensorData);

	printf("**************************************************\n");
	printf("Print MPL3115A2 temperature by pressing t         \n");
	printf("Print MPL3115A2 pressure by pressing    p         \n");
	printf("Print MPL3115A2 altitude by pressing    a         \n");
	printf("Print TMP36 temperature by pressing     y         \n");
	printf("Print HIH4030 humidity by pressing      h         \n");
	printf("Quit the program by pressing Ctrl+C               \n");
	printf("**************************************************\n");

	/************************************************************/
	/* Create Threads                                           */
	/************************************************************/
	iret = pthread_create(&measureMPL3115A2Thread, NULL, measureMPL3115A2, (void*)&sensorData);
	if(iret)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret);
		exit(EXIT_FAILURE);
	}

	iret1 = pthread_create(&measureMCP3002Thread, NULL, measureMCP3002, (void*)&sensorData);
	if(iret1)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
		exit(EXIT_FAILURE);
	}

	iret2 = pthread_create(&printToLCDThread, NULL, printToLCD, (void*)&sensorData);
	if(iret2)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret2);
		exit(EXIT_FAILURE);
	}

	iret3 = pthread_create(&bluetoothRFCOMMThread, NULL, bluetoothRFCOMM, (void*)&sensorData);
	if(iret3)
	{
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret3);
		exit(EXIT_FAILURE);
	}

	/* Let the threads exit */
	pthread_join(measureMPL3115A2Thread, NULL);
	pthread_join(measureMCP3002Thread, NULL);
	pthread_join(printToLCDThread, NULL);
	pthread_join(bluetoothRFCOMMThread, NULL);

	clear_LCD();
	setBacklight_LCD(0);
	serialLCD_Close();
	spiClose();
#ifdef MPL3115A2_H_
	closeI2C();
#endif
	bcm2835_close();
	printf("UART, BCM2835, I2C and SPI disabled!\n");
	return 0;
}

