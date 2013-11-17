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
//#include <rtthread.h>
#include <stm32f10x.h>
#include "24cxx.h" 
#include "delay.h"



/*********************************************************************************************************
** Function name:       AT24CXX_Init
** Descriptions:        初始化IIC接口	
** input parameters:  
** output parameters:   
** Returned value:     	   
*********************************************************************************************************/
void AT24CXX_Init(void)
{
	IIC_Init();
}

/*********************************************************************************************************
** Function name:       AT24CXX_ReadOneByte
** Descriptions:        在AT24CXX指定地址读出一个数据	
** input parameters:    ReadAddr:开始读数的地址
** output parameters:   
** Returned value:     	读到的数据   
*********************************************************************************************************/
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;	
  	  																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{	
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	 
	}
	else 
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	}
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();						//产生一个停止条件	    
	return temp;
}

/*********************************************************************************************************
** Function name:       AT24CXX_WriteOneByte
** Descriptions:        在AT24CXX指定地址写入一个数据	
** input parameters:    WriteAddr  :写入数据的目的地址，DataToWrite:要写入的数据 
** output parameters:   
** Returned value:      
*********************************************************************************************************/
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{	
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址		 
	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   				//发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     				//发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();										//产生一个停止条件 
	delay_ms(10);	 
}

/*********************************************************************************************************
** Function name:       AT24CXX_WriteLenByte
** Descriptions:        在AT24CXX里面的指定地址开始写入长度为Len的数据，该函数用于写入16bit或者32bit的数据	
** input parameters:    WriteAddr  :开始写入的地址，DataToWrite:数据数组首地址
** output parameters:   Len        :要写入数据的长度2,4
** Returned value:      
*********************************************************************************************************/
void AT24CXX_WriteLenByte(uint16_t WriteAddr,uint32_t DataToWrite,uint8_t Len)
{  	
	uint8_t t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

/*********************************************************************************************************
** Function name:       AT24CXX_WriteLenByte
** Descriptions:        在AT24CXX里面的指定地址开始读出长度为Len的数据，该函数用于读出16bit或者32bit的数据.	
** input parameters:    ReadAddr   :开始读出的地址，Len        :要读出数据的长度2,4
** output parameters:   数据
** Returned value:      
*********************************************************************************************************/
uint32_t AT24CXX_ReadLenByte(uint16_t ReadAddr,uint8_t Len)
{  	
	uint8_t t;
	uint32_t temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}

/*********************************************************************************************************
** Function name:       AT24CXX_WriteLenByte
** Descriptions:        检查AT24CXX是否正常，这里用了24XX的最后一个地址(255)来存储标志字.	
** input parameters:   	
** output parameters:   
** Returned value:       返回1:检测失败	 返回0:检测成功
** note:				如果用其他24C系列,这个地址要修改
*********************************************************************************************************/
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);	//避免每次开机都写AT24CXX			   
	if(temp==0X55)return 0;		   
	else							//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/*********************************************************************************************************
** Function name:       AT24CXX_Read
** Descriptions:        在AT24CXX里面的指定地址开始读出指定个数的数据	
** input parameters:   	ReadAddr开始读出的地址 对24c02为0~255 ,pBuffer,数据数组首地址,NumToRead:要读出数据的个数
** output parameters:   
** Returned value:       
*********************************************************************************************************/
void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  

/*********************************************************************************************************
** Function name:       AT24CXX_Write
** Descriptions:        在AT24CXX里面的指定地址开始写入指定个数的数据
** input parameters:   	WriteAddr:开始写入的地址 对24c02为0~255，pBuffer:数据数组首地址，NumToWrite:要写入数据的个数
** output parameters:   
** Returned value:       
*********************************************************************************************************/
void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}
 

static rt_err_t rt_i2c_init (rt_device_t dev)
{
	return RT_EOK;
}

static rt_err_t rt_i2c_open(rt_device_t dev, rt_uint16_t oflag)
{
	return RT_EOK;
}

static rt_err_t rt_i2c_close(rt_device_t dev)
{
	return RT_EOK;
}

static rt_size_t rt_i2c_read (rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	rt_uint8_t* ptr;
	ptr = buffer;
    AT24CXX_Read((uint16_t)pos, ptr, size);
	return size;
}

static rt_size_t rt_i2c_write (rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	rt_uint8_t* ptr;

	ptr = (rt_uint8_t*)buffer;
	
	AT24CXX_Write((uint16_t)pos, ptr, size);

	return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
}

static rt_err_t rt_i2c_control (rt_device_t dev, rt_uint8_t cmd, void *args)
{
	return RT_EOK;
}


rt_err_t rt_hw_i2c_register(rt_device_t device, const char* name, rt_uint32_t flag)
{
	RT_ASSERT(device != RT_NULL);

	device->type 		= RT_Device_Class_Char;
	device->rx_indicate = RT_NULL;
	device->tx_complete = RT_NULL;
	device->init 		= rt_i2c_init;
	device->open		= rt_i2c_open;
	device->close		= rt_i2c_close;
	device->read 		= rt_i2c_read;
	device->write 		= rt_i2c_write;
	device->control 	= rt_i2c_control;
	device->user_data	= RT_NULL;

	/* register a character device */
	return rt_device_register(device, name, RT_DEVICE_FLAG_RDWR | flag);
}

struct rt_device i2c_device;

void rt_hw_i2c_init(void)
{
 	AT24CXX_Init();
  	rt_hw_i2c_register(&i2c_device, "i2c",(RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX));

}


//void rt_thread_i2c_entry(void* parameter)
//{
//	rt_uint8_t	temp[]= {7,6,5,4,3,2,1,0};
//	rt_uint8_t readbuf[8]={0};
//	rt_uint8_t i = 0;

//	
//	rt_device_read(&i2c_device,0,&modbus_slave_master,1);

//	while(1)
//	{
//	;
//	rt_thread_delay(50); 
//	}
//	{
//	rt_memset(readbuf,0,sizeof(readbuf));
//		rt_device_write(&i2c_device,0,temp,8);
//		rt_device_read(&i2c_device,0,readbuf,8);

//		for(i = 0; i < 8; i++)
//		{
//			if(temp[i] != readbuf[i])
//			{
//				rt_kprintf("iic error!\r\n");
//				break;
//			}
//		}
//		rt_thread_delay(50); 
//	}
//}


//int i2c_init(void)
//{
//	rt_thread_t thread;
//	 rt_hw_i2c_init();

//    /* create led1 thread */
//    thread = rt_thread_create("i2c",
//                              rt_thread_i2c_entry, RT_NULL,
//                              256,
//                              20, 5);
//    if (thread != RT_NULL)
//        rt_thread_startup(thread);


//	return 0;
//}
