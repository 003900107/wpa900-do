/**
******************************************************************************
* @file OptimizedI2Cexamples/src/I2CRoutines.c
* @author  MCD Application Team
* @version  V4.0.0
* @date  06/18/2010
* @brief  Contains the I2C1 slave/Master read and write routines.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
*/

/* Includes ------------------------------------------------------------------*/
#include "I2CRoutines.h"
#include "BaseBus_Protocol.h"

/** @addtogroup Optimized I2C examples
* @{
*/


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO uint8_t SlaveReceptionComplete;
extern unsigned char TxBuffer[8];
extern unsigned char RxBuffer[8];
__IO uint8_t Tx_Idx=0;
__IO uint8_t Rx_Idx=0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
* @brief  Reads buffer of bytes  from the slave.
* @param pBuffer: Buffer of bytes to be read from the slave.
* @param NumByteToRead: Number of bytes to be read by the Master.
* @param Mode: Polling or DMA or Interrupt having the highest priority in the application.
* @param SlaveAddress: The address of the slave to be addressed by the Master.
* @retval : None.
*/

#if I2C_METHOD == INTERRUPT
void _I2C1_EV_IRQHandler(void)
{
  
  __IO uint32_t SR1Register =0;
  __IO uint32_t SR2Register =0;
  /* Read the I2C1 SR1 and SR2 status registers */
  SR1Register = I2C1->SR1;
  SR2Register = I2C1->SR2;
  
  /* If I2C1 is slave (MSL flag = 0) */
  if ((SR2Register &0x0001) != 0x0001)
  {
    /* If ADDR = 1: EV1 */
    if ((SR1Register & 0x0002) == 0x0002)
    {
      /* Clear SR1Register and SR2Register variables to prepare for next IT */
      SR1Register = 0;
      SR2Register = 0;
      /* Initialize the transmit/receive counters for next transmission/reception
      using Interrupt  */
      
      Tx_Idx = 0;
      Rx_Idx = 0;
    }
    
    /* If RXNE = 1: EV2 */
    if ((SR1Register & 0x0040) == 0x0040)
    {
      /* Read data from data register */
      RxBuffer[Rx_Idx++] = I2C1->DR;
      SR1Register = 0;
      SR2Register = 0;
      if(8==Rx_Idx) SlaveReceptionComplete = 1;
      
    }
    /* If STOPF =1: EV4 (Slave has detected a STOP condition on the bus */
    if (( SR1Register & 0x0010) == 0x0010)
    {
      I2C1->CR1 |= CR1_PE_Set;
      SR1Register = 0;
      SR2Register = 0;
      
    }
  } /* End slave mode */
  
  
  
  
}
#endif

#if I2C_METHOD == DMA
void _I2C1_EV_IRQHandler(void)
{
  
  __IO uint32_t SR1Register =0;
  __IO uint32_t SR2Register =0;
  
  SR1Register = I2C1->SR1;
  /*注意，在软件读取SR1寄存器后，对SR2寄存器的读操作将清除ADDR位，清除ADDR后ai板则不再控制scl，
  所以要在SR2读取前做配置工作*/
  if ((SR1Register & 0x0002) == 0x0002)
  {	   
    DMA_InitTypeDef  I2CDMA_InitStructure;
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
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_Init(DMA1_Channel7, &I2CDMA_InitStructure);
    DMA_Cmd(DMA1_Channel7, ENABLE);
    SR2Register = I2C1->SR2;
  }
  
  if ( SR1Register & 0x0010)
  {
    I2C1->CR1 |= CR1_PE_Set;
  }
}
#endif

void _I2C1_ER_IRQHandler(void)
{
  __IO uint32_t SR1Register =0;
  
  /* Read the I2C1 status register */
  SR1Register = I2C1->SR1;
  
  /* If AF = 1 */
  if ((SR1Register & 0x0400) == 0x0400)
  {
    I2C1->SR1 &= 0xFBFF;
    SR1Register = 0;
  }
  
  /* If ARLO = 1 */
  if ((SR1Register & 0x0200) == 0x0200)
  {
    I2C1->SR1 &= 0xFBFF;
    SR1Register = 0;
  }
  
  /* If BERR = 1 */
  if ((SR1Register & 0x0100) == 0x0100)
  {
    I2C1->SR1 &= 0xFEFF;
    SR1Register = 0;
  }
  
  /* If OVR = 1 */
  if ((SR1Register & 0x0800) == 0x0800)
  {
    I2C1->SR1 &= 0xF7FF;
    SR1Register = 0;
  }
  
  //添加异常处理 tyh 20130731
  if(SR1Register != 0)
  {
    //复位I2C
    I2CHW_Reset();
  }  
}

void _DMA1_Channel7_IRQHandler(void)
{
  
  if(DMA_GetITStatus(DMA1_IT_TC7))
  {
    DMA_ClearITPendingBit(DMA1_IT_GL7);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    SlaveReceptionComplete = 1;
  }
}


void I2CHW_Reset(void)
{
  __IO uint16_t Timeout;
  uint8_t i;
  GPIO_InitTypeDef GPIO_InitStructure; 
  
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  I2C_DeInit(I2C1);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
  
  //  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_8;
  //  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  //  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  //  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //  
  //  GPIO_SetBits(GPIOB,GPIO_Pin_9);
  //  GPIO_SetBits(GPIOB,GPIO_Pin_8);
  //  Timeout=0x1ff;
  //  while(Timeout--);
  //  
  I2C_Configuration();
  Timeout=0x1ff;
  while(Timeout--);
  
}



/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
