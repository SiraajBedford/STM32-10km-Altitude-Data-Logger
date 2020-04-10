/*
 * auxilliary.h
 *
 *  Created on: 27 Feb 2019
 *      Author: 20797540
 */

#include "stdint.h"
#include "stdio.h"
#include "main.h"
#include "stm32f3xx_it.h"
#ifndef AUXILLIARY_H_
#define AUXILLIARY_H_

//User: S. Bedford, SU: 21093741
//Permission A. Esmael, SU: 20797540


// lcd

void delayUs(uint16_t micros);
//void writeNibble(uint8_t dni, uint8_t nibble);
//void writeByte(uint8_t dni, uint8_t nibble);
//void _delay(int d);
void lcdClear();
void lcdInit();
void lcdPrint(char * message);

int32_t accx, accy, accz;

void accelerometerInit();
void accelerometerRead();

/* USER CODE BEGIN 2 */
extern volatile uint8_t flag;
extern int lasttick ;
extern char output_string [91];
extern int student_num;
extern char GPGGA_string [75];
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern float latitude;
extern float altitude ;
extern char hour[3];
extern char sec[3];
extern char min[3];
extern int range_buffer[5];
extern int burn_start;
extern int burn_flag;
extern int runtime;
extern int field_check[6];//[0]=time, [1]=latitude,[2]=latitude direction,[3]=longitude,[4]=longitude direction,[5]=altitude

extern float ADC_volatage_in_volts;
extern float ADC_volatage_in_volts_average;
extern float ADC_current_in_milliamps;
extern float ADC_current_in_milliamps_average;
extern float ave_voltage;
extern float ave_current;
extern volatile uint8_t tim3;


void clear_range_buffer();
void StringPrint();
void BurnReleaseOn();
void BurnReleaseOff();
void var_init();
void GPSReceive();
void GPGGA_Split(const char *s);
int check_sum(const char *s);
void GPS_array_clear();
void range_check();
int five_consecutive_check();
void add_element(int element);
void presence_check(const char *s);
void GetAltitude(const char *s);
void GetTime(const char *s);
void GetLongitude(const char *s);
void GetLatitude(const char *s);
#endif /* AUXILLIARY_H_ */
