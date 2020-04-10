/*
 * I2C.c
 * Created on: 07 May 2019
 * Author: S. Bedford
 * Usage/Credit: https://github.com/BoschSensortec/BME280_driver
 */

#include "main.h"
#include "auxilliary.h"
#include "bme280.h"
#include "bme280_defs.h"
#include "lis2dh12_reg.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

float hum;
float press;
float temperature;

extern I2C_HandleTypeDef hi2c1;

int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	HAL_I2C_Master_Transmit(&hi2c1, id<<1 ,&reg_addr, 1 , 100);
	HAL_I2C_Master_Receive(&hi2c1,(id<<1)+1,data,len,100);

	return 0;
}

void user_delay_ms(uint32_t period)
{
	HAL_Delay(period);
}

int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
	int8_t *buf;
	buf = malloc(len +1);
	buf[0] = reg_addr;
	memcpy(buf +1, data, len);
	HAL_I2C_Master_Transmit(&hi2c1 , id<<1 , (uint8_t*)buf , len+1 , 100);
	free(buf);
	return 0;
}


int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev, int8_t rslt)
{
	//int8_t rslt;
	uint8_t settings_sel;
	struct bme280_data comp_data;

	/* Recommended mode of operation: Indoor navigation */
	dev->settings.osr_h = BME280_OVERSAMPLING_1X;
	dev->settings.osr_p = BME280_OVERSAMPLING_16X;
	dev->settings.osr_t = BME280_OVERSAMPLING_2X;
	dev->settings.filter = BME280_FILTER_COEFF_16;

	settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;

	rslt = bme280_set_sensor_settings(settings_sel, dev);


		rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, dev);
		/* Wait for the measurement to complete and print data @25Hz */
		dev->delay_ms(40);
		rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, dev);
		temperature = (float)(comp_data.temperature)/100;
		hum = (float)(comp_data.humidity)/1000;
		press = (comp_data.pressure)/100000;

	return rslt;
}

