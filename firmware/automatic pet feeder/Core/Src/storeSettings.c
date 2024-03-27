/*
 * storeSettings.c
 *
 *  Created on: Jan 13, 2023
 *      Author: Filippo Castellan
 */
#include "storeSettings.h"

/* FUNZIONI */
void storeSettings(Meal m1, Meal m2, Meal m3, uint8_t brk, uint8_t lu,
		uint8_t din) {
	/* DATA PREPARATION */
	uint8_t rawdata[12];
	rawdata[0] = m1.type;
	rawdata[1] = m1.hour;
	rawdata[2] = m1.min;

	rawdata[3] = m2.type;
	rawdata[4] = m2.hour;
	rawdata[5] = m2.min;

	rawdata[6] = m3.type;
	rawdata[7] = m3.hour;
	rawdata[8] = m3.min;

	rawdata[9] = brk;
	rawdata[10] = lu;
	rawdata[11] = din;

	/* STORING DATA */
	EEPROM_Write(DATA_PAGE, DATA_OFFSET, rawdata, 12);
}

void getSettings(Meal *meal1, Meal *meal2, Meal *meal3, uint8_t *brk, uint8_t *lu,
		uint8_t *din) {
	uint8_t rawdata[12];
	EEPROM_Read(DATA_PAGE, DATA_OFFSET, rawdata, 12);

	meal1->type = rawdata[0];
	meal1->hour = rawdata[1];
	meal1->min = rawdata[2];

	meal2->type = rawdata[3];
	meal2->hour = rawdata[4];
	meal2->min = rawdata[5];

	meal3->type = rawdata[6];
	meal3->hour = rawdata[7];
	meal3->min = rawdata[8];

	*brk = rawdata[9];
	*lu = rawdata[10];
	*din = rawdata[11];
}

