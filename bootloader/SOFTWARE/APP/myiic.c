/****************************************Copyright (c)****************************************************
**                               LDS INTELLIGENCE TECHNOLOGY (SHANGHAI) CO.,LTD.
**
**                                 http://www.LDS-china.com
**
**--------------File Info---------------------------------------------------------------------------------
** File Name:               can.c
** Last modified Date:      2010.02.25
** Last Version:            1.0
** Description:             CAN �Ĵ�������������
**--------------------------------------------------------------------------------------------------------
** Created By:              ��С��
** Created date:            2010.02.25
** Version:                 1.0
** Descriptions:            The original version ��ʼ�汾
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "stm32f10x_gpio.h"
#include "myiic.h"
#include "delay.h"

/*********************************************************************************************************
** Function name:       IIC_Init
** Descriptions:        IIC��ʼ��
** input parameters:    ��
** output parameters:   ��
** Returned value:     
*********************************************************************************************************/
void IIC_Init(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;
	//RCC->APB2ENR|=1<<4;//��ʹ������IO PORTCʱ�� 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
						 	   
	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
	GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);

	Set_IIC_SCL;
	Set_IIC_SDA;
}


/*********************************************************************************************************
** Function name:       IIC_Start
** Descriptions:        ����IIC��ʼ�ź�
** input parameters:    ��
** output parameters:   ��
** Returned value:     
*********************************************************************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	Set_IIC_SDA;	  	  
	Set_IIC_SCL;
	delay_us(4);
 	Clr_IIC_SDA;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	Clr_IIC_SCL;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  


/*********************************************************************************************************
** Function name:       IIC_Stop
** Descriptions:        ����IICֹͣ�ź�
** input parameters:    ��
** output parameters:   ��
** Returned value:     
*********************************************************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	Clr_IIC_SCL;
	Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	Set_IIC_SCL; 
	Set_IIC_SDA;//����I2C���߽����ź�
	delay_us(4);							   	
}

/*********************************************************************************************************
** Function name:       IIC_Wait_Ack
** Descriptions:        �ȴ�Ӧ���źŵ���
** input parameters:    ��
** output parameters:   ��
** Returned value:     	1������Ӧ��ʧ��
				        0������Ӧ��ɹ�
*********************************************************************************************************/
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      					//SDA����Ϊ����  
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
	Clr_IIC_SCL;					//ʱ�����0 	   
	return 0;  
} 

/*********************************************************************************************************
** Function name:       IIC_Ack
** Descriptions:        ����ACKӦ��
** input parameters:    ��
** output parameters:   ��
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
** Descriptions:        ������ACKӦ��	
** input parameters:    ��
** output parameters:   ��
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
** Descriptions:        IIC����һ���ֽ�	
** input parameters:    ��
** output parameters:   
** Returned value:     	���شӻ�����Ӧ��  
						//1����Ӧ��
						//0����Ӧ��	
*********************************************************************************************************/		  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
    Clr_IIC_SCL;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if ((txd&0x80)>>7) 	Set_IIC_SDA
		else Clr_IIC_SDA;
        txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		Set_IIC_SCL;
		delay_us(2); 
		Clr_IIC_SCL;	
		delay_us(2);
    }	 
} 	    
/*********************************************************************************************************
** Function name:       IIC_Read_Byte
** Descriptions:        IIC����һ���ֽ�	
** input parameters:    ��
** output parameters:   
** Returned value:     	��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
*********************************************************************************************************/
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
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

