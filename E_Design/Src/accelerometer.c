/*
 * accelerometer.c
 *
 *  Created on: 04 May 2019
 *      Author: 21093741
 */


#include "LIS2DH12/lis2dh12_reg.h"
#include "auxilliary.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;

static axis3bit16_t data_raw_acceleration;
static axis1bit16_t data_raw_temperature;
float acceleration_mg[3];
static float temperature_degC;
static uint8_t whoamI;
//static uint8_t tx_buffer[1000];

lis2dh12_ctx_t dev_ctx;

static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void tx_com(uint8_t *tx_buffer, uint16_t len);
static void platform_init(void);

void accelerometerInit() {

	dev_ctx.write_reg = platform_write;
	dev_ctx.read_reg = platform_read;
	dev_ctx.handle = &hi2c1;

	/* Initialize platform specific hardware */
	platform_init();

	/*
	 *  Check device ID
	 */
	lis2dh12_device_id_get(&dev_ctx, &whoamI);
	if (whoamI != LIS2DH12_ID)
	{
		while(1)
		{
			/* manage here device not found */
		}
	}

	/* Enable Block Data Update */
	lis2dh12_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

	/* Set Output Data Rate to 1Hz */
	lis2dh12_data_rate_set(&dev_ctx, LIS2DH12_ODR_1Hz);

	/* Set full scale to 2g */
	lis2dh12_full_scale_set(&dev_ctx, LIS2DH12_2g);

	/* Enable temperature sensor */
	lis2dh12_temperature_meas_set(&dev_ctx, LIS2DH12_TEMP_ENABLE);

	/* Set device in continuous mode with 12 bit resol. */
	lis2dh12_operating_mode_set(&dev_ctx, LIS2DH12_HR_12bit);
}

int32_t _999clamp(float acc) {
	if (acc >= 999.0) return 999;
	if (acc <= -999.0) return -999;
	return (int32_t)acc;
}


void accelerometerRead() {
	lis2dh12_reg_t reg;

	/* Read output only if new value available */
	lis2dh12_xl_data_ready_get(&dev_ctx, &reg.byte);
	if (reg.byte)
	{
		/* Read accelerometer data */
		memset(data_raw_acceleration.u8bit, 0x00, 3*sizeof(int16_t));
		lis2dh12_acceleration_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
		acceleration_mg[0] = lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[0]);
		acceleration_mg[1] = lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[1]);
		acceleration_mg[2] = lis2dh12_from_fs2_hr_to_mg(data_raw_acceleration.i16bit[2]);

		accx = _999clamp(acceleration_mg[2]) * -1;
		accy = _999clamp(acceleration_mg[1]);
		accz = _999clamp(acceleration_mg[0]) * -1;

//		sprintf((char*)tx_buffer, "Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
//				acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
//		tx_com(tx_buffer, strlen((char const*)tx_buffer));
	}

	lis2dh12_temp_data_ready_get(&dev_ctx, &reg.byte);
	if (reg.byte)
	{
		/* Read temperature data */
		memset(data_raw_temperature.u8bit, 0x00, sizeof(int16_t));
		lis2dh12_temperature_raw_get(&dev_ctx, data_raw_temperature.u8bit);
		temperature_degC = lis2dh12_from_lsb_hr_to_celsius(data_raw_temperature.i16bit);

//		sprintf((char*)tx_buffer,
//				"Temperature [degC]:%6.2f\r\n",
//				temperature_degC);
//		tx_com(tx_buffer, strlen((char const*)tx_buffer));
	}
}


/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
  if (handle == &hi2c1)
  {
    /* Write multiple command */
    reg |= 0x80;
    HAL_I2C_Mem_Write(handle, LIS2DH12_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  }
  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
  if (handle == &hi2c1)
  {
    /* Read multiple command */
    reg |= 0x80;
    HAL_I2C_Mem_Read(handle, LIS2DH12_I2C_ADD_L, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  }
  return 0;
}

/*
 * @brief  Send buffer to console (platform dependent)
 *
 * @param  tx_buffer     buffer to trasmit
 * @param  len           number of byte to send
 *
 */
static void tx_com(uint8_t *tx_buffer, uint16_t len)
{
  HAL_UART_Transmit(&huart1, tx_buffer, len, 1000);
}

/*
 * @brief  platform specific initialization (platform dependent)
 */
static void platform_init(void)
{
//#ifdef STEVAL_MKI109V3
//  TIM3->CCR1 = PWM_3V3;
//  TIM3->CCR2 = PWM_3V3;
//  HAL_Delay(1000);
//#endif
}
