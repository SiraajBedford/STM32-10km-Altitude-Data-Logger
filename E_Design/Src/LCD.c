/*
 * LCD.c
 *
 *  Created on: 24 Apr 2019
 *      Author: User 1
 */

/*
 * Author: S. Bedford
 *
 *
 * */

#include "stm32f3xx_it.h"
#include "auxilliary.h"
#include "main.h"
#include "string.h"
#include "stdlib.h"



#define PIN_LCD_DB4 GPIO_PIN_15
#define PIN_LCD_DB5 GPIO_PIN_14
#define PIN_LCD_DB6 GPIO_PIN_13
#define PIN_LCD_DB7 GPIO_PIN_12
#define PIN_LCD_RS GPIO_PIN_8
#define PIN_LCD_RNW GPIO_PIN_4
#define PIN_LCD_E GPIO_PIN_8

uint32_t getUs(void) {

	uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000;

	register uint32_t ms, cycle_cnt;
	do {
		ms = HAL_GetTick();
		cycle_cnt = SysTick->VAL;
	} while (ms != HAL_GetTick());
	return (ms * 1000) + (usTicks * 1000 - cycle_cnt) / usTicks;
}

void delayUs(uint16_t micros) {

	uint32_t start = getUs();
	while (getUs()-start < (uint32_t) micros) {
		//		asm('nop');

	}
}



void writeNibble(uint8_t dni, uint8_t nibble) {

	// read not write
	HAL_GPIO_WritePin(GPIOB, PIN_LCD_RNW, GPIO_PIN_RESET);

	// data or instruction
	if (dni) {
		HAL_GPIO_WritePin(GPIOA, PIN_LCD_RS, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(GPIOA, PIN_LCD_RS, GPIO_PIN_RESET);
	}

	// enable
	HAL_GPIO_WritePin(GPIOC, PIN_LCD_E, GPIO_PIN_SET);

	// data
	HAL_GPIO_WritePin(GPIOB, PIN_LCD_DB7, GPIO_PIN_SET & ((nibble >> 3) & 0x1));
	HAL_GPIO_WritePin(GPIOB, PIN_LCD_DB6, GPIO_PIN_SET & ((nibble >> 2) & 0x1));
	HAL_GPIO_WritePin(GPIOB, PIN_LCD_DB5, GPIO_PIN_SET & ((nibble >> 1) & 0x1));
	HAL_GPIO_WritePin(GPIOB, PIN_LCD_DB4, GPIO_PIN_SET & ((nibble) & 0x1));


	// delay
	int i;
	for (i = 0; i < 100; i++);

	HAL_Delay(1); // ms

	// disable en pin
	HAL_GPIO_WritePin(GPIOC, PIN_LCD_E, GPIO_PIN_RESET);

	HAL_Delay(1);
}

void writeByte(uint8_t dni, uint8_t byte) {
	writeNibble(dni, byte >> 4);
	writeNibble(dni, byte);
}

void _delay(int d) {
	for(; !d; d--);
	//	HAL_Delay(1);
	delayUs(d);
}

void lcdClear() {
	writeByte(0, 0x0E);// Display ON with Cursor
	_delay(40);
	writeByte(0, 0x01);// Clear LCD display and home cursor
	_delay(1640);
	writeByte(0, 0x06);// Cursor auto increment after R/W
	_delay(40);
	writeByte(0, 0x80);// Move cursor to line 1
	_delay(40);
}

void lcdInit() {
	// lcd init

	// wait for more than 15ms after Vcc rises to 4.5 V

	writeByte(0, 0x3);//Function set special case
	_delay(4100);//wait for more than 4.1 us
	writeByte(0, 0x3);//Function set special case
	_delay(100);//wait for more than 100 us
	writeByte(0, 0x3);//Function set special case
	_delay(40);
	writeByte(0, 0x2);//Function set to change the interface (lower 4 bits not relevant)
	_delay(40);

	/*
	 * the  device is in 8-bit mode when powered up and up until this point
	 * the device device reads all 8 data pins when Enable is pulsed
	 *
	 * the device now switches to 4-bit mode
	 * the device reads only 4 data pins when Enable is pulsed
	 * The device temporarily stores the first group of four data bits it receives.
	 * After it receives the second group group of 4 data bits it will reassemble them
	 * and execute the resulting instruction - no time delay between two required
	 *
	 * Step 5.   Instruction 0010b (2h), then delay > 100 us

//Here is where the LCD controller is expecting the 'real' Function Set instruction which, in the 8-bit mode, would start with 0011.   Instead, it gets a Function Set instruction starting with 0010.   This is it's signal to again ignore the lower four bits, switch to the four-bit mode, and expect another 'real' Function Set instruction.
//Once again the required time delay is speculation.
//The LCD controller is now in the 4-bit mode.
//This means that the LCD controller reads only the
//four high order data pins each time the Enable pin is pulsed .   To accomodate this, the host microcontroller must put the four high bits on the data lines and pulse the enable pin, it must then put the four low bits on the data lines and again pulse the enable pin.   There is no need for a delay between these two sequences because the LCD controller isn't processing the instruction yet.   After the second group of data bits is received the LCD controller
//reconstructs and executes the instruction and this is when the delay is required.

*/

	writeByte(0, 0x28); //function set 0010 and NF** where I=1 and F as required
	//// Setup, 4 bits,2 lines, 5X7
	_delay(40);

	lcdClear();

	_delay(10000);
	HAL_Delay(10);

	// test data
	//writeByte(1, 'z');

	_delay(1000);
	HAL_Delay(10);
}

void lcdPrint(char * message) {

	uint8_t len = strlen(message);
	//	lcdClear();

	writeByte(0, 0x06);
	_delay(40);
	writeByte(0, 0x80);
	_delay(40);

	delayUs(100);

	volatile uint8_t i = 0;

	for (i = 0; i < len; i++)
	{
		writeByte(1, message[i]);
		if (i == 7){
			writeByte(0, 0xC0);
		}
	}
}

////UserInit() is useless so far because there are not many variables we need here and in the main.c
//void UserInitilization(){
//
//	HAL_UART_Receive_IT(&huart1, (uint8_t*)&one, 1);
//
//}
