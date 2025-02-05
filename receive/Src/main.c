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
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//用户头文件
#include "sys.h"
#include "delay.h"
#include "24l01.h"
#include "stdio.h"
#include "usbd_cdc_if.h"
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
u8 tmp_buf[30]= {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x30};
u8 rv_data[32];
u8 mode=1;//发送模式还是接收模式选择 0发送模式 1接收模式
u8 status_tx=0;
u8 status_rx=0;
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
    USB_ReEnum(); // Toggle the level of GpIO_A_12 to avoid extract-plug action
    uint8_t temp[1]= {0x11};
    HAL_UART_Transmit(&huart1,temp,1,2);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
    NRF24L01_Init();  //初始化NRF24L01
    while(NRF24L01_Check())//函数返回值为1说明检测24L01错误	
    {
        printf("\n\r nrf测试失败 nrf未连接\n\r");
			break;
    }

    while(NRF24L01_Check()==0)//之前是while
    {
        //printf("\n\r nrf测试成功 nrf连接\n\r");
        //Rx接收模式
        if(mode==1)
        {
            int k=0;
            int Rec_Length=0;
            //printf("\n\r 接收模式\n\r");
            NRF24L01_RX_Mode();
			SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_16);//该语句在例程中是在RxPacket函数中，每次循环都需要执行一次。现放在这里。只初始化，不循环
            while(1)
            {
							//接收数组是UserTxBufferFS，这里使用的是指针；*UserTxBufferFS指的是数组的首地址
							//接收函数的返回值：0——接收完成；其他-存在错误；
                status_rx  = NRF24L01_RxPacket((uint8_t *)(UserTxBufferFS)+Rec_Length);

//				status_rx  = NRF24L01_RxPacket(rv_data);//一次传输32个字节。接收的数据保存在rv_data数组中。返回值0为接收成功
//				HAL_Delay (1);

                
				/*if(status_rx==0)
				{
					Rec_Length+=20;
					if(Rec_Length>=18)//UserTxBufferFS数组是1000个元素
					{
						CDC_Transmit_FS(UserTxBufferFS, 20);//虚拟usb发送数据
						Rec_Length=0;
					}
					k++;
					CDC_Transmit_FS(&k, sizeof(k));//将输出值改为UserTxBufferFS数组里的数据
				}*/
				
				/*if(status_rx==0)
                {
                    Rec_Length+=20;
                    if(Rec_Length>=18)//UserTxBufferFS数组是1000个元素
                    {
						CDC_Transmit_FS(UserTxBufferFS,20);//虚拟usb发送数据
                        Rec_Length=0;
                    }
                    k++;
                    printf("%dE",UserTxBufferFS[0]);
                }*/
				
				
				if(status_rx==0)
				{
					Rec_Length+=20;
					if(Rec_Length>=18)//UserTxBufferFS数组是1000个元素
					{
						CDC_Transmit_FS(UserTxBufferFS,20);//虚拟usb发送数据
						Rec_Length=0;
					}
					k++;
					for (int i = 0; i < 8; i++) {
						printf("%u,", UserTxBufferFS[i]); // 将输出值改为UserTxBufferFS数组里的前八个元素

					}
					printf("E");
				}
            }
			
//				break;
        }
        //tx发送模式
        if(mode ==0)
        {
            printf("\n\r 发送模式\n\r");
            for(int i=0; i<101; i++)
            {
                //tx发送模式
                NRF24L01_TX_Mode();
                HAL_Delay(2);
                status_tx=NRF24L01_TxPacket(tmp_buf);//函数返回值为发送完成状态
                printf("发送状态为%d,%d",status_tx,i+1);//输出发送状态
            }
        }
//		break ;
    }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
//		HAL_Delay(300);
//		CDC_Transmit_FS(UserTxBufferFS,10);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
