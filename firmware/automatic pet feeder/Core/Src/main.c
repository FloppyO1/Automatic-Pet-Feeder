/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "bitmap.h"
#include "DS3231lib.h"
#include "EEPROM.h"
#include "storeSettings.h"
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define TIME_STBY 10		//tempo in s per lo standby dello schermo (riduce lumnosità)
#define CONTRAST_STBY 5	//contrasto quando lo schermo va in standby
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim14;

/* USER CODE BEGIN PV */
//variabili per controllo tempo senza delay
uint32_t buzT;			//tempo buzzer in millisecondi
uint32_t contrastT;	//tempo contrasto per riduzione della luminosità dopo tot tempo senza premere qualche pulsanto
//variabili generali
uint8_t buz = 0;		//attiva disattiva buzzer
uint8_t feed = 0;//flag che segna se è da dare una porzione o meno: 0 no, altrimenti numero porzioni da dare
uint8_t status = 1;		//attiva o disattiva il dispositivo
uint8_t color = 1;		//colore per lampeggio per selezione
//variabili menu
uint8_t menu = 0;		//menu visualizzazione attivo
uint8_t lineSel = 0;//indica la linea selezionata, se 0 vuol dire torna a menu principale (per i menu: settings, meal settings, type settings)
uint8_t timeSel = 0;//0 se non da modificare, 1 se ora selezionata, 2 se minuti selezionati
uint8_t qtySel = 0;	//0 se non da modificare, 1 se da modificare
//variabili tempo
TIME time;			//variabile che tiene traccia del tempo reale
TIME timetoset;		//ora da impostare
uint8_t setTime = 0;	//se a 1 bisogna impostare il l'ora
//variabili tipi pasto
uint8_t breakfast;	//quantità pasto colazione
uint8_t lunch;		//quantità pasto pranzo
uint8_t dinner;		//quantità pasto cena
uint8_t pastoDaServ = 0;		//0 nessuno, 1 m1, 2 m2, 3 m3
uint8_t storeSet = 0;	//se 1 bisogna salvare nella eeprom le impostazioni

//creazione dei 3 pasti
Meal m1;
Meal m2;
Meal m3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM14_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
	ssd1306_Init();				//inizializzo il display
	//HAL_TIM_PWM_Init(&htim1);	//inizializzo il pwm per il buzzer
	//TIM1->CCR4 = 0;				//imposto il duty a 1%
	HAL_TIM_Base_Start_IT(&htim14);

	//reset del contrasto
	contrastT = HAL_GetTick() + TIME_STBY * 1000;

	//set del tempo da impostare
	timetoset = Get_Time(&hi2c1);

	//caricamento dalla memoria eeprom delle impostazioni

	//recupero dei 3 pasti dalla eeprom
	getSettings(&m1, &m2, &m3, &breakfast, &lunch, &dinner);//recuoera le impostazioni salvate nella eeprom

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		if (setTime == 1) {
			Set_Time(timetoset.seconds, timetoset.minutes, timetoset.hour, 1, 1,
					1, 2023, &hi2c1);
			ssd1306_Fill(0);
			ssd1306_SetCursor(9, 23);
			ssd1306_WriteString("TIME SAVED", Font_11x18, 1);
			ssd1306_UpdateScreen();
			HAL_Delay(500);
			setTime = 0;
		}

		if (storeSet == 1) {
			storeSettings(m1, m2, m3, breakfast, lunch, dinner);// salva tutte le impostazioni nella memoria eeprom
			ssd1306_Fill(0);
			ssd1306_SetCursor(4, 23);
			ssd1306_WriteString("SETT.STORED", Font_11x18, 1);
			ssd1306_UpdateScreen();
			HAL_Delay(500);
			storeSet = 0;
		}

		HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
		if (menu != 5) {
			time = Get_Time(&hi2c1);
		}

		//spegnimento buzzer dopo averlo acceso nella ISR del bottone
		if (buz == 1 && (buzT <= HAL_GetTick())) {
			//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
			//TIM1->CCR4 = 0;
		}

		//controllo menu da visuallizzare
		switch (menu) {
		case 0:
			menuStart();	//menu avvio
			//ssd1306_UpdateScreen();
			break;
		case 1:
			menuMain();		//menu principale
			//ssd1306_UpdateScreen();
			break;
		case 2:
			menuSettings();		//menu impostazioni
			//ssd1306_UpdateScreen();
			break;
		case 3:
			menuSetMeal();		//menu impostazioni meal
			//ssd1306_UpdateScreen();
			break;
		case 4:
			menuSetType();		//menu impostazioni type
			//ssd1306_UpdateScreen();
			break;
		case 5:
			menuSetTime();		//menu impostazioni time
			//ssd1306_UpdateScreen();
			break;
		}
		//aggiorno il display
		ssd1306_UpdateScreen();

		//controlla se bisogna dare da mangiare. Se mealCheck ritorna 0 vol dire che non c'è da servire niente, altrimenti ritorna il tipo
		if (status == 1 && menu != 5) {
			if(feed != 0){
				ssd1306_SetContrast(255);
				contrastT = HAL_GetTick() + TIME_STBY * 1000;
				//HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_4);
				printFeeding();
				feed = quatrerTurn(feed);
			}
			switch (mealCheck()) {	//controlla le impostazioni e le estrae.
			case 1:
				feed = 2 * breakfast;
				ssd1306_SetContrast(255);
				contrastT = HAL_GetTick() + TIME_STBY * 1000;
				printFeeding();
				feed = quatrerTurn(feed);
				break;
			case 2:
				feed = 2 * lunch;
				ssd1306_SetContrast(255);
				contrastT = HAL_GetTick() + TIME_STBY * 1000;
				printFeeding();
				feed = quatrerTurn(feed);
				break;
			case 3:
				feed = 2 * dinner;
				ssd1306_SetContrast(255);
				contrastT = HAL_GetTick() + TIME_STBY * 1000;
				printFeeding();
				feed = quatrerTurn(feed);
				break;
			}
		}

		//quando è mezza notte resetto tutti gli stati dei pasti
		if (time.hour == 0 && time.minutes == 0 && time.seconds == 0) {	//quando arriva mezza notte resetta lo stato di ogni pasto
			m1.done = 0;
			m2.done = 0;
			m3.done = 0;
		}

		//diminuizione e aumento contrasto display dopo inutilizzo e prima dei pasti
		if (contrastT <= HAL_GetTick()) {
			ssd1306_SetContrast(CONTRAST_STBY);
		} else {
			ssd1306_SetContrast(255);
		}

	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00303D5B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 1600-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 5000-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, MOT3_Pin|MOT0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BUZZER_Pin|MOT2_Pin|MOT1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_LED_Pin */
  GPIO_InitStruct.Pin = USER_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USER_LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ENTER_B_Pin MINUS_B_Pin PLUS_B_Pin */
  GPIO_InitStruct.Pin = ENTER_B_Pin|MINUS_B_Pin|PLUS_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MOT3_Pin MOT0_Pin */
  GPIO_InitStruct.Pin = MOT3_Pin|MOT0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : BUZZER_Pin MOT2_Pin MOT1_Pin */
  GPIO_InitStruct.Pin = BUZZER_Pin|MOT2_Pin|MOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void menuSetTime() {
	//display
	//title
	ssd1306_Fill(0);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("TIME SETT.", Font_11x18, 1);
	//legenda
	ssd1306_SetCursor(3, 19);
	ssd1306_WriteString("      h  m", Font_7x10, 1);

	//tempo
	//scritta tempo
	ssd1306_SetCursor(0, 29);
	ssd1306_WriteString("time: ", Font_7x10, 1);
	//print ore
	if (timetoset.hour < 10) {
		if (lineSel == 1) {
			ssd1306_WriteChar('0', Font_7x10, color);
		} else {
			ssd1306_WriteChar('0', Font_7x10, 1);
		}
	}
	if (lineSel == 1) {
		PrintNumber(timetoset.hour, 2, color);
	} else {
		PrintNumber(timetoset.hour, 2, 1);
	}

	//print minuti
	ssd1306_WriteChar(':', Font_7x10, 1);
	if (timetoset.minutes < 10) {
		if (lineSel == 2) {
			ssd1306_WriteChar('0', Font_7x10, color);
		} else {
			ssd1306_WriteChar('0', Font_7x10, 1);
		}
	}
	if (lineSel == 2) {
		PrintNumber(timetoset.minutes, 2, color);
	} else {
		PrintNumber(timetoset.minutes, 2, 1);
	}
	/*
	 //print secondi
	 ssd1306_WriteChar(':', Font_7x10, 1);
	 if (time.seconds < 10) {
	 ssd1306_WriteChar('0', Font_7x10, 1);
	 }
	 PrintNumber(time.seconds, 2, 1);
	 */

	printGear(118, 54);
}
void menuSetType() {
	//display
	//title
	ssd1306_Fill(0);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("TYPE SETT.", Font_11x18, 1);
	//legenda
	ssd1306_SetCursor(0, 19);
	ssd1306_WriteString("TYPE    QTY 1-20", Font_7x10, 1);
	//tipo breakfast
	ssd1306_SetCursor(0, 29);
	if (qtySel == 0 && lineSel == 1) {//lampeggia se la quantità NON è da moficare e la linea è la prima
		ssd1306_WriteString("break", Font_7x10, color);
	} else {
		ssd1306_WriteString("break", Font_7x10, 1);
	}
	ssd1306_WriteString("      ", Font_7x10, 1);
	if (qtySel == 1 && lineSel == 1) {//lampeggia se la quantità è da moficare e la linea è la prima
		PrintNumber(breakfast, 2, color);
	} else {
		PrintNumber(breakfast, 2, 1);
	}
	//tipo lunch
	ssd1306_SetCursor(0, 39);
	if (qtySel == 0 && lineSel == 2) {//lampeggia se la quantità NON è da moficare e la linea è la prima
		ssd1306_WriteString("lunch", Font_7x10, color);
	} else {
		ssd1306_WriteString("lunch", Font_7x10, 1);
	}
	ssd1306_WriteString("      ", Font_7x10, 1);
	if (qtySel == 1 && lineSel == 2) {//lampeggia se la quantità è da moficare e la linea è la prima
		PrintNumber(lunch, 2, color);
	} else {
		PrintNumber(lunch, 2, 1);
	}
	//tipo dinner
	ssd1306_SetCursor(0, 49);
	if (qtySel == 0 && lineSel == 3) {//lampeggia se la quantità NON è da moficare e la linea è la prima
		ssd1306_WriteString("dinner", Font_7x10, color);
	} else {
		ssd1306_WriteString("dinner", Font_7x10, 1);
	}
	ssd1306_WriteString("     ", Font_7x10, 1);
	if (qtySel == 1 && lineSel == 3) {//lampeggia se la quantità è da moficare e la linea è la prima
		PrintNumber(dinner, 2, color);
	} else {
		PrintNumber(dinner, 2, 1);
	}

	printGear(118, 0);
}

void menuSetMeal() {
	//display
	//title
	ssd1306_Fill(0);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("MEAL SETT.", Font_11x18, 1);
	//legenda
	ssd1306_SetCursor(0, 19);
	ssd1306_WriteString("TYPE  ACTIVE  TIME", Font_7x10, 1);
	//pasti
	//m1
	ssd1306_SetCursor(0, 29);
	printMealType(m1, 1);
	printMealStatus(m1, 1);
	ssd1306_SetCursor(93, 29);
	printMealTime(m1, 1);

	//m2
	ssd1306_SetCursor(0, 39);
	printMealType(m2, 2);
	printMealStatus(m2, 2);
	ssd1306_SetCursor(93, 39);
	printMealTime(m2, 2);

	//m3
	ssd1306_SetCursor(0, 49);
	printMealType(m3, 3);
	printMealStatus(m3, 3);
	ssd1306_SetCursor(93, 49);
	printMealTime(m3, 3);

	printGear(118, 0);

	//disegno cursore
}

void menuSettings() {
	//display
	//title
	ssd1306_Fill(0);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("SETTINGS", Font_11x18, 1);
	//linea 1
	ssd1306_SetCursor(0, 19);
	ssd1306_WriteString("MEAL SET", Font_7x10, 1);
	//linea 2
	ssd1306_SetCursor(0, 29);
	ssd1306_WriteString("TYPE SET", Font_7x10, 1);
	//linea 3
	ssd1306_SetCursor(0, 39);
	ssd1306_WriteString("TIME SET", Font_7x10, 1);
	//linea 4
	ssd1306_SetCursor(0, 49);
	ssd1306_WriteString("ACTIVE?", Font_7x10, 1);//DA DECIDERE COME FARE LA DISATTIVAZIONE DEL DISPOSITIVO
	if (lineSel != 4) {
		if (status == 1) {
			ssd1306_DrawBitmap(60, 49, attivo, 10, 10, 1);
		} else {
			ssd1306_DrawBitmap(60, 49, nonAttivo, 10, 10, 1);
		}
	}

	//disegno cursore linea selezionata
	switch (lineSel) {
	case 1:
		ssd1306_SetCursor(60, 19);
		ssd1306_WriteChar('<', Font_7x10, 1);
		break;
	case 2:
		ssd1306_SetCursor(60, 29);
		ssd1306_WriteChar('<', Font_7x10, 1);
		break;
	case 3:
		ssd1306_SetCursor(60, 39);
		ssd1306_WriteChar('<', Font_7x10, 1);
		break;
	case 4:
		if (status == 1) {
			if (!color) {
				ssd1306_DrawBitmap(60, 49, attivo, 10, 10, 1);
			} else {
				ssd1306_DrawBitmap(60, 49, attivoInv, 10, 10, 1);
			}
		} else {
			if (!color) {
				ssd1306_DrawBitmap(60, 49, nonAttivo, 10, 10, 1);
			} else {
				ssd1306_DrawBitmap(60, 49, nonAttivoInv, 10, 10, 1);
			}
		}
		break;
	}
	printGear(118, 54);
}

void menuMain() {
	ssd1306_Fill(0);
	//write time
	ssd1306_Line(0, 50, 128, 50, 1);
	ssd1306_SetCursor(0, 54);
	printTime(time);
	//grafical thigs
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("MEALS...", Font_11x18, 1);
	//settings button
	printGear(118, 54);

	//stampa tutti i pasti
	ssd1306_SetCursor(0, 19);
	printMealType(m1, 1);
	ssd1306_SetCursor(55, 19);
	printMealTime(m1, 1);
	ssd1306_SetCursor(0, 29);
	printMealType(m2, 2);
	ssd1306_SetCursor(55, 29);
	printMealTime(m2, 2);
	ssd1306_SetCursor(0, 39);
	printMealType(m3, 3);
	ssd1306_SetCursor(55, 39);
	printMealTime(m3, 3);
	/*	non va bene perchè c'è qualche bug sulla comparazione del tempo
	 //cancello quelli fatti
	 if (!(((m1.hour <= time.hour) && (m1.min <= time.minutes)))) {
	 //ssd1306_Line(0, 22, 100, 22, 1);
	 ssd1306_SetCursor(0, 19);
	 printMealType(m1, 1);
	 ssd1306_SetCursor(55, 19);
	 printMealTime(m1, 1);
	 } else {
	 m1.done = 1;
	 }

	 if (!(((m2.hour <= time.hour) && (m2.min <= time.minutes)))) {
	 //ssd1306_Line(0, 32, 100, 32, 1);
	 ssd1306_SetCursor(0, 29);
	 printMealType(m2, 2);
	 ssd1306_SetCursor(55, 29);
	 printMealTime(m2, 2);
	 } else {
	 m2.done = 1;
	 }

	 if (!(((m3.hour <= time.hour) && (m3.min <= time.minutes)))) {
	 //ssd1306_Line(0, 42, 100, 42, 1);
	 ssd1306_SetCursor(0, 39);
	 printMealType(m3, 3);
	 ssd1306_SetCursor(55, 39);
	 printMealTime(m3, 3);
	 } else {
	 m2.done = 1;
	 }
	 */
	if (m1.done != 0) {
		ssd1306_Line(0, 22, 100, 22, 1);
	}
	if (m2.done != 0) {
		ssd1306_Line(0, 32, 100, 32, 1);
	}
	if (m3.done != 0) {
		ssd1306_Line(0, 42, 100, 42, 1);
	}

	//entra in sleep mode
//	HAL_SuspendTick();
//	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void menuStart() {
	ssd1306_SetCursor(45, 18);
	ssd1306_WriteString("PET", Font_11x18, 1);
	ssd1306_SetCursor(31, 37);
	ssd1306_WriteString("FEEDER", Font_11x18, 1);
	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("made by Floppy Lab", Font_7x10, 1);
	ssd1306_UpdateScreen();
	HAL_Delay(2000);
	menu = 1;
}

void printTime(TIME T) {
	ssd1306_WriteString("time: ", Font_7x10, 1);
	//print ore
	if (T.hour < 10) {
		ssd1306_WriteChar('0', Font_7x10, 1);
	}
	PrintNumber(T.hour, 2, 1);

	//print minuti
	ssd1306_WriteChar(':', Font_7x10, 1);
	if (T.minutes < 10) {
		ssd1306_WriteChar('0', Font_7x10, 1);
	}
	PrintNumber(T.minutes, 2, 1);

	//print secondi
	ssd1306_WriteChar(':', Font_7x10, 1);
	if (T.seconds < 10) {
		ssd1306_WriteChar('0', Font_7x10, 1);
	}
	PrintNumber(T.seconds, 2, 1);

}

void printGear(uint8_t x, uint8_t y) {
	//disegno ingranaggio
	if (lineSel == 0 && color == 0) {
		ssd1306_DrawBitmap(x, y, ingranaggioInv, 10, 10, 1);
	} else {
		ssd1306_DrawBitmap(x, y, ingranaggio, 10, 10, 1);
	}
}

void printMealType(Meal m, uint8_t mealN) {
	uint8_t c;
	if (lineSel == mealN) {
		c = color;
	} else {
		c = 1;
	}
	switch (m.type) {
	case 0:
		ssd1306_WriteString("-----", Font_7x10, c);
		break;
	case 1:
		ssd1306_WriteString("break", Font_7x10, c);
		break;
	case 2:
		ssd1306_WriteString("lunch", Font_7x10, c);
		break;
	case 3:
		ssd1306_WriteString("dinner", Font_7x10, c);
		break;
	}
}

void printMealStatus(Meal m, uint8_t mealN) {
	if (m.type != 0) {
		if (lineSel == mealN + 3 && !color) {
			ssd1306_DrawBitmap(59, 29 + (mealN - 1) * 10, attivoInv, 10, 10, 1);
		} else {
			ssd1306_DrawBitmap(59, 29 + (mealN - 1) * 10, attivo, 10, 10, 1);
		}
	} else {
		if (lineSel == mealN + 3 && !color) {
			ssd1306_DrawBitmap(59, 29 + (mealN - 1) * 10, nonAttivoInv, 10, 10,
					1);
		} else {
			ssd1306_DrawBitmap(59, 29 + (mealN - 1) * 10, nonAttivo, 10, 10, 1);
		}
	}
}

void printMealTime(Meal m, uint8_t mealN) {
	if (timeSel == 0 && lineSel == mealN + 6) {
		//print ore
		if (m.hour < 10) {
			ssd1306_WriteChar('0', Font_7x10, color);
		}
		PrintNumber(m.hour, 2, color);

		//print minuti
		ssd1306_WriteChar(':', Font_7x10, color);
		if (m.min < 10) {
			ssd1306_WriteChar('0', Font_7x10, color);
		}
		PrintNumber(m.min, 2, color);
	} else {
		//print ore
		if (m.hour < 10) {
			if (!(timeSel == 1 && lineSel == mealN + 6 && !color)) {
				ssd1306_WriteChar('0', Font_7x10, 1);
			} else {
				ssd1306_WriteChar('0', Font_7x10, 0);
			}
		}
		if (!(timeSel == 1 && lineSel == mealN + 6 && !color)) {
			PrintNumber(m.hour, 2, 1);
		} else {
			PrintNumber(m.hour, 2, 0);
		}

		ssd1306_WriteChar(':', Font_7x10, 1);

		//print minuti
		if (m.min < 10) {
			if (!(timeSel == 2 && lineSel == mealN + 6 && !color)) {
				ssd1306_WriteChar('0', Font_7x10, 1);
			} else {
				ssd1306_WriteChar('0', Font_7x10, 0);
			}
		}
		if (!(timeSel == 2 && lineSel == mealN + 6 && !color)) {
			PrintNumber(m.min, 2, 1);
		} else {
			PrintNumber(m.min, 2, 0);
		}
	}
}

uint8_t mealCheck() {	//aggiungere tanti if quanti i pasti 'meal'
	if (time.hour == m1.hour && time.minutes == m1.min && time.seconds == 0
			&& m1.done == 0) {
		pastoDaServ = 1;
		m1.done = 1;
		return m1.type;
	}
	if (time.hour == m2.hour && time.minutes == m2.min && time.seconds == 0
			&& m2.done == 0) {
		pastoDaServ = 2;
		m2.done = 1;
		return m2.type;
	}
	if (time.hour == m3.hour && time.minutes == m3.min && time.seconds == 0
			&& m3.done == 0) {
		pastoDaServ = 3;
		m3.done = 1;
		return m3.type;
	}
	return 0;//se viene ritornato 0 vuol dire che nessun pasto deve essere servito
}

void PrintNumber(int n, uint8_t cifre, uint8_t colore) {//numero da printare, cifre max numero, colore [0/1]
	char s[cifre + 1];
	itoa(n, s, 10); //converte int to string.  char* itoa(int value, char* str, int base);
	/*for (uint8_t i = 0; i < cifre; i++) {
	 ssd1306_WriteChar(' ', Font_7x10, 1);
	 }*/
	ssd1306_WriteString(s, Font_7x10, colore);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
	if (htim == &htim14) {
		//aggiorno il colore del selezionato ogni mezzo secondo
		color = (color + 1) % 2;
	}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
//	HAL_ResumeTick();	//tolto per sleepmode
	if(feed !=0){
		return;
	}
	//PULSANTE ENTER/OK
	if (GPIO_Pin == ENTER_B_Pin) {
		contrastT = HAL_GetTick() + TIME_STBY * 1000;
		switch (menu) {		//controllo quale menu è attivo
		//menu principale attivo
		case 1:
			menu = 2;		//entra nel menu impostazioni
			lineSel = 1;	//imposta la prima linea selezionata
			break;
			//menu impostazioni attivo
		case 2:
			switch (lineSel) {	//controllo quale linea è selezionata
			case 0:
				menu = 1;//ritorna al menu principale se viene cliccato lingranaggio
				lineSel = 0;
				break;
			case 1:
				menu = 3;	//imposta il menu IMPOSTAZIONI MEAL
				lineSel = 1;
				break;
			case 2:
				menu = 4;	//imposta il menu IMPOSTAZIONI TYPE
				lineSel = 1;
				break;
			case 3:
				menu = 5;	//imposta il menu IMPOSTAZIONI TIME
				lineSel = 1;
				break;
			case 4:
				status = (status + 1) % 2;
				break;
			}
			break;
			//menu impostazioni meal attivo
		case 3:
			switch (lineSel) {	//controllo quale linea è selezionata
			case 0:
				storeSet = 1;
				menu = 2;//se sei sull'ingranaggio ritorna al menu precedente (menu impostazioni)
				break;
			case 1:	//scorre tra i vari tipi di pasto senza passare per lo 0 (pasto disattivato)
				if ((m1.type + 1) % 4 == 0) {
					m1.type = 1;
				} else {
					m1.type = (m1.type + 1) % 4;
				}

				break;
			case 2:	//scorre tra i vari tipi di pasto senza passare per lo 0 (pasto disattivato)
				if ((m2.type + 1) % 4 == 0) {
					m2.type = 1;
				} else {
					m2.type = (m2.type + 1) % 4;
				}

				break;
			case 3:	//scorre tra i vari tipi di pasto senza passare per lo 0 (pasto disattivato)
				if ((m3.type + 1) % 4 == 0) {
					m3.type = 1;
				} else {
					m3.type = (m3.type + 1) % 4;
				}
				break;
			case 4:	//scorre tra i vari tipi di pasto senza passare per lo 0 (pasto disattivato)
				if (m1.type != 0) {
					m1.type = 0;
				} else {
					m1.type = (m1.type + 1) % 2;
				}
				break;
			case 5:	//scorre tra i vari tipi di pasto senza passare per lo 0 (pasto disattivato)
				if (m2.type != 0) {
					m2.type = 0;
				} else {
					m2.type = (m2.type + 1) % 2;
				}
				break;
			case 6:	//scorre tra i vari tipi di pasto senza passare per lo 0 (pasto disattivato)
				if (m3.type != 0) {
					m3.type = 0;
				} else {
					m3.type = (m3.type + 1) % 2;
				}
				break;
			case 7:		//sleziona l'ora da impostare se minuti o ore
				timeSel = (timeSel + 1) % 3;
				break;
			case 8:		//sleziona l'ora da impostare se minuti o ore
				timeSel = (timeSel + 1) % 3;
				break;
			case 9:		//sleziona l'ora da impostare se minuti o ore
				timeSel = (timeSel + 1) % 3;
				break;
			}
			break;
			//menu impostazioni type attivo
		case 4:
			switch (lineSel) {
			case 0:
				storeSet = 1;
				menu = 2;// torna al menu impostazioni se premuto l'ingranaggio
				break;
			case 1:
				qtySel = (qtySel + 1) % 2;//seleziona o no la quantità da modificare
				break;
			case 2:
				qtySel = (qtySel + 1) % 2;//seleziona o no la quantità da modificare
				break;
			case 3:
				qtySel = (qtySel + 1) % 2;//seleziona o no la quantità da modificare
				break;
			}
			break;
			//menu impostazioni tempo
		case 5:
			switch (lineSel) {
			case 0:
				timetoset.seconds = 0;
				setTime = 1;
				menu = 2;
				lineSel = 0;
				m1.done = 0;
				m2.done = 0;
				m3.done = 0;
				break;
			case 1:
				lineSel = (lineSel + 1) % 3;
				break;
			case 2:
				lineSel = (lineSel + 1) % 3;
				break;
			}
			break;
		}
	}

	//PULSANTE +
	if (GPIO_Pin == PLUS_B_Pin) {
		contrastT = HAL_GetTick() + TIME_STBY * 1000;
		switch (menu) {		//controllo quale menu è attivo
		//menu principale
		case 1:
			feed = feed + 2;		//aggiunge una porzione
			break;
			//menu impostazioni
		case 2:
			if (lineSel == 0) {
				lineSel = 4;
			} else {
				lineSel--;
			}
			break;
			//menu meal set
		case 3:
			if (timeSel == 0) {
				if (lineSel == 0) {
					lineSel = 9;
				} else {
					lineSel--;
				}
			}
			switch (timeSel) {
			case 1:
				switch (lineSel) {
				case 7:
					m1.hour = (m1.hour + 1) % 24;
					break;
				case 8:
					m2.hour = (m2.hour + 1) % 24;
					break;
				case 9:
					m3.hour = (m3.hour + 1) % 24;
					break;
				}
				break;
			case 2:
				switch (lineSel) {
				case 7:
					m1.min = (m1.min + 5) % 60;
					break;
				case 8:
					m2.min = (m2.min + 5) % 60;
					break;
				case 9:
					m3.min = (m3.min + 5) % 60;
					break;
				}
				break;
			}
			break;
			//menu type set
		case 4:
			if (qtySel != 1) {
				if (lineSel == 0) {
					lineSel = 3;
				} else {
					lineSel = lineSel - 1;
				}
			} else {
				switch (lineSel) {
				case 1:
					if (breakfast < 20) {
						breakfast = breakfast + 1;
					}
					break;
				case 2:
					if (lunch < 20) {
						lunch = lunch + 1;
					}
					break;
				case 3:
					if (dinner < 20) {
						dinner = dinner + 1;
					}
					break;
				}
			}
			break;
			//menu time set
		case 5:
			switch (lineSel) {
			case 1:
				timetoset.hour = (timetoset.hour + 1) % 24;
				break;
			case 2:
				timetoset.minutes = (timetoset.minutes + 1) % 60;
				break;
			}
			break;
		}
	}

//PULSANTE -
	if (GPIO_Pin == MINUS_B_Pin) {
		contrastT = HAL_GetTick() + TIME_STBY * 1000;
		switch (menu) {		//controllo quale menu è attivo
		//menu principale
		case 1:
			break;
			//menu impostazioni
		case 2:
			lineSel = (lineSel + 1) % 5;
			break;
			//menu meal set
		case 3:
			if (timeSel == 0) {
				lineSel = (lineSel + 1) % 10;
			}
			switch (timeSel) {
			case 1:
				switch (lineSel) {
				case 7:
					if (m1.hour == 0) {
						m1.hour = 23;
					} else {
						m1.hour = (m1.hour - 1);
					}
					break;
				case 8:
					if (m2.hour == 0) {
						m2.hour = 23;
					} else {
						m2.hour = (m2.hour - 1);
					}
					break;
				case 9:
					if (m3.hour == 0) {
						m3.hour = 23;
					} else {
						m3.hour = (m3.hour - 1);
					}
					break;
				}
				break;
			case 2:
				switch (lineSel) {
				case 7:
					if (m1.min == 0) {
						m1.min = 55;
					} else {
						m1.min = (m1.min - 5);
					}
					break;
				case 8:
					if (m2.min == 0) {
						m2.min = 55;
					} else {
						m2.min = (m2.min - 5);
					}
					break;
				case 9:
					if (m3.min == 0) {
						m3.min = 55;
					} else {
						m3.min = (m3.min - 5);
					}
					break;
				}
				break;
			}
			break;
			//menu type set
		case 4:
			if (qtySel != 1) {
				lineSel = (lineSel + 1) % 4;
			} else {
				switch (lineSel) {
				case 1:
					if (breakfast > 0) {
						breakfast = breakfast - 1;
					}
					break;
				case 2:
					if (lunch > 0) {
						lunch = lunch - 1;
					}
					break;
				case 3:
					if (dinner > 0) {
						dinner = dinner - 1;
					}
					break;
				}
			}
			break;
			//menu time set
		case 5:
			switch (lineSel) {
			case 1:
				if (timetoset.hour == 0) {
					timetoset.hour = 23;
				} else {
					timetoset.hour = timetoset.hour - 1;
				}
				break;
			case 2:
				if (timetoset.minutes == 0) {
					timetoset.minutes = 59;
				} else {
					timetoset.minutes = timetoset.minutes - 1;
				}
				break;
			}
			break;
		}

	}

		buz = 1;
		buzT = HAL_GetTick() + 10;
		//TIM1->CCR4 = 5;
		//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

//HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
//HAL_GPIO_TogglePin(MOTOR_GPIO_Port, MOTOR_Pin);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
