/****************************************Copyright (c)****************************************************
**                               LDS INTELLIGENCE TECHNOLOGY (SHANGHAI) CO.,LTD.
**
**                                 http://www.LDS-china.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               can.c
** Last modified Date:      2010.02.25
** Last Version:            1.0
** Description:             CAN 寄存器级驱动程序
**--------------------------------------------------------------------------------------------------------
** Created By:              王小乐
** Created date:            2010.02.25
** Version:                 1.0
** Descriptions:            The original version 初始版本
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "stm32f10x_gpio.h"
#include "myiic.h"
#include "delay.h"

/*********************************************************************************************************
** Function name:       IIC_Init
** Descriptions:        IIC初始化
** input parameters:    无
** output parameters:   无
** Returned value:     
*********************************************************************************************************/
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//先使能外设IO PORTC时钟 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
						 	   
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);

	Set_IIC_SCL;
	Set_IIC_SDA;
}


/*********************************************************************************************************
** Function name:       IIC_Start
** Descriptions:        产生IIC起始信号
** input parameters:    无
** output parameters:   无
** Returned value:     
*********************************************************************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	Set_IIC_SDA;	  	  
	Set_IIC_SCL;
	delay_us(4);
 	Clr_IIC_SDA;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	Clr_IIC_SCL;//钳住I2C总线，准备发送或接收数据 
}	  


/*********************************************************************************************************
** Function name:       IIC_Stop
** Descriptions:        产生IIC停止信号
** input parameters:    无
** output parameters:   无
** Returned value:     
*********************************************************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	Clr_IIC_SCL;
	Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	Set_IIC_SCL; 
	Set_IIC_SDA;//发送I2C总线结束信号
	delay_us(4);							   	
}

/*********************************************************************************************************
** Function name:       IIC_Wait_Ack
** Descriptions:        等待应答信号到来
** input parameters:    无
** output parameters:   无
** Returned value:     	1，接收应答失败
				        0，接收应答成功
*********************************************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      					//SDA设置为输入  
	Set_IIC_SDA;delay_us(1);	   
	Set_IIC_SCL;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	Clr_IIC_SCL;					//时钟输出0 	   
	return 0;  
} 

/*********************************************************************************************************
** Function name:       IIC_Ack
** Descriptions:        产生ACK应答
** input parameters:    无
** output parameters:   无
** Returned value:     	
*********************************************************************************************************/
void IIC_Ack(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Clr_IIC_SDA;
	delay_us(2);
	Set_IIC_SCL;
	delay_us(2);
	Clr_IIC_SCL;
}
	
/*********************************************************************************************************
** Function name:       IIC_NAck
** Descriptions:        不产生ACK应答	
** input parameters:    无
** output parameters:   无
** Returned value:     	
*********************************************************************************************************/	    
void IIC_NAck(void)
{
	Clr_IIC_SCL;
	SDA_OUT();
	Set_IIC_SDA;
	delay_us(2);
	Set_IIC_SCL;
	delay_us(2);
	Clr_IIC_SCL;
}					 				     


/*********************************************************************************************************
** Function name:       IIC_Send_Byte
** Descriptions:        IIC发送一个字节	
** input parameters:    无
** output parameters:   
** Returned value:     	返回从机有无应答  
						//1，有应答
						//0，无应答	
*********************************************************************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    Clr_IIC_SCL;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7) 	Set_IIC_SDA
		else Clr_IIC_SDA;
        txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		Set_IIC_SCL;
		delay_us(2); 
		Clr_IIC_SCL;	
		delay_us(2);
    }	 
} 	    
/*********************************************************************************************************
** Function name:       IIC_Read_Byte
** Descriptions:        IIC发送一个字节	
** input parameters:    无
** output parameters:   
** Returned value:     	读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
*********************************************************************************************************/
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        Clr_IIC_SCL; 
        delay_us(2);
		Set_IIC_SCL;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}



unsigned char SW_WriteEE(uint16_t Address,uint8_t Length,uint8_t* buf)       //Only for one byte Address EEPROM
{
//	uint8_t i;
	if(Address<0x2000)
	{
		IIC_Start();
		IIC_Send_Byte(0xa0);
		if(IIC_Wait_Ack()!=0)
		{
				IIC_Stop();
				return 1;
		}
	
		IIC_Send_Byte(Address/256);
		if(IIC_Wait_Ack()!=0)
		{
			IIC_Stop();
			return 1;
		}
		
		IIC_Send_Byte(Address%256);		
		if(IIC_Wait_Ack()!=0)
		{
			IIC_Stop();
			return 1;			
		}
		
		while(Length--)
		{
			IIC_Send_Byte(*(buf++));
			if(IIC_Wait_Ack()==0)
			{
				continue;
			}
			else
				return 1;
		}		
		IIC_Stop();
		delay_ms(10);
		return 0;
	}
	else return 1;
}

unsigned char SW_ReadEE(uint16_t Address,uint8_t Length,uint8_t* buf)
{
  uint8_t i;
	if(Address<0x2000)
	{
		IIC_Start();
		IIC_Send_Byte(0xa0);
		if(IIC_Wait_Ack()!=0)
		{
			IIC_Stop();
			return 1;				
		}
		
		IIC_Send_Byte(Address/256);
		if(IIC_Wait_Ack()!=0)
		{
				IIC_Stop();
			return 1;		
		}
		IIC_Send_Byte(Address%256);
		if(IIC_Wait_Ack()!=0)
		{
			IIC_Stop();
			return 1;		
		}
				
		IIC_Start();
		IIC_Send_Byte(0xa1);
		if(IIC_Wait_Ack()!=0)
		{
			IIC_Stop();
			return 1;						
		}
			
		if(Length!=1)
		{
			for(i=0;i<Length-1;i++)
			{	
				*(buf+i)=IIC_Read_Byte(1);
			}
			*(buf+i)=IIC_Read_Byte(0);
		}
		else
			*(buf+Length-1)=IIC_Read_Byte(0);
		return 0;
	}
	else return 1;
}

