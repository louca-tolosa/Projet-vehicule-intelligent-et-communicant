/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   STM32F4xx HAL API Template project 
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "Board_LED.h"                  // ::Board Support:LED
#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "Driver_SPI.h"                 // ::CMSIS Driver:SPI

ADC_HandleTypeDef myADC2Handle; 

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif


#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif



/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(void);


/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Main program
  * @param  None
  * @retval None
  */


void init_PIN_PA1()
{
	GPIO_InitTypeDef ADCpin; 										// Création de structure de config PIN_ANALOG
	ADC_ChannelConfTypeDef Channel_AN1; 							// Création de structure de config ADC

	//Intialisation PA1
	__HAL_RCC_GPIOA_CLK_ENABLE(); 									// activation Horloge GPIOA
	ADCpin.Pin = GPIO_PIN_1; 										// Selection pin PA1
	ADCpin.Mode = GPIO_MODE_ANALOG; 								// Selection mode analogique
	ADCpin.Pull = GPIO_NOPULL; 										// Désactivation des résistance de pull-up et pull-down

	//Paramétrage ADC2
	__HAL_RCC_ADC2_CLK_ENABLE(); 									// activation Horloge ADC2
	myADC2Handle.Instance = ADC2; 									// Selection de ADC2
	myADC2Handle.Init.Resolution = ADC_RESOLUTION_12B; 				// Selection résolution 12 bit
	myADC2Handle.Init.EOCSelection = ADC_EOC_SINGLE_CONV; 			// Selection du mode single pour l'EOC(end of conversion)
	myADC2Handle.Init.DataAlign = ADC_DATAALIGN_RIGHT; 				// Alignement des data à gauche des octets
	myADC2Handle.Init.ClockPrescaler =ADC_CLOCK_SYNC_PCLK_DIV8; 	// Syncronisation des horloges

	//Paramètrage CHANEL1
	Channel_AN1.Channel = ADC_CHANNEL_1; 							// Selection analog channel 1
	Channel_AN1.Rank = 1; 											// Selection du Rang : 1
	Channel_AN1.SamplingTime = ADC_SAMPLETIME_15CYCLES; 			// Selection du temps d'échentillonage à 15

	HAL_GPIO_Init(GPIOA, &ADCpin); 									// Initialisation PA0 avec les paramètre ci-dessus
	HAL_ADC_Init(&myADC2Handle); 									// Initialisation ADC2 avec les paramètre ci-dessus
	HAL_ADC_ConfigChannel(&myADC2Handle, &Channel_AN1); 			// Initialisation Chanel_0 avec les paramètre ci-dessus.
}


short Conversion_AD()
{
	short entree;
	
	HAL_ADC_Stop(&myADC2Handle);
	HAL_ADC_Start(&myADC2Handle);
	while(HAL_ADC_PollForConversion(&myADC2Handle, 5) != HAL_OK); //Check if conversion is completed
	entree = HAL_ADC_GetValue(&myADC2Handle);
	
	return entree;
}


void ADCthread(void const* argument);
osThreadId ID_ADCthread;
osThreadDef(ADCthread, osPriorityNormal, 1, 0);

extern ARM_DRIVER_SPI Driver_SPI1; 

void mySPI_Thread (void const *argument);                             // thread function pour allumer les leds
osThreadId tid_mySPI_Thread;                                          // thread id
osThreadDef (mySPI_Thread, osPriorityNormal, 1, 0);                   // thread object

void mySPI_Thread2 (void const *argument);                             // thread function pour allumer moins fort
osThreadId tid_mySPI_Thread2;                                          // thread id
osThreadDef (mySPI_Thread2, osPriorityNormal, 1, 0);                   // thread object

void mySPI_Thread3 (void const *argument);                             // thread function pour l'éteindre
osThreadId tid_mySPI_Thread3;                                          // thread id
osThreadDef (mySPI_Thread3, osPriorityNormal, 1, 0);                   // thread object


void mySPI_callback(uint32_t event)
{
	switch (event) {

		case ARM_SPI_EVENT_TRANSFER_COMPLETE  :			osSignalSet(tid_mySPI_Thread, 0x01);
																								osSignalSet(tid_mySPI_Thread2, 0x01);
																								osSignalSet(tid_mySPI_Thread3, 0x01);
																								break;
		default : break;
	}
}

void Init_SPI(void){
	Driver_SPI1.Initialize(mySPI_callback);
	Driver_SPI1.PowerControl(ARM_POWER_FULL);
	Driver_SPI1.Control(ARM_SPI_MODE_MASTER | 
											ARM_SPI_CPOL1_CPHA1 | 
											ARM_SPI_MSB_LSB | 
											ARM_SPI_SS_MASTER_UNUSED |
											ARM_SPI_DATA_BITS(8), 1000000);
	Driver_SPI1.Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
}




int main(void)
{
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();
	LED_Initialize();
	Init_SPI();
	
  /* Configure the system clock to 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();

  /* Add your application code here
     */
	

	init_PIN_PA1();

  osKernelInitialize ();

  /* Create thread functions that start executing, 
  Example: osThreadNew(app_main, NULL, NULL); */
	ID_ADCthread = osThreadCreate(osThread(ADCthread), NULL);
	tid_mySPI_Thread = osThreadCreate (osThread(mySPI_Thread), NULL);
	tid_mySPI_Thread2 = osThreadCreate (osThread(mySPI_Thread2), NULL);
	tid_mySPI_Thread3 = osThreadCreate (osThread(mySPI_Thread3), NULL);

  /* Start thread execution */
  osKernelStart();
	osDelay(osWaitForever);
	
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;  
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


void ADCthread(void const* argument)
{
	short sortie;
	while (1)
  {
		sortie = Conversion_AD();
		if (sortie>=0 && sortie<1000)
		{
			osSignalSet(tid_mySPI_Thread, 0x02);
		}
		else if (sortie>1000 && sortie<3000)
		{
			osSignalSet(tid_mySPI_Thread2, 0x02);
		}
		else 
		{
			osSignalSet(tid_mySPI_Thread3, 0x02);
		}
  }
}

void mySPI_Thread (void const *argument) {
	//osEvent evt;
	char tab[24];
	int i, nb_led;
	
	for (i=0;i<4;i++){
		tab[i] = 0;
	}
	// 4 LED blanches
		for (nb_led = 0; nb_led <4;nb_led++){
				tab[4+nb_led*4]=0xff;
				tab[5+nb_led*4]=0xff;
				tab[6+nb_led*4]=0xff;
				tab[7+nb_led*4]=0xff;
			}
		// end
		tab[20] = 0xff; tab[21] = 0xff; tab[22] = 0xff; tab[23] = 0xff;
	
	while (1) {
		osSignalWait(0x02, osWaitForever);
		Driver_SPI1.Send(tab,24); 
    osSignalWait(0x01, osWaitForever);	// sommeil fin emission
		osDelay(1000);
  }
}

void mySPI_Thread2 (void const *argument) {
	//osEvent evt;
	char tab[24];
	int i, nb_led;
	
	for (i=0;i<4;i++){
		tab[i] = 0;
	}
	
	// 4 LED éteint
		for (nb_led = 0; nb_led <4;nb_led++){
			tab[4+nb_led*4]=0xFF;
			tab[5+nb_led*4]=0x55;
			tab[6+nb_led*4]=0x55;
			tab[7+nb_led*4]=0x55;
		}
		
		// end
	tab[20] = 0xff; tab[21] = 0xff; tab[22] = 0xff; tab[23] = 0xff;
			
	while (1) {
		osSignalWait(0x02, osWaitForever);
		Driver_SPI1.Send(tab,24); 
    osSignalWait(0x01, osWaitForever);	// sommeil fin emission
		osDelay(1000);
  }
}

void mySPI_Thread3 (void const *argument) {
	//osEvent evt;
	char tab[24];
	int i, nb_led;
	
	for (i=0;i<4;i++){
		tab[i] = 0;
	}
	// 4 LED éteint
		for (nb_led = 0; nb_led <4;nb_led++){
			tab[4+nb_led*4]=0x0E;
			tab[5+nb_led*4]=0x00;
			tab[6+nb_led*4]=0x00;
			tab[7+nb_led*4]=0x00;
		}
		
//	tab[4] = 0xff; tab[5] = 0x00;	tab[6] = 0x00; tab[7] = 0xff;																			//pour tester
//	tab[8] = 0xE0 | 0xff ; tab[9] = 0x55;	tab[10] = 0x00; tab[11] = 0x55;
//	tab[12] = 0xff; tab[13] = 0x00;	tab[14] = 0xff; tab[15] = 0x00;
//	tab[16] = 0xE0 | 0x00; tab[17] = 0x00;	tab[18] = 0x00; tab[19] = 0x00;

		// end
	tab[20] = 0xff; tab[21] = 0xff; tab[22] = 0xff; tab[23] = 0xff;
			
	while (1) {
		osSignalWait(0x02, osWaitForever);
		Driver_SPI1.Send(tab,24); 
    osSignalWait(0x01, osWaitForever);	// sommeil fin emission
		osDelay(1000);
  }
}

	