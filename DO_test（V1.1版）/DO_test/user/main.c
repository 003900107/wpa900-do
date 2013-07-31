
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "bsp.h"
#include "BaseBus_Protocol.h"



/** @addtogroup STM32F10x_StdPeriph_Examples
* @{
*/

/** @addtogroup ADC_3ADCs_DMA
* @{
*/ 


/* Private functions ---------------------------------------------------------*/

/**
* @brief   Main program
* @param  None
* @retval None
*/
int main(void)
{
  static uint32_t i;
  static uint8_t RunLedTurned;
  
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32f10x_xx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32f10x.c file
  */     
  
  /* System clocks configuration ---------------------------------------------*/
  
  SystemInit();
  RCC_Configuration();
  
  /* NVIC configuration ------------------------------------------------------*/
  NVIC_Configuration();
  
  /* GPIO configuration ------------------------------------------------------*/
  GPIO_Configuration();
  
  /* i2c configuration ------------------------------------------------------*/
  I2C_Configuration();
  
  /* iwdg configuration ------------------------------------------------------*/
#ifdef WATCHDOG
  IWDG_Configuration();
#endif
  /* SysTick configuration ------------------------------------------------------*/
  SysTick_Configuration();
  
  DO_BSPInit();
  
  
  while (1)
  {
    if((i++&0x80000)&&(!RunLedTurned))
    {
      GPIO_WriteBit(RUNSTAT_LED,  Bit_RESET);
      RunLedTurned=0x01;
      
      I2CHW_Maintain();
    }
    else if (!(i++&0x80000)&&RunLedTurned)
    {
      GPIO_WriteBit(RUNSTAT_LED,  Bit_SET);
      RunLedTurned=0x00;
      
      I2CHW_Maintain();
    }

    
    Deal_Comming();
    DO_Process();
#ifdef WATCHDOG
    WDGFeeding();
#endif
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
  //  while (1)
  //  {
  //  }
}
#endif

/**
* @}
*/ 

/**
* @}
*/ 

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
