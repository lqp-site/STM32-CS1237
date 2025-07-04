/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "iwdg.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "delay.h"
#include "cs1237.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRIGGER_THRESHOLD   1000   //压力差阈值
#define VOLTAGE_THRESHOLD   1.25   //电压差阈值 Voltage
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t flag_sign = 0; //压力值符号，代表正反压力值
uint16_t emaWeight = 0; //滤波后的重量
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
    delay_init(72);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
    CS1237_init();
    delay_ms(500);

#if USE_PRINTF_MSG    
    printf("Init finshed --------------------\n\r");
#endif
    
    uint8_t speed_sel = SpeedSelct_640HZ;
    uint8_t pga_sel = PGA_64;
    CS1237_WriteConfig(RefOut_ON | speed_sel | pga_sel | CH_A);
    delay_ms(300);
    IWDG_Feed(); //喂狗

#if USE_PRINTF_MSG    
    //读取并输出配置
    uint8_t config = CS1237_ReadConfig();
    CS1237_ReadConfigOutput(config);
    delay_ms(200);
#endif
    
    int d_value;
    uint32_t data_zero = 0,cs1237_data = 0;
    uint32_t max_data = 0,min_data = 0,sum_data = 0; 
    uint8_t value_count = 0;
    uint8_t trigger_count = 0;
    data_zero = CS1237_ReadData(); //获取首次数据
    max_data = data_zero;
    min_data = data_zero;
    IWDG_Feed(); //喂狗
    
#if CODE_TEST //测试使用
    delay_ms(100);
    data_zero = CS1237_ReadData(); 
    delay_ms(200);
    uint16_t k = 1;
    uint16_t w;
    while(k < 200)
    {
        cs1237_data = CS1237_ReadData(); 
//        w = (cs1237_data - data_zero) / k;
        w = cs1237_data > data_zero ? cs1237_data - data_zero : data_zero - cs1237_data;
        printf("k = %d ,data = %u, weight = %u \n\r",flag_sign,cs1237_data, w);
        delay_ms(500);
        k += 1;
        data_zero = cs1237_data;
    }
#endif

    
    
    
    
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        
        cs1237_data = CS1237_ReadData();
        d_value = data_zero > cs1237_data ? data_zero - cs1237_data : cs1237_data - data_zero;
        if(cs1237_data != 0)
        {
            if(d_value > TRIGGER_THRESHOLD)
            {
                trigger_count += 1;
                if(trigger_count == 2)
                {
                    ENDSTOP_DATA_OUT(1);
                    LED_OUT(1);
                    delay_ms(450);
#if USE_PRINTF_MSG
                printf("Difference = %d \n\r", d_value);
#endif
                    LED_OUT(0);
                    ENDSTOP_DATA_OUT(0);
                    delay_ms(100);
                }
                value_count = 0;
                sum_data = 0;
                max_data = data_zero;
                min_data = data_zero;
            }
            else{ 
                value_count += 1;
                sum_data += cs1237_data;
                if(max_data < cs1237_data) max_data = cs1237_data;
                else if(min_data > cs1237_data) min_data = cs1237_data;    
                if(value_count > 16){
                    value_count = 0;
                    data_zero = (sum_data - max_data - min_data)  / 14;
                    sum_data = 0;
                    max_data = data_zero;
                    min_data = data_zero;
                }
                trigger_count = 0;
                IWDG_Feed(); //喂狗
            }
            
        }
    
        switch(speed_sel)
        {
            case SpeedSelct_10HZ: delay_ms(100);break;
            case SpeedSelct_40HZ: delay_ms(30);break;
            case SpeedSelct_640HZ: delay_ms(6);break;
            case SpeedSelct_1280HZ: delay_ms(5);break;
        }
        
#if CODE_TEST
        printf("Difference = %d \n\r", d_value);
        delay_ms(500);
        ENDSTOP_DATA_OUT(1);
        LED_OUT(1);
        delay_ms(500);
        ENDSTOP_DATA_OUT(0);
        LED_OUT(0);
#endif
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  __disable_irq();
  while (1)
  {
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
