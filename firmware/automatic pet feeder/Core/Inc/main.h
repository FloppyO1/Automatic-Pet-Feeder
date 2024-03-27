/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "DS3231lib.h"		//per la struttura TIME
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
//meal
typedef struct {
	uint8_t type; //tipo di pasto (imposta la quantità) [0=non attivo, 1=break, 2=lunch, 3=dinner]
	uint8_t hour;	//ora del pasto
	uint8_t min;	//minuti del pasto
	uint8_t done;	//pasto fatto: 1 si, 0 no
} Meal;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
//funzioni generali
void dateStamp();		//stampa la data N.B. da mette prima un imposta cursore
void PrintNumber(int n, uint8_t cifre, uint8_t colore);	//numero da printare, cifre max numero, colore [0/1]s
uint8_t mealCheck();		//controlla se è arrivata l'ora di qualache pasto
void printGear(uint8_t x, uint8_t y);//disegna l'ingrnaggio delle impostazioni nella posizione x,y
void printMealType(Meal m, uint8_t mealN);//stamapa il tipo di pasto mealN serve per inicare quale dei 3 pasti è
void printMealStatus(Meal m, uint8_t mealN);//stamapa se il pasto è attivo (mealN è il numero del pasto passato)
void printMealTime(Meal m, uint8_t mealN);		//stamapa l'ora del pasto e
void printTime(TIME T);			//scrive l'ora in formato hh:mm:ss
//funzioni menu
void menuStart();		//menu di avvio					[0]
void menuMain();		//menu principale 				[1]
void menuSettings();	//menu impostazioni 			[2]
void menuSetMeal();		//menu impostazioni pasto 		[3]
void menuSetType();		//menu impostazioni tipo pasto 	[4]
void menuSetTime();		//menu di impostazione del tempo[5]
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_LED_Pin GPIO_PIN_15
#define USER_LED_GPIO_Port GPIOC
#define ENTER_B_Pin GPIO_PIN_5
#define ENTER_B_GPIO_Port GPIOA
#define ENTER_B_EXTI_IRQn EXTI4_15_IRQn
#define MINUS_B_Pin GPIO_PIN_6
#define MINUS_B_GPIO_Port GPIOA
#define MINUS_B_EXTI_IRQn EXTI4_15_IRQn
#define PLUS_B_Pin GPIO_PIN_7
#define PLUS_B_GPIO_Port GPIOA
#define PLUS_B_EXTI_IRQn EXTI4_15_IRQn
#define MOT3_Pin GPIO_PIN_0
#define MOT3_GPIO_Port GPIOB
#define BUZZER_Pin GPIO_PIN_11
#define BUZZER_GPIO_Port GPIOA
#define MOT2_Pin GPIO_PIN_12
#define MOT2_GPIO_Port GPIOA
#define MOT1_Pin GPIO_PIN_13
#define MOT1_GPIO_Port GPIOA
#define MOT0_Pin GPIO_PIN_3
#define MOT0_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
