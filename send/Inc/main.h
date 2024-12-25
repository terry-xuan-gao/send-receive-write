/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BSP_LED0_Pin GPIO_PIN_13
#define BSP_LED0_GPIO_Port GPIOC
#define ADC_Fz_Pin GPIO_PIN_1
#define ADC_Fz_GPIO_Port GPIOA
#define ADC_Fx_Pin GPIO_PIN_2
#define ADC_Fx_GPIO_Port GPIOA
#define ADC_Fy_Pin GPIO_PIN_3
#define ADC_Fy_GPIO_Port GPIOA
#define ADC_T_Pin GPIO_PIN_4
#define ADC_T_GPIO_Port GPIOA
#define REF0_7v_Pin GPIO_PIN_5
#define REF0_7v_GPIO_Port GPIOA
#define REF1_5v_Pin GPIO_PIN_6
#define REF1_5v_GPIO_Port GPIOA
#define SPI1_CE_Pin GPIO_PIN_15
#define SPI1_CE_GPIO_Port GPIOA
#define SPI1_CSN_Pin GPIO_PIN_6
#define SPI1_CSN_GPIO_Port GPIOB
#define SPI1_IRQ_Pin GPIO_PIN_7
#define SPI1_IRQ_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
