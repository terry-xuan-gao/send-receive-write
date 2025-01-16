/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//用户头文件
#include "sys.h"
#include "delay.h"
#include "24l01.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//私有类型定义

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//私有定义

#define a ox1;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//私有宏

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//私有变量
u8 tmp_buf[20]= {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19};
u8 rv_data[32];
u8 mode=0;//模式选择 0发送模式 1接收模式
u8 status_tx=0;
u8 status_rx=0;
//uint16_t 表示数据范围则是0~65535。
uint16_t ADC_Value[120];
uint16_t adc_aver[10];
uint8_t  send_val[10];
uint8_t  hight_part[10];
uint8_t  lower_part[10];	
uint16_t sumref7=0,sumref15=0;
uint16_t count1=0,count2=0;
	
unsigned char Send[20] ={19, 'f', 'a','a','a','a','b','b','b','b','c','c','c','c','d','d','d','d','e',' '}; 
unsigned char SendFail[9] ={0x08,'s','e','n','d','f','a', '\r', ' '}; 

uint32_t count_send_fail=0;
u8 statusCheck=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//私有函数原型

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

    uint8_t temp[1]= {0x11};
    HAL_UART_Transmit(&huart1,temp,1,2);
	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_SPI1_Init();
	MX_USART1_UART_Init();
	MX_ADC1_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */
	//初始化nrf
    NRF24L01_Init();  //初始化NRF24L01
    if(NRF24L01_Check())//函数返回值为1说明检测24L01错误	！！！
    {
        printf("\n\r nrf测试失败 nrf未连接\n\r");
    }
		
	//初始化adc相关
    HAL_ADCEx_Calibration_Start(&hadc1);//校准
    HAL_Delay(10);
    if(HAL_TIM_Base_Start_IT(&htim3)!=HAL_OK) 
		printf("\r\n******** 定时器打开失败 ********\r\n\r\n");
    //开启dma传输，传输60个值 每10个取平均值，共6个有效数据
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Value, 120);

	
    if(NRF24L01_Check()==0)
    {
        printf("\n\r nrf测试成功 nrf连接\n\r");
			  SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_16);
        
		//发送模式
        if(mode == 0)
        {
            printf("\n\r 发送模式\n\r");
            NRF24L01_TX_Mode();//将射频模块配置为发射模式
        }
    }

	
    while (1)
    {
		
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

char digit_to_char(int digit) {
    return digit + '0';
}


void convert() {
	
	int offset = 2;
    
	for (int i = 0; i < 4; i++) {

        //uint8_t high_byte = (adc_aver[i] >> 8) & 0xFF;
        uint16_t byte = adc_aver[i];

        Send[offset + 4 * i ] = digit_to_char(byte / 1000);
        Send[offset + 4 * i + 1] = digit_to_char((byte / 100) % 10);

        Send[offset + 4 * i + 2] = digit_to_char((byte / 10) % 10);
        Send[offset + 4 * i + 3] = digit_to_char(byte % 10);
		
    }
}

/* USER CODE BEGIN 4 */
//DMA传输完成中断回调函数   没有开启dma中断  耗时
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

	for(int i = 0; i < 8; i ++)
		adc_aver[i] = 0;

	// 仍然是10次求和，但没有加入参考电压（即 sumref7 sumref15）和 7000
	// by 高璇
		int times = 9;
//		float t1 = 0.0;
//		float t2 = 0.0;
		for(int i = 0; i < 4; i ++)
		{
			for(int j = 0; j < times; j ++)
			{
				adc_aver[i] += (ADC_Value[i + j*6]/times);
			}
			
			hight_part [i] = adc_aver [i] / 255; 
			lower_part [i] = adc_aver [i] % 255;
			send_val[i * 2] = hight_part[i];
			send_val[i * 2 + 1] = lower_part[i];
			
			adc_aver[i] = ADC_Value[i];
			
		}
		convert();
			
	status_tx=NRF24L01_TxPacket((uint8_t *)Send);
	if(status_tx != 0x20){
		//status_tx=NRF24L01_TxPacket((uint8_t *)SendFail);
	}
}
/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
//定时器全局中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

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
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
