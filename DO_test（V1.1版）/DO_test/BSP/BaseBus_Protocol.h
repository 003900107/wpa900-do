#ifndef BASEBUSPROTCOL
#define BASEBUSPROTCOL

#define DO_EXE 0xB3
#define DO_CHK 0xB2
#define DO_SEL 0xB1

#define CR1_PE_Set              ((uint16_t)0x0001)
#define CR1_PE_Reset            ((uint16_t)0xFFFE)

#define DMA	1
#define POLLING	2
#define INTERRUPT 3
#define I2C_METHOD DMA

void _I2C1_EV_IRQHandler(void);
void DoCheck(unsigned char DoSeq);
void DoExecute(unsigned char DoSeq);
void DoSelect(unsigned char DoSeq);
void Deal_Comming(void);

void I2CHW_Maintain(void);

#endif
