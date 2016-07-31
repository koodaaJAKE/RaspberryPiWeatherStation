/*
 * thread.c
 */
#include "thread.h"
#include "LCD.h"
#include "BitBangMPL.h"
#include "MCP3002SPI.h"
#include "Bluetooth_RFCOMM.h"
#include "TCP_Socket.h"

/* Static function declarations */
static int GetKey(void);

/* Local flag for terminate the thread loops */
static volatile sig_atomic_t thread_loop_flag = 0;

/* Calling the signal handler will set the thread loop terminating flag */
void sigHandler(int sig)
{
	printf("I got the signal! Exiting...\n");
	//Set the flag to exit thread loops
    thread_loop_flag = 1;
}

int initMutex(thread_data_t *init_mutex_t)
{
	int res;

	res = pthread_mutex_init(&init_mutex_t->mutex1, NULL);
	if (res != 0) {
		perror("Mutex1 initialization failed \n");
		return -1;
	}

	res = pthread_mutex_init(&init_mutex_t->mutex2, NULL);
	if (res != 0) {
		perror("Mutex2 initialization failed \n");
		return -1;
	}

	res = pthread_mutex_init(&init_mutex_t->mutex3, NULL);
	if (res != 0) {
		perror("Mutex3 initialization failed \n");
		return -1;
	}

	res = pthread_mutex_init(&init_mutex_t->mutex4, NULL);
	if (res != 0) {
		perror("Mutex4 initialization failed \n");
		return -1;
	}

	res = pthread_mutex_init(&init_mutex_t->mutex5, NULL);
	if (res != 0) {
		perror("Mutex5 initialization failed \n");
		return -1;
	}

	return 0;
}

/* This thread reads the I2C MPL3115A2 sensor */
void *measureMPL3115A2(void *arg)
{
	while(!thread_loop_flag)
	{
		thread_data_t *sensorData = (thread_data_t*)arg;

		pthread_mutex_lock(&sensorData->mutex1);
		readTemperature(sensorData);
		pthread_mutex_unlock(&sensorData->mutex1);

		pthread_mutex_lock(&sensorData->mutex2);
		readPressure(&sensorData->pressure);
		pthread_mutex_unlock(&sensorData->mutex2);

		pthread_mutex_lock(&sensorData->mutex3);
		readAltitude(&sensorData->altitude);
		pthread_mutex_unlock(&sensorData->mutex3);
	}
	pthread_exit(NULL);
}

/* This thread reads the TMP36 and HIH4030 sensors */
void *measureMCP3002(void *arg)
{
	while(!thread_loop_flag)
	{
		thread_data_t *sensorData = (thread_data_t*)arg;

		pthread_mutex_lock(&sensorData->mutex4);
		readTMP36Temperature(&sensorData->TMP36temperature);
		pthread_mutex_unlock(&sensorData->mutex4);

		pthread_mutex_lock(&sensorData->mutex5);
		readHIH4030Humidity(sensorData);
		pthread_mutex_unlock(&sensorData->mutex5);
	}
	pthread_exit(NULL);
}

/* This thread polls the stdin for pressed keyboard keys */
void *printToLCD(void *arg)
{
	thread_data_t *sensorData = (thread_data_t*)arg;

	while(!thread_loop_flag)
	{
		int key;
		key = GetKey();

		/*
		 * Print the measurement values by pressing the particular keyboard key
		 */

		/* Press t for MPL3115A2 temperature */
		if(key == 't')
		{
			pthread_mutex_lock(&sensorData->mutex1);
			printMPL3115A2Temperature_LCD(sensorData->MPL3115A2temperature);
			pthread_mutex_unlock(&sensorData->mutex1);
			sleep(1);
			clear_LCD();
		}

		/* Press p for pressure */
		if(key == 'p')
		{
			pthread_mutex_lock(&sensorData->mutex2);
			printPressure_LCD(sensorData->pressure);
			pthread_mutex_unlock(&sensorData->mutex2);
			sleep(1);
			clear_LCD();
		}

		/* Press a for altitude */
		if(key == 'a')
		{
			pthread_mutex_lock(&sensorData->mutex3);
			printAltitude_LCD(sensorData->altitude);
			pthread_mutex_unlock(&sensorData->mutex3);
			sleep(1);
			clear_LCD();
		}

		/* Press y for TMP36 temperature */
		if(key == 'y')
		{
			pthread_mutex_lock(&sensorData->mutex4);
			printTMP36Temperature_LCD(sensorData->TMP36temperature);
			pthread_mutex_unlock(&sensorData->mutex4);
			sleep(1);
			clear_LCD();
		}

		/* Press h for humidity */
		if(key == 'h')
		{
			pthread_mutex_lock(&sensorData->mutex5);
			printHumidity_LCD(sensorData->humidity);
			pthread_mutex_unlock(&sensorData->mutex5);
			sleep(1);
			clear_LCD();
		}
	}
	pthread_mutex_destroy(&sensorData->mutex1);
	pthread_mutex_destroy(&sensorData->mutex2);
	pthread_mutex_destroy(&sensorData->mutex3);
	pthread_mutex_destroy(&sensorData->mutex4);
	pthread_mutex_destroy(&sensorData->mutex5);

	pthread_exit(NULL);
}

/* This thread starts the Bluetooth RFCOMM server */
void *bluetoothRFCOMM(void *arg)
{
	while(!thread_loop_flag)
	{
		thread_data_t *sensorData = (thread_data_t*)arg;
/*
		if(bluetoothRFCOMM_Server(sensorData) == 0);
				thread_loop_flag = 1;
*/
		if(TCP_SocketPollingServer(sensorData) <= 0);
			thread_loop_flag = 1;
	}
	pthread_exit(NULL);
}

/*
 set the terminal into raw (non-canonical) mode by using tcsetattr() to manipulate the termios structure.
 Clearing the ECHO and ICANON flags respectively disables echoing of characters as they are typed and
 causes read requests to be satisfied directly from the input queue. Setting the values of VTIME and VMIN
 to zero in the c_cc array causes the read request (fgetc()) to return immediately rather than block;
 effectively polling stdin. The call to fgetc() will return EOF if a character is not available in the stream.
 */
static int GetKey(void)
{
    int character, error;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    error = tcgetattr(fileno(stdin), &orig_term_attr);
    if(error == -1)
    {
    	perror("tcgetattr error!\n");
    	return -1;
    }

    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;

    error = tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);
    if(error == -1)
    {
    	perror("tcsetattr error!\n");
    	return -1;
    }

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    error = tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);
    if(error == -1)
    {
    	perror("tcsetattr error!\n");
    	return -1;
    }

    return character;
}
