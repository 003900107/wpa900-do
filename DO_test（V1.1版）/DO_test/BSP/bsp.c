#include "bsp.h"
#include "stdio.h"
#include "I2CRoutines.h"
#include "BaseBus_Protocol.h"
#include <string.h>

#define DO_TMR_INTERVAL 100
__IO  int timingdelay;
__IO uint32_t LocalTime = 0 ;

extern unsigned char RxBuffer[8];

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
I2C_InitTypeDef I2C_InitStruct;
DMA_InitTypeDef  I2CDMA_InitStructure;
uint8_t __IO DoStatusMap;
DoStructure DoStruct[8]; 
/* Private function prototypes -----------------------------------------------*/


/**
* @brief  Configures the different system clocks.
* @param  None
* @retval None
*/
void RCC_Configuration(void)
{
  /* ADCCLK = PCLK2/4 */
  //RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
  
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 and DMA2 clocks */
  //RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_DMA2, ENABLE);
  
  /* Enable ADC1, ADC2, ADC3 and GPIOC clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOD| RCC_APB2Periph_GPIOB, ENABLE);
  
}

/**
* @brief  Configures the different GPIO ports.
* @param  None
* @retval None
*/
void GPIO_Configuration(void)
{
  /* Configure PC.02, PC.03 and PC.04 (ADC Channel12, ADC Channel13 and 
  ADC Channel14) as analog inputs */
  //  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
  //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  //  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|\
    GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin =	GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);					    
}


/**
* @brief  Configures Vector Table base location.
* @param  None
* @retval None
*/
void NVIC_Configuration(void)
{
  /* Configure and enable ADC interrupt */
  
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
  
#if I2C_METHOD==DMA
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;	
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_SetPriority (SysTick_IRQn, 1);
  
}

void I2C_Configuration(void)
{
  uint16_t timeout;
  GPIO_InitTypeDef GPIO_InitStructure; 
  DMA_InitTypeDef  I2CDMA_InitStructure;
  
  /* Enable I2C2 reset state */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
  /* Release I2C2 from reset state */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  
  GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;   //复用开漏输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStruct.I2C_ClockSpeed =100000;
  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  
  I2C_InitStruct.I2C_OwnAddress1 = 0xB0;
  I2C_Init(I2C1,&I2C_InitStruct);
  timeout=0x1ff;
  while(timeout--);
  I2C_DeInit(I2C1);
  I2C_Init(I2C1,&I2C_InitStruct);
  I2C_Cmd(I2C1,ENABLE);
  
  I2C_ITConfig(I2C1, I2C_IT_EVT|I2C_IT_ERR, ENABLE);
#if I2C_METHOD == DMA  /* I2C1 Slave Transmission using DMA */
  {
    /* Enable I2C1 DMA requests */
    I2C1->CR2 |= CR2_DMAEN_Set;
  }
#endif
  
#if I2C_METHOD == INTERRUPT  /* I2C1 Slave Transmission using Interrupt with highest priority in the application */
  {
    /* Enable Buffer IT (TXE and RXNE ITs) */
    I2C1->CR2 |= I2C_IT_BUF;
    
  }
#endif
#if I2C_METHOD ==DMA		
  DMA_DeInit(DMA1_Channel7);                         
  
  I2CDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)I2C1_DR_Address;
  I2CDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)RxBuffer;   
  I2CDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    
  I2CDMA_InitStructure.DMA_BufferSize =8;            
  I2CDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  I2CDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  I2CDMA_InitStructure.DMA_PeripheralDataSize = DMA_MemoryDataSize_Byte;
  I2CDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  I2CDMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  I2CDMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  I2CDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel7, &I2CDMA_InitStructure);
  DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
#endif
}

void IWDG_Configuration(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  /* IWDG counter clock: 32KHz(LSI) / 32 = 1KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_32);
  
  /* Set counter reload value to 349 */
  IWDG_SetReload(349);
  
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
  
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
  
}
//void SysTick_Config(void)
//{
//    /* Disable SysTick Counter */
//    SysTick_CounterCmd(SysTick_Counter_Disable);        //失能计数器 
//  
//    /* Disable the SysTick Interrupt */
//    SysTick_ITConfig(DISABLE);   //关闭中断
//  
//    /* Configure HCLK clock as SysTick clock source */
//    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);   //8分频
//    /* SysTick interrupt each 1000 Hz with HCLK equal to 72MHz */
//    SysTick_SetReload(9000);   //周期1ms
//    /* Enable the SysTick Interrupt */
//    SysTick_ITConfig(ENABLE);  //打开中断
//
// /* Enable the SysTick Counter */
// //   SysTick_CounterCmd(SysTick_Counter_Enable); //使能计数器
//}

//void Delay(u32 nTime)
//{
//  /* Enable the SysTick Counter */
//  SysTick_CounterCmd(SysTick_Counter_Enable);
//  
//  TimingDelay = nTime;
//
//  while(TimingDelay != 0);
//
//  /* Disable SysTick Counter */
//  SysTick_CounterCmd(SysTick_Counter_Disable);
//  /* Clear SysTick Counter */
//  SysTick_CounterCmd(SysTick_Counter_Clear);
//}

void SysTick_Configuration()

{
  
  if (SysTick_Config(SystemCoreClock / 1000))
    
  { 
    
    /* Capture error */ 
    
    while (1);
    
  }
  
}
void Time_Update(void)
{
  LocalTime++;
}

void Delay(volatile uint32_t nCount)

{ 
  timingdelay = LocalTime + nCount;  
  
  /* wait until the desired delay finish */  
  while(timingdelay > LocalTime)
  {     
  }
}

void RelayClose(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{ 
  
  GPIO_SetBits(GPIOx,GPIO_Pin);
  // GPIO_ResetBits(GPIOx,GPIO_Pin);
}

void RelayOpen(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{ 
  // GPIO_SetBits(GPIOx,GPIO_Pin);
  GPIO_ResetBits(GPIOx,GPIO_Pin);
}

void DO_Process(void)
{
  uint8_t i;
  while(DoStatusMap)
  {
    for(i=0;i<8;i++)
    {
      if(DoStruct[i].status)
      {
        if((LocalTime-DoStruct[i].DoTimer)>= DO_TMR_INTERVAL) 
        {
          DoStruct[i].status=0x00;
          DoStruct[i].DoTimer=0;
          GPIO_WriteBit(ALARM_LED,  Bit_SET);	
          switch(i)
          {
          case 0:
            DoStatusMap&=0xfe;
            RelayOpen(DO0);
            break;
          case 1:
            DoStatusMap&=0xfd;
            RelayOpen(DO1);
            break;
          case 2:
            DoStatusMap&=0xfb;
            RelayOpen(DO2);
            break;
          case 3:
            DoStatusMap&=0xf7;
            RelayOpen(DO3);
            break;
          case 4:
            DoStatusMap&=0xef;
            RelayOpen(DO4);
            break;
          case 5:
            DoStatusMap&=0xdf;
            RelayOpen(DO5);
            break;
          case 6:
            DoStatusMap&=0xbf;
            RelayOpen(DO6);
            break;
          case 7:
            DoStatusMap&=0x7f;
            RelayOpen(DO7);
            break;
          default:break;
          }	
        } 
      }
    }
  }
}

void DO_BSPInit(void)
{
  GPIO_WriteBit(PWR_LED,  Bit_RESET);
  GPIO_WriteBit(ALARM_LED,  Bit_SET);
  GPIO_WriteBit(RUNSTAT_LED,  Bit_SET);
  
  memset(DoStruct,0,sizeof(DoStructure)*8);
}
void WDGFeeding(void)
{
  IWDG_ReloadCounter();
}

