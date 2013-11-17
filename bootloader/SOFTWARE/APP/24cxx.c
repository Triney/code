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
//#include <rtthread.h>
#include <stm32f10x.h>
#include "24cxx.h" 
#include "delay.h"



/*********************************************************************************************************
** Function name:       AT24CXX_Init
** Descriptions:        ��ʼ��IIC�ӿ�	
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
** Descriptions:        ��AT24CXXָ����ַ����һ������	
** input parameters:    ReadAddr:��ʼ�����ĵ�ַ
** output parameters:   
** Returned value:     	����������   
*********************************************************************************************************/
uint8_t AT24CXX_ReadOneByte(uint16_t ReadAddr)
{				  
	uint8_t temp=0;	
  	  																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{	
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	 
	}
	else 
	{
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	}
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();						//����һ��ֹͣ����	    
	return temp;
}

/*********************************************************************************************************
** Function name:       AT24CXX_WriteOneByte
** Descriptions:        ��AT24CXXָ����ַд��һ������	
** input parameters:    WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ��DataToWrite:Ҫд������� 
** output parameters:   
** Returned value:      
*********************************************************************************************************/
void AT24CXX_WriteOneByte(uint16_t WriteAddr,uint8_t DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{	
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ		 
	}else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   				//���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     				//�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();										//����һ��ֹͣ���� 
	delay_ms(10);	 
}

/*********************************************************************************************************
** Function name:       AT24CXX_WriteLenByte
** Descriptions:        ��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen�����ݣ��ú�������д��16bit����32bit������	
** input parameters:    WriteAddr  :��ʼд��ĵ�ַ��DataToWrite:���������׵�ַ
** output parameters:   Len        :Ҫд�����ݵĳ���2,4
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
** Descriptions:        ��AT24CXX�����ָ����ַ��ʼ��������ΪLen�����ݣ��ú������ڶ���16bit����32bit������.	
** input parameters:    ReadAddr   :��ʼ�����ĵ�ַ��Len        :Ҫ�������ݵĳ���2,4
** output parameters:   ����
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
** Descriptions:        ���AT24CXX�Ƿ���������������24XX�����һ����ַ(255)���洢��־��.	
** input parameters:   	
** output parameters:   
** Returned value:       ����1:���ʧ��	 ����0:���ɹ�
** note:				���������24Cϵ��,�����ַҪ�޸�
*********************************************************************************************************/
uint8_t AT24CXX_Check(void)
{
	uint8_t temp;
	temp=AT24CXX_ReadOneByte(255);	//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else							//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(255,0X55);
	    temp=AT24CXX_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

/*********************************************************************************************************
** Function name:       AT24CXX_Read
** Descriptions:        ��AT24CXX�����ָ����ַ��ʼ����ָ������������	
** input parameters:   	ReadAddr��ʼ�����ĵ�ַ ��24c02Ϊ0~255 ,pBuffer,���������׵�ַ,NumToRead:Ҫ�������ݵĸ���
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
** Descriptions:        ��AT24CXX�����ָ����ַ��ʼд��ָ������������
** input parameters:   	WriteAddr:��ʼд��ĵ�ַ ��24c02Ϊ0~255��pBuffer:���������׵�ַ��NumToWrite:Ҫд�����ݵĸ���
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
