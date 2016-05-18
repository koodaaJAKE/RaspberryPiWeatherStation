/*
 * thread.h
 */

#ifndef PTHREAD_H_
#define PTHREAD_H_

#include <pthread.h>
#include <termios.h>
#include <signal.h>

typedef struct thread_data
{
	float MPL3115A2temperature;
	float pressure;
	float altitude;
	float TMP36temperature;
	float humidity;
	pthread_mutex_t mutex1;
	pthread_mutex_t mutex2;
	pthread_mutex_t mutex3;
	pthread_mutex_t mutex4;
	pthread_mutex_t mutex5;

}thread_data_t;

/* Function prototypes */
void sigHandler(int sig);
int initMutex(thread_data_t *init_mutex_t);
void *measureMPL3115A2(void *arg);
void *measureMCP3002(void *arg);
void *printToLCD(void *arg);
void *bluetoothRFCOMM(void *arg);

#endif /* PTHREAD_H_ */
