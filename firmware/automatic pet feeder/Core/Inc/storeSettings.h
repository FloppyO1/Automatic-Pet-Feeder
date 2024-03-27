/*
 * storeSettings.h
 *
 *  Created on: Jan 13, 2023
 *      Author: Filippo Castellan
 */

#ifndef INC_STORESETTINGS_H_
#define INC_STORESETTINGS_H_

#include "stm32g0xx_hal.h"
#include "main.h"

/* DEFINE */
#define DATA_PAGE 0
#define DATA_OFFSET 0


/* FUNZIONI */
void storeSettings(Meal m1, Meal m2, Meal m3, uint8_t brk, uint8_t lu, uint8_t din);	//store the settings into the eeprom memory
void getSettings();	//extract the stored settings on the eeprom



#endif /* INC_STORESETTINGS_H_ */
