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
//�û�ͷ�ļ�
#include "sys.h"
#include "delay.h"
#include "24l01.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//˽�����Ͷ���

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//˽�ж���

#define a ox1;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
//˽�к�

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//˽�б���
u8 tmp_buf[20]= {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19};
u8 rv_data[32];
u8 mode=0;//ģʽѡ�� 0����ģʽ 1����ģʽ
u8 status_tx=0;
u8 status_rx=0;
//uint16_t ��ʾ���ݷ�Χ����0~65535��
uint16_t ADC_Value[120];
uint16_t adc_aver[10];
uint8_t  send_val[10];
uint8_t  hight_part[10];
uint8_t  lower_part[10];	
uint16_t sumref7=0,sumref15=0;
uint16_t count1=0,count2=0;

uint32_t count_send_fail=0;
u8 statusCheck=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
//˽�к���ԭ��

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
//void delay(unsigned int time) {
//    unsigned int i, j;
//    for (i = 0; i < time; i++) {
//        for (j = 0; j < 1275; j++) {
//            // ��ʱѭ��
//        }
//    }
//}

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
//��ʼ��nrf
    NRF24L01_Init();  //��ʼ��NRF24L01
    if(NRF24L01_Check())//��������ֵΪ1˵�����24L01����	������
    {
        printf("\n\r nrf����ʧ�� nrfδ����\n\r");
    }
		
//��ʼ��adc���
    HAL_ADCEx_Calibration_Start(&hadc1);//У׼
    HAL_Delay(10);
    if(HAL_TIM_Base_Start_IT(&htim3)!=HAL_OK) 
		printf("\r\n******** ��ʱ����ʧ�� ********\r\n\r\n");
    //����dma���䣬����60��ֵ ÿ10��ȡƽ��ֵ����6����Ч����
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Value, 120);

	
//	if(NRF24L01_Check() == 1)
//	{
//			NRF24L01_Init();
//			NRF24L01_TX_Mode();
//			
//			if(NRF24L01_Check() == 1)
//			{
//				HAL_NVIC_SystemReset(); 
//			}
//		}
//	
//	
	
    if(NRF24L01_Check()==0)
    {
        printf("\n\r nrf���Գɹ� nrf����\n\r");
			  SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_16);
        //Rx����ģʽ
        if(mode == 1)
        {
            printf("\n\r ����ģʽ\n\r");
            for(int i=0; i<34; i++)
            {
                NRF24L01_RX_Mode();//����Ƶģ������Ϊ����ģʽ
                status_rx  = NRF24L01_RxPacket(tmp_buf);//һ�δ���32���ֽڡ����յ����ݱ�����tmp_buf�����С�����ֵ0Ϊ���ճɹ�
                HAL_Delay (1000);
            }
        }
		//����ģʽ
        if(mode == 0)
        {
            printf("\n\r ����ģʽ\n\r");
            NRF24L01_TX_Mode();//����Ƶģ������Ϊ����ģʽ
        }
    }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	
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

/* USER CODE BEGIN 4 */
//DMA��������жϻص�����   û�п���dma�ж�  ��ʱ
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	//��adcת���õ���ֵȡƽ�� ���ڳ˷�����ʱ��̫��������ֻ�Ѷ�Ӧ��10��ֵ�ۼӡ�
	
//	for(int i=0,l=0,k=0;i<16;i++)
//	{
//				adc_aver[i]=ADC_Value[k+l]+ADC_Value[k+4+l]+ADC_Value[k+8+l]+ADC_Value[k+12+l]+ADC_Value[k+16+l]
//		+ADC_Value[k+20+l]+ADC_Value[k+24+l]+ADC_Value[k+28+l]+ADC_Value[k+32+l]+ADC_Value[k+36+l];
//				k++;
//				if(k==4)
//				{
//						l+=40;
//						k=0;
//				}		
//	}	
//		int checkResult = NRF24L01_Check();
//	
//		if(checkResult == 1)
//		{
//			NRF24L01_Init();
//			NRF24L01_TX_Mode();
//			
//			if(NRF24L01_Check() == 1)
//			{
//				HAL_NVIC_SystemReset(); 
//			}
//		}
		//HAL_Delay(10);
     
		for(int i = 0; i < 8; i ++)
			adc_aver[i] = 0;

	// ��Ȼ��10����ͣ���û�м���ο���ѹ���� sumref7 sumref15���� 7000
	// by ���
		int times = 9;
//		float t1 = 0.0;
//		float t2 = 0.0;
		for(int i = 0; i < 4; i ++)
		{
			for(int j = 0; j < times; j ++)
			{
				adc_aver[i] += (ADC_Value[i + j*6]/times);
				//adc_aver[i+4] += (ADC_Value[i + 60 + j*6]/times);
			}
			
			hight_part [i] = adc_aver [i] / 255; 
			lower_part [i] = adc_aver [i] % 255;
			send_val[i * 2] = hight_part[i];
			send_val[i * 2 + 1] = lower_part[i];
			
//			t1 = (float)adc_aver[i] * 255.0 / 4096.0;
//			send_val[i] = (uint8_t)t1;
//			
//			t2 = (float)adc_aver[i+4] * 255.0 / 4096.0;
//			send_val[i+4] = (uint8_t)t2;
		}
			
		//����ÿ�����ֵ��+7000����Ϊ�˱�����ָ�ֵ��
//			adc_aver[0]=ADC_Value[0]+ADC_Value[6]+ADC_Value[12]+ADC_Value[18]+ADC_Value[24]+ADC_Value[30]+ADC_Value[36]
//					+ADC_Value[42]+ADC_Value[48]+ADC_Value[54]-sumref7+7000;//Fz
//			adc_aver[1]=ADC_Value[1]+ADC_Value[7]+ADC_Value[13]+ADC_Value[19]+ADC_Value[25]+ADC_Value[31]+ADC_Value[37]
//					+ADC_Value[43]+ADC_Value[49]+ADC_Value[55]-sumref15+7000;//Fx
//			adc_aver[2]=ADC_Value[2]+ADC_Value[8]+ADC_Value[14]+ADC_Value[20]+ADC_Value[26]+ADC_Value[32]+ADC_Value[38]
//					+ADC_Value[44]+ADC_Value[50]+ADC_Value[56]-sumref15+7000;//Fy
//			adc_aver[3]=ADC_Value[3]+ADC_Value[9]+ADC_Value[15]+ADC_Value[21]+ADC_Value[27]+ADC_Value[33]+ADC_Value[39]
//					+ADC_Value[45]+ADC_Value[51]+ADC_Value[57]-sumref7+7000;//T
//					
//			sumref7=ADC_Value[4]+ADC_Value[10]+ADC_Value[16]+ADC_Value[22]+ADC_Value[28]+ADC_Value[34]+ADC_Value[40]
//					+ADC_Value[46]+ADC_Value[52]+ADC_Value[58];//�ο���ѹ0.7��ʮ�����
//			sumref15=ADC_Value[5]+ADC_Value[11]+ADC_Value[17]+ADC_Value[23]+ADC_Value[29]+ADC_Value[35]+ADC_Value[41]
//					+ADC_Value[47]+ADC_Value[53]+ADC_Value[59];//�ο���ѹ1.5��ʮ�����
//					
//			
//			adc_aver[4]=ADC_Value[60]+ADC_Value[66]+ADC_Value[72]+ADC_Value[78]+ADC_Value[84]+ADC_Value[90]+ADC_Value[96]
//					+ADC_Value[102]+ADC_Value[108]+ADC_Value[114]-sumref7+7000;//Fz
//			adc_aver[5]=ADC_Value[61]+ADC_Value[67]+ADC_Value[73]+ADC_Value[79]+ADC_Value[85]+ADC_Value[91]+ADC_Value[97]
//					+ADC_Value[103]+ADC_Value[109]+ADC_Value[115]-sumref15+7000;//Fx
//			adc_aver[6]=ADC_Value[62]+ADC_Value[68]+ADC_Value[74]+ADC_Value[80]+ADC_Value[86]+ADC_Value[92]+ADC_Value[98]
//					+ADC_Value[104]+ADC_Value[110]+ADC_Value[116]-sumref15+7000;//Fy
//			adc_aver[7]=ADC_Value[63]+ADC_Value[69]+ADC_Value[75]+ADC_Value[81]+ADC_Value[87]+ADC_Value[93]+ADC_Value[99]
//					+ADC_Value[105]+ADC_Value[111]+ADC_Value[117]-sumref7+7000;//T
//					
//			sumref7=ADC_Value[64]+ADC_Value[70]+ADC_Value[76]+ADC_Value[82]+ADC_Value[88]+ADC_Value[94]+ADC_Value[100]
//					+ADC_Value[106]+ADC_Value[112]+ADC_Value[118];//�ο���ѹ0.7��ʮ�����
//			sumref15=ADC_Value[65]+ADC_Value[71]+ADC_Value[77]+ADC_Value[83]+ADC_Value[89]+ADC_Value[95]+ADC_Value[101]
//					+ADC_Value[107]+ADC_Value[113]+ADC_Value[119];//�ο���ѹ1.5��ʮ�����

//			adc_aver[8]=ADC_Value[118];//ref0.7�ο���ѹ�ĵ��β���ֵ
//			adc_aver[9]=ADC_Value[119];//ref1.5�ο���ѹ�ĵ��β���ֵ

	
	

	//adc_aver���飺1-4��z/x/y/T��ʮ�β���ֵ�ĺͣ�5-8��z/x/y/T��ʮ�β���ֵ�ĺͣ�9��ref0.7�ĵ��β���ֵ��10��ref1.5�ĵ��β���ֵ
	status_tx=NRF24L01_TxPacket((uint8_t *)send_val);//nrf��Ƶ��������
	if(status_tx != 0x20)
		HAL_NVIC_SystemReset(); 
		
	

//	status_tx=NRF24L01_TxPacket((uint8_t *)tmp_buf);//nrf��Ƶ��������
	//�������״̬  ����״̬�²�ʹ��
//	printf("z-%d,x-%d,y-%d,T-%d  ",adc_aver[0],adc_aver[1],adc_aver[2],adc_aver[3]);

//  if(status_tx!=0x20)
//  {
//     count_send_fail++;
//  }  
//	else {count1++;}     

}
/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
//��ʱ��ȫ���жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

    /* Prevent unused argument(s) compilation warning */
	//ÿ��һ��ʱ��������ͳɹ����� ����״̬�²�ʹ��
//    if(htim->Instance==TIM3)
//    {   
//        count2++;
//	}
//	
//	if(count2==10000)//3s���һ�ζ�������
//    {
//		printf("shibai %d.%d",count_send_fail,count1); //count1�Ƿ��ͳɹ��Ĵ���
//				count_send_fail=0;
//        count2=0;
//	}

    /* NOTE : This function should not be modified, when the callback is needed,
              the HAL_TIM_PeriodElapsedCallback could be implemented in the user file
     */
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
