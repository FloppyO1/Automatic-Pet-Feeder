/*
 * DS3231lib.h
 *
 *  Created on: Jan 12, 2023
 *      Author: Filippo Castellan
 *
 *  NOTE: to use this library you only need to include "DS3231lib.h" file into the main.c file.
 *  	  In this file modify in the INCLUDE section the correct _hal.h file according to your MCU.
 */

#ifndef INC_DS3231LIB_H_
#define INC_DS3231LIB_H_

/* INCLUDE */
#include "stm32g0xx_hal.h"	// included for all the data type <------------- INCLUDE THE RIGHT HAL.H FILE

/* DS3231 REGISTER'S ADDRESS */
#define DS3231_ADDRESS 0xD0

/* DATA & DATA TYPE */
typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hour;
	uint8_t dayofweek;
	uint8_t dayofmonth;
	uint8_t month;
	uint8_t year;
} TIME;

/* FUNTIONS */
uint8_t decToBcd(int val);	// convert decimal to bcd
int bcdToDec(uint8_t val);	// convert bcd to decimal
void Set_Time(uint8_t sec, uint8_t min, uint8_t hour, uint8_t dow, uint8_t dom,
		uint8_t month, uint8_t year, I2C_HandleTypeDef *i2c);// set the time and date
TIME Get_Time(I2C_HandleTypeDef *i2c);	//get the time from the DS3231 module

#endif /* INC_DS3231LIB_H_ */
