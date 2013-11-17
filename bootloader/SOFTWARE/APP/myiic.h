#ifndef __MYIIC_H
#define __MYIIC_H

#include "stm32f10x.h"

#define IIC_SCL_PORT              GPIOB
#define IIC_SCL_CLK               RCC_APB2Periph_GPIOB  
#define IIC_SCL_PIN               GPIO_Pin_6

#define IIC_SDA_PORT              GPIOB
#define IIC_SDA_CLK               RCC_APB2Periph_GPIOB  
#define IIC_SDA_PIN               GPIO_Pin_7

//extern GPIO_InitTypeDef  GPIO_InitStructure;   	   		   
//IO��������
//#define SDA_IN()  {GPIOB->CRL|=0X80000000;}
//#define SDA_OUT() {GPIOB->CRL|=0X30000000;}

#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF; GPIOB->CRL|=0X80000000;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF; GPIOB->CRL|=0X30000000;}

//IO��������	 
//#define IIC_SCL    PBout(6) //SCL
#define Set_IIC_SCL  {GPIO_SetBits(IIC_SCL_PORT,IIC_SCL_PIN);}
#define Clr_IIC_SCL  {GPIO_ResetBits(IIC_SCL_PORT,IIC_SCL_PIN);} 
//#define IIC_SDA    PBout(7) //SDA
#define Set_IIC_SDA  {GPIO_SetBits(IIC_SDA_PORT,IIC_SDA_PIN);}
#define Clr_IIC_SDA  {GPIO_ResetBits(IIC_SDA_PORT,IIC_SDA_PIN);} 
//#define READ_SDA   PBin(7)  //����SDA 
#define READ_SDA    (GPIO_ReadInputDataBit(IIC_SDA_PORT, IIC_SDA_PIN))


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

unsigned char SW_WriteEE(uint16_t Address,uint8_t Length,uint8_t* buf);      
unsigned char SW_ReadEE(uint16_t Address,uint8_t Length,uint8_t* buf);

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  
#endif
















