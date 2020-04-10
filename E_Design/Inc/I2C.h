///*
// * I2C.h
// *
// *  Created on: 07 May 2019
// *      Author: User 1
// */
//
//#ifndef I2C_H_
//#define I2C_H_
//
//

#include "bme280_defs.h"



void user_delay_ms(uint32_t period);
int8_t user_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t user_i2c_write(uint8_t dev_id, uint8_t reg_addr, uint8_t *reg_data, uint16_t len);
int8_t stream_sensor_data_forced_mode(struct bme280_dev *dev, int8_t rslt);


//
//
//
//#endif /* I2C_H_ */
