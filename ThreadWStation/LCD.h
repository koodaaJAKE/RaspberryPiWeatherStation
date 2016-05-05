/*
 * LCD.h
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdio.h>
#include <unistd.h> //Used for UART
#include <fcntl.h> //Used for UART
#include <termios.h> //Used for UART
#include <stdlib.h>
#include <string.h>
#include <errno.h>


/* Function prototypes */
int setup_Serial(void);
int serialLCD_Close(void);
int clear_LCD(void);
int setType_LCD(const unsigned char line, const unsigned char col);
int setBacklight_LCD(const unsigned char brightness);
int setCursor_LCD(const unsigned char line, const unsigned char col);
int writeChar_LCD(const unsigned char c);
int printLongString_LCD(const char *pstring, const size_t len);
int printString_LCD(const char *pstring, const size_t len);
void printData_LCD(float temperature, float pressure, float altitude);
void printMPL3115A2Temperature_LCD(const float temperature);
void printTMP36Temperature_LCD(const float temperature);
void printPressure_LCD(const float pressure);
void printAltitude_LCD(const float altitude);
void printHumidity_LCD(const float humidity);
int printConnect(void);
int printDisconnect(void);





#endif /* LCD_H_ */
