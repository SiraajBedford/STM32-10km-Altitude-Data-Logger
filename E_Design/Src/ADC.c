/*
 * ADC.c
 *
 *  Created on: 24 Apr 2019
 *      Author: 21093741
 */
#include "stm32f3xx_it.h"
#include "auxilliary.h"
#include "main.h"
#include "string.h"
#include "stdlib.h"
#include "inttypes.h"

//uint32_t adcVal1;
//float PA0;
//float ADC_volatage_in_volts;
//
//
//uint32_t adcVal2;
//float PA1;
//float ADC_current_in_milliamps;

int running;
int time_on;

uint32_t adcVal1;
float PA0;
float ADC_volatage_in_volts;

uint32_t adcVal2;
float PA1;
float ADC_current_in_milliamps;

float ave_voltage;
float ave_current;
float total_voltage = 0;
float total_current = 0;

extern ADC_ChannelConfTypeDef sConfig ;
extern ADC_HandleTypeDef hadc1;

int counter=0;


void ADC(){running = HAL_GetTick();

if(running - time_on >= 50){

	time_on = running;
	time_on = HAL_GetTick();


	sConfig.Channel = ADC_CHANNEL_1;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK){

		adcVal1 = HAL_ADC_GetValue(&hadc1)+3;
		PA0 = 3.3 * (float)(adcVal1/256.0);
		ADC_volatage_in_volts = PA0*((10.0+22.0)/10.0);
		total_voltage += ADC_volatage_in_volts;

	}


	sConfig.Channel = ADC_CHANNEL_2;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);


	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1, 5) == HAL_OK){

		adcVal2 = HAL_ADC_GetValue(&hadc1);//ADC give a higher value due to possible  PA1 damaged
		PA1 = 3.3 * (float)(adcVal2/256.0);
		ADC_current_in_milliamps = ((PA1 / 11.0)/2.0)*1000;
		total_current += ADC_current_in_milliamps;

	}

	counter++;

	if(counter == 20){

		ave_voltage = total_voltage/20;
		ave_current = total_current/20;

		total_current=0;
		total_voltage=0;
		counter=0;


	}

	HAL_Delay(50);

}

}
