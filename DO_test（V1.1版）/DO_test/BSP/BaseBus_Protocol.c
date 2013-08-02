/*
*     @arg I2C_EVENT_SLAVE_ADDRESS_MATCHED   : EV1
*     @arg I2C_EVENT_SLAVE_BYTE_RECEIVED     : EV2
*     @arg I2C_EVENT_SLAVE_BYTE_TRANSMITTED  : EV3
*     @arg I2C_EVENT_SLAVE_ACK_FAILURE       : EV3-2
*     @arg I2C_EVENT_MASTER_MODE_SELECT      : EV5
*     @arg I2C_EVENT_MASTER_MODE_SELECTED    : EV6
*     @arg I2C_EVENT_MASTER_BYTE_RECEIVED    : EV7
*     @arg I2C_EVENT_MASTER_BYTE_TRANSMITTED : EV8
*     @arg I2C_EVENT_MASTER_MODE_ADDRESS10   : EV9
*     @arg I2C_EVENT_SLAVE_STOP_DETECTED     : EV4

*/


#include "stm32f10x_i2c.h"
#include "bsp.h"
#include "BaseBus_Protocol.h"
#include "I2CRoutines.h"

unsigned char TxBuffer[8];
unsigned char RxBuffer[8];
uint8_t CommingCall=0;
uint8_t SlaveReceptionComplete = 0;
extern uint8_t __IO DoStatusMap;
extern __IO uint32_t LocalTime;
extern DoStructure DoStruct[8];
extern I2C_InitTypeDef I2C_InitStruct;

#define HEADCHECK(a,b)  ((a==0xA5&&b==0x5A)?1:0)
#define BufferSize            8
__IO uint8_t I2C_BUSYHOLDING_FLAG=0x00;//hold住总线信号量

__IO uint16_t BusBusyCounter=0;

//tyh:20130730 总线复位次数记录，用以重启AI板
__IO uint16_t BUSErrorCounter=0;

//tyh:20130730 总线复位最大次数
#define I2C_BUS_ERROR_MAX_COUNT   0x07

void DoCheck(unsigned char DoSeq)
{
  TxBuffer[0]=0xA5;
  TxBuffer[1]=0x5A;
  TxBuffer[2]=DO_CHK;
  TxBuffer[3]=DoSeq;
  TxBuffer[4]=0x00;
  TxBuffer[5]=0x00;
  TxBuffer[6]=0x00;
  TxBuffer[7]=0x00;
  
  CommingCall=0;
}

void Deal_Comming(void)
{
  if(1==SlaveReceptionComplete )	
  {
    if(HEADCHECK(RxBuffer[0],RxBuffer[1]))
    {
      GPIO_WriteBit(ALARM_LED,  Bit_RESET);	
      
      switch(RxBuffer[2])
      {
      case DO_SEL:
        DoCheck(RxBuffer[3]);
        break;
        
      case DO_EXE:
        {
          if((RxBuffer[3]>0)&&(RxBuffer[3]<9))
            DoStruct[RxBuffer[3]-1].DoTimer =  LocalTime;
          
          DoStruct[RxBuffer[3]-1].status =1;
          
          switch(RxBuffer[3])
          {
          case 1:
            DoStatusMap|=0x01;
            RelayClose(DO0);
            break;
          case 2:	
            DoStatusMap|=0x02;
            RelayClose(DO1);
            break;
          case 3:	
            DoStatusMap|=0x04;
            RelayClose(DO2);
            break;
          case 4:
            DoStatusMap|=0x08;
            RelayClose(DO3);
            break;
          case 5:	
            DoStatusMap|=0x10;
            RelayClose(DO4);
            break;
          case 6:
            DoStatusMap|=0x20;
            RelayClose(DO5);
            break;
          case 7:
            DoStatusMap|=0x40;
            RelayClose(DO6);
            break;
          case 8:
            DoStatusMap|=0x80;
            RelayClose(DO7);
            break;
            
          default:
            break;
          }	
        }
        break;
        
      default:
        break;	          
      }
      
    }
    SlaveReceptionComplete = 0;
    
    // I2C_Cmd(I2C1, ENABLE);
  }
}

void I2CHW_Maintain(void)
{
  __IO uint32_t temp = 0;
  __IO uint32_t Timeout = 0;
  
  if (1==I2C_BUSYHOLDING_FLAG)
  {
    return;
  } 
  else
  {
    temp=I2C1->SR2;
    if(temp&0x0002)
    {
      BusBusyCounter++;
    }
    else
    {
      if(BusBusyCounter != 0)
      {
        BusBusyCounter = 0;
      }
    }
  }
  
  //tyh:20130730 i2c在指定次数内没有复位成功, 重启AI板
  if( BUSErrorCounter > I2C_BUS_ERROR_MAX_COUNT )   //当前时间约为 15*7 s
  {
    GPIO_WriteBit(ALARM_LED,  Bit_RESET);
    Timeout=0xffff;
    while(Timeout--); 
    
    NVIC_SystemReset();
  }  
  
  if(BusBusyCounter > 0x001b) //15s
  {
    I2C_Cmd(I2C1,DISABLE);
    I2C_Cmd(I2C1,ENABLE);
    I2CHW_Reset();
    
    //tyh:20130730 总线复位次数加"1"，
    BUSErrorCounter++; 
    if(BUSErrorCounter%2)
        GPIO_WriteBit(I2C_RESET_LED,  Bit_RESET);
    else
        GPIO_WriteBit(I2C_RESET_LED,  Bit_SET);
    
    BusBusyCounter = 0;    
  }
}



/*------------------------------End of File------------------------------------------*/
