/*
 * motor.c
 *
 *  Created on: Dec 9, 2023
 *      Author: Filippo Castellan
 */

#include "main.h"
#include "stm32g0xx_hal.h"
#include "ssd1306.h"
#include "motor.h"

#define STEPS_PER_REV 4096
#define STEP_RATE 7

int vibration = 0;
uint8_t quatrerTurn(uint8_t n) {
	vibration = HAL_GetTick();
	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	for (int step = 0; step < n * STEPS_PER_REV / 4; step++) {
		if (HAL_GetTick() >= vibration + 5000) {
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 1);
			HAL_Delay(1000);
			HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 0);
			vibration = HAL_GetTick();
		}
		switch (step % 4) {
		case 0:
			HAL_GPIO_WritePin(MOT0_GPIO_Port, MOT0_Pin, 1);
			HAL_GPIO_WritePin(MOT1_GPIO_Port, MOT1_Pin, 1);
			HAL_GPIO_WritePin(MOT2_GPIO_Port, MOT2_Pin, 0);
			HAL_GPIO_WritePin(MOT3_GPIO_Port, MOT3_Pin, 0);
			HAL_Delay(STEP_RATE);
			break;
		case 1:
			HAL_GPIO_WritePin(MOT0_GPIO_Port, MOT0_Pin, 0);
			HAL_GPIO_WritePin(MOT1_GPIO_Port, MOT1_Pin, 1);
			HAL_GPIO_WritePin(MOT2_GPIO_Port, MOT2_Pin, 1);
			HAL_GPIO_WritePin(MOT3_GPIO_Port, MOT3_Pin, 0);
			HAL_Delay(STEP_RATE);
			break;
		case 2:
			HAL_GPIO_WritePin(MOT0_GPIO_Port, MOT0_Pin, 0);
			HAL_GPIO_WritePin(MOT1_GPIO_Port, MOT1_Pin, 0);
			HAL_GPIO_WritePin(MOT2_GPIO_Port, MOT2_Pin, 1);
			HAL_GPIO_WritePin(MOT3_GPIO_Port, MOT3_Pin, 1);
			HAL_Delay(STEP_RATE);
			break;
		case 3:
			HAL_GPIO_WritePin(MOT0_GPIO_Port, MOT0_Pin, 1);
			HAL_GPIO_WritePin(MOT1_GPIO_Port, MOT1_Pin, 0);
			HAL_GPIO_WritePin(MOT2_GPIO_Port, MOT2_Pin, 0);
			HAL_GPIO_WritePin(MOT3_GPIO_Port, MOT3_Pin, 1);
			HAL_Delay(STEP_RATE);
			break;
		}
	}
	disableMotor();		//finiti i quarti di giro spengo il motore
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, 0);
	//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
	return 0;
}

void disableMotor() {
	HAL_GPIO_WritePin(MOT0_GPIO_Port, MOT0_Pin, 0);
	HAL_GPIO_WritePin(MOT1_GPIO_Port, MOT1_Pin, 0);
	HAL_GPIO_WritePin(MOT2_GPIO_Port, MOT2_Pin, 0);
	HAL_GPIO_WritePin(MOT3_GPIO_Port, MOT3_Pin, 0);
}

void printFeeding() {
	ssd1306_Fill(0);
	ssd1306_SetCursor(20, 25);
	ssd1306_WriteString("Feeding..", Font_11x18, 1);
	ssd1306_UpdateScreen();
}
