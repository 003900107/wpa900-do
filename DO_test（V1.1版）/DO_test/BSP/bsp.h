#ifndef __BSP_H
#define __BSP_H
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#define ALARM_LED  GPIOD, GPIO_Pin_9
#define RUNSTAT_LED  GPIOD, GPIO_Pin_8
#define PWR_LED  GPIOB, GPIO_Pin_15

#define I2C_RESET_LED    GPIOC, GPIO_Pin_6

#define DO0  GPIOD, GPIO_Pin_10
#define DO1  GPIOD, GPIO_Pin_11
#define DO2  GPIOD, GPIO_Pin_12
#define DO3  GPIOD, GPIO_Pin_13
#define DO4  GPIOD, GPIO_Pin_14
#define DO5  GPIOD, GPIO_Pin_15
#define DO6  GPIOD, GPIO_Pin_7
#define DO7  GPIOD, GPIO_Pin_4

typedef struct TagDoStruct
{
  __IO uint8_t status;
  __IO uint32_t DoTimer;
} DoStructure ;

void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void I2C_Configuration(void);
void IWDG_Configuration(void);

void SysTick_Configuration(void);
void Delay(u32 nTime);
void TimingDelay_Decrement(void);

void RelayOpen(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void RelayClose(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void DO_Process(void);
void DO_BSPInit(void);
void WDGFeeding(void);
#endif

