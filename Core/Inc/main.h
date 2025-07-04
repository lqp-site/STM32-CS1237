/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern uint8_t flag_sign;
extern uint32_t data_zero;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
#define USE_PRINTF_MSG      0
#define CODE_TEST           0      //≥Ã–Ú≤‚ ‘

#define LED_OUT(x)              HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, (GPIO_PinState)(x))
#define CS1237_SCK_OUT(x)       HAL_GPIO_WritePin(CS1237_SCK_GPIO_Port, CS1237_SCK_Pin, (GPIO_PinState)(x))
#define CS1237_SDA_OUT(x)       HAL_GPIO_WritePin(CS1237_SDA_GPIO_Port, CS1237_SDA_Pin, (GPIO_PinState)(x))
#define CS1237_SDA_READ         HAL_GPIO_ReadPin(CS1237_SDA_GPIO_Port,CS1237_SDA_Pin)
#define ENDSTOP_DATA_OUT(x)     HAL_GPIO_WritePin(DATA_OUT_GPIO_Port, DATA_OUT_Pin, (GPIO_PinState)(x))
#define ENDSTOP_DATA_IN         HAL_GPIO_ReadPin(DATA_IN_GPIO_Port,DATA_IN_Pin)

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_3
#define LED_GPIO_Port GPIOA
#define CS1237_SCK_Pin GPIO_PIN_5
#define CS1237_SCK_GPIO_Port GPIOA
#define CS1237_SDA_Pin GPIO_PIN_7
#define CS1237_SDA_GPIO_Port GPIOA
#define DATA_IN_Pin GPIO_PIN_15
#define DATA_IN_GPIO_Port GPIOB
#define DATA_OUT_Pin GPIO_PIN_8
#define DATA_OUT_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
