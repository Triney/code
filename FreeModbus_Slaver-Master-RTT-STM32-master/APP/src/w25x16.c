/*
 * File      : w25x16.c
 * Change Logs:
 * Date           Author       Notes
 * 2013-10-17     beilijun     the first version
 */
 
#include <rtthread.h>
#include <stm32f10x.h>
//#include "thread_info.h"
#include "w25x16.h"



//static struct rt_thread w25x16_thread;
//static rt_uint8_t w25x16_thread_stack[W25X16_THREAD_STAKE_SIZE];


/*******************************************************************************
* Function Name  : SPIx_ReadWriteByte
* Description    : Sends a byte through the SPI interface and return the byte 
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
uint8_t SPIx_ReadWriteByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sFLASH_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(sFLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(sFLASH_SPI);
}

//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
uint8_t SPI_Flash_ReadSR(void)   
{  
	uint8_t byte=0;   
  sFLASH_CS_LOW();  //ʹ������  
	SPIx_ReadWriteByte(W25X_ReadStatusReg);  //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPIx_ReadWriteByte(0Xff); //��ȡһ���ֽ�  
  sFLASH_CS_HIGH();  //ȡ��Ƭѡ   
	return byte;   
} 

/*******************************************************************************
* Function Name  : SPI_FLASH_Write_SR
* Description    : дSPI_FLASH״̬�Ĵ�����ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_FLASH_Write_SR(uint8_t sr)   
{   
  sFLASH_CS_LOW();  //ʹ������ 
	SPIx_ReadWriteByte(W25X_WriteStatusReg); //����дȡ״̬�Ĵ�������    
	SPIx_ReadWriteByte(sr);	//д��һ���ֽ�  
  sFLASH_CS_HIGH();  //ȡ��Ƭѡ 	      
}  


/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Enable
* Description    : SPI_FLASHдʹ�ܣ���WEL��λ 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/ 
void SPI_FLASH_Write_Enable(void)   
{
  sFLASH_CS_LOW();  //ʹ������  
  SPIx_ReadWriteByte(W25X_WriteEnable); //����дʹ��  
  sFLASH_CS_HIGH();  //ȡ��Ƭѡ    	      
} 

/*******************************************************************************
* Function Name  : SPI_Flash_Wait_Busy
* Description    : �ȴ�����           
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR() & 0x01) == 0x01);   	// �ȴ�BUSYλ���
}  



/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Disable
* Description    : SPI_FLASHд��ֹ	����WEL���� 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_FLASH_Write_Disable(void)   
{  
	sFLASH_CS_LOW();  //ʹ������ 
  SPIx_ReadWriteByte(W25X_WriteDisable); //����д��ָֹ��    
  sFLASH_CS_HIGH();  //ȡ��Ƭѡ      	      
} 	


/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Sector
* Description    : ����һ������, Dst_Addr:������ַ 0~511 for w25x16 ,����һ������������ʱ��:150ms         
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr *= 4096;
  SPI_FLASH_Write_Enable(); //SET WEL 	 
  SPI_Flash_Wait_Busy();   
  sFLASH_CS_LOW();  //ʹ������ 
  SPIx_ReadWriteByte(W25X_SectorErase); //������������ָ�� 
  SPIx_ReadWriteByte((uint8_t)((Dst_Addr) >> 16));  //����24bit��ַ    
  SPIx_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));   
  SPIx_ReadWriteByte((uint8_t)Dst_Addr);  
  sFLASH_CS_HIGH();  //ȡ��Ƭѡ         
  SPI_Flash_Wait_Busy(); //�ȴ��������
}  

/*******************************************************************************
* Function Name  : SPI_Flash_Write_Page
* Description    : SPI��һҳ(0~65535)��д������256���ֽڵ�����,��ָ����ַ��ʼд�����256�ֽڵ�����
* Input          : pBuffer:���ݴ洢��,WriteAddr:��ʼд��ĵ�ַ(24bit),NumByteToWrite:Ҫд����ֽ���(���65535)
* Output         : ������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
* Return         : 
*******************************************************************************/	 
void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i;  
    	
	SPI_FLASH_Write_Enable();	//SET WEL 
  sFLASH_CS_LOW();  //ʹ������ 
  SPIx_ReadWriteByte(W25X_PageProgram); //����дҳ����   
  SPIx_ReadWriteByte((uint8_t)((WriteAddr) >> 16)); //����24bit��ַ    
  SPIx_ReadWriteByte((uint8_t)((WriteAddr) >> 8));   
  SPIx_ReadWriteByte((uint8_t)WriteAddr);   
  for(i = 0; i < NumByteToWrite; i ++) SPIx_ReadWriteByte(pBuffer[i]);	//ѭ��д��  
  sFLASH_CS_HIGH();  //ȡ��Ƭѡ        
	SPI_Flash_Wait_Busy();	//�ȴ�д�����
} 

/*******************************************************************************
* Function Name  : SPI_Flash_Write_NoCheck
* Description    : �޼���дSPI FLASH ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
*                : �����Զ���ҳ����,��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
* Input          : pBuffer:���ݴ洢��,WriteAddr:��ʼд��ĵ�ַ(24bit),NumByteToWrite:Ҫд����ֽ���(���65535)
* Output         : CHECK OK
* Return         : 
*******************************************************************************/
void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   

	pageremain = 256 - WriteAddr%256;	//��ҳʣ����ֽ���		 	    
	if(NumByteToWrite <= pageremain) pageremain = NumByteToWrite;	//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer, WriteAddr, pageremain);
		if(NumByteToWrite == pageremain) break;	//д�������
	 	else 	//NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;	//��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite > 256) pageremain = 256; //һ�ο���д��256���ֽ�
			else pageremain = NumByteToWrite;	//����256���ֽ���
		}
	};	    
} 


 
/*******************************************************************************
* Function Name  : SPI_Flash_Read
* Description    : ��ȡSPI FLASH ,��ָ����ַ��ʼ��ȡָ�����ȵ�����
* Input          : pBuffer:���ݴ洢��,ReadAddr:��ʼ�����ĵ�ַ(24bit),NumByteToWrite:Ҫд����ֽ���(���65535)
* Output         : ������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
* Return         : 
*******************************************************************************/	 
void SPI_Flash_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)   
{ 
 	uint16_t i;  
	
  /* �����ٽ��� */
									    
  sFLASH_CS_LOW();  //ʹ������   
	SPIx_ReadWriteByte(W25X_ReadData);  //���Ͷ�ȡ����   
  SPIx_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); 	//����24bit��ַ    
  SPIx_ReadWriteByte((uint8_t)((ReadAddr) >> 8));   
  SPIx_ReadWriteByte((uint8_t)ReadAddr);   
  for(i = 0; i < NumByteToRead; i ++)
	{ 
    pBuffer[i]=SPIx_ReadWriteByte(0XFF); //ѭ������  
  }
  sFLASH_CS_HIGH();  //ȡ��Ƭѡ   
	
  /* �˳��ٽ��� */
	
}  


/*******************************************************************************
* Function Name  : SPI_Flash_Write
* Description    : дSPI FLASH, ��ָ����ַ��ʼд��ָ�����ȵ�����,�ú�������������! 
* Input          : pBuffer:���ݴ洢��,WriteAddr:��ʼд��ĵ�ַ(24bit),NumByteToWrite:Ҫд����ֽ���(���65535)
* Output         : None
* Return         : 
*******************************************************************************/		   
uint8_t SPI_FLASH_BUF[4096];
void SPI_Flash_Write(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)   
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    

 /* �����ٽ��� */

	secpos = WriteAddr/4096;	//������ַ 0~511 for w25x16
	secoff = WriteAddr%4096;	//�������ڵ�ƫ��
	secremain = 4096-secoff;	//����ʣ��ռ��С   

	if(NumByteToWrite <= secremain) secremain = NumByteToWrite;					//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF, secpos*4096, 4096);	//������������������
		for(i = 0; i < secremain; i ++)		//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;	//��Ҫ����  	  
		}
		if(i < secremain)	//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);	//�����������
			for(i = 0; i < secremain; i ++)	//����
			{
				SPI_FLASH_BUF[i + secoff] = pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF, secpos*4096, 4096);	//д����������  

		}
		else 
			SPI_Flash_Write_NoCheck(pBuffer, WriteAddr, secremain);	//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite == secremain) break;	//д�������
		else	//д��δ����
		{
			secpos ++;	//������ַ��1
			secoff = 0;	//ƫ��λ��Ϊ0 	 

		   	pBuffer += secremain;	//ָ��ƫ��
			WriteAddr += secremain;	//д��ַƫ��	   
		   	NumByteToWrite -= secremain;	//�ֽ����ݼ�
			if(NumByteToWrite > 4096) secremain = 4096;	//��һ����������д����
			else secremain = NumByteToWrite;	//��һ����������д����
		}	 
	};
	
   /* �˳��ٽ��� */	 	
	
}

/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Chip
* Description    : ��������оƬ ,W25X16:25s, W25X32:40s,W25X64:40s,�ȴ�ʱ�䳬��...      
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_Erase_Chip(void)   
{                                             
  SPI_FLASH_Write_Enable();	//SET WEL 
  SPI_Flash_Wait_Busy();   
  sFLASH_CS_LOW();
  SPIx_ReadWriteByte(W25X_ChipErase); //����Ƭ��������  
  sFLASH_CS_HIGH();
	SPI_Flash_Wait_Busy();	//�ȴ�оƬ��������
}   

/*******************************************************************************
* Function Name  : SPI_Flash_PowerDown
* Description    : �������ģʽ            
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_PowerDown(void)   
{ 
  sFLASH_CS_LOW();
  SPIx_ReadWriteByte(W25X_PowerDown); //���͵�������  
  sFLASH_CS_HIGH();
  rt_thread_delay( 2 );	//delay_us(3);�ȴ�TPD  
}   

/*******************************************************************************
* Function Name  : SPI_Flash_WAKEUP
* Description    : ����             
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_WAKEUP(void)   
{  
  sFLASH_CS_LOW(); 
  SPIx_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB    
  sFLASH_CS_HIGH();
   rt_thread_delay( 2 ); //delay_us(3);�ȴ�TRES1
}   

/*******************************************************************************
* Function Name  : SPI_Flash_ReadID
* Description    : ��ȡоƬID W25X16��ID:0XEF14
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint32_t sFLASH_ReadID(void)
{
	uint16_t Temp = 0;	

  /*!< Select the FLASH: Chip Select low */
  sFLASH_CS_LOW();

  SPIx_ReadWriteByte(0x90);
	SPIx_ReadWriteByte(0x00);
	SPIx_ReadWriteByte(0x00);
	SPIx_ReadWriteByte(0x00);
	Temp =SPIx_ReadWriteByte(0xFF) << 8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 
	
  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  return Temp;
}   




static void SPI_RCC_Configuration(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                         RCC_APB2Periph_SPI1, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC,ENABLE);
}

static void SPI_GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

  /*!< sFLASH_SPI_CS_GPIO, sFLASH_SPI_MOSI_GPIO, sFLASH_SPI_MISO_GPIO 
       and sFLASH_SPI_SCK_GPIO Periph clock enable */
  RCC_APB2PeriphClockCmd(sFLASH_CS_GPIO_CLK | sFLASH_SPI_MOSI_GPIO_CLK | sFLASH_SPI_MISO_GPIO_CLK |
                         sFLASH_SPI_SCK_GPIO_CLK, ENABLE);

  /*!< sFLASH_SPI Periph clock enable */
  RCC_APB2PeriphClockCmd(sFLASH_SPI_CLK, ENABLE);
  
  /*!< Configure sFLASH_SPI pins: SCK */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(sFLASH_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH_SPI pins: MOSI */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MOSI_PIN;
  GPIO_Init(sFLASH_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sFLASH_SPI pins: MISO */
  GPIO_InitStructure.GPIO_Pin = sFLASH_SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  
  GPIO_Init(sFLASH_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
  
  /*!< Configure sFLASH_CS_PIN pin: sFLASH Card CS pin */
  GPIO_InitStructure.GPIO_Pin = sFLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(sFLASH_CS_GPIO_PORT, &GPIO_InitStructure);
}
static void SPI_NVIC_Configuration(void)
{

}
void	sFLASH_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
  SPI_RCC_Configuration();

  SPI_GPIO_Configuration();	

  SPI_NVIC_Configuration();
	
	  /*!< Deselect the FLASH: Chip Select high */
  sFLASH_CS_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;


  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sFLASH_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(sFLASH_SPI, ENABLE);
}

int spi_test(void)
{
  uint32_t FlashID = 0;
  uint32_t i = 0x0;
  uint8_t Tx_Buffer[14] = "hello spi test";
  uint8_t  Rx_Buffer[14];

  sFLASH_Init();
	
	/* Get SPI Flash ID */
  FlashID = sFLASH_ReadID();
  SPI_Flash_Erase_Sector(0);
//	SPI_Flash_Write(Tx_Buffer,0 ,sizeof(Tx_Buffer));
//	SPI_Flash_Read(Rx_Buffer,0 ,sizeof(Rx_Buffer));
	FlashID = sFLASH_ReadID();
  /* Check the SPI Flash ID */
//  if (FlashID != sFLASH_MASK_ID)
//  {
//    /* Perform a write in the Flash followed by a read of the written data */
//    /* Erase SPI FLASH Sector to write on */
//    sFLASH_EraseSector(FLASH_SECTOR_TO_ERASE);
//		
//	  sFLASH_WriteBuffer(Tx_Buffer, FLASH_WRITE_ADDRESS, 14);

//		
//		 /* Read data from SPI FLASH memory */
//    sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, 14);

//    /* Write Tx_Buffer data to SPI FLASH memory */
//    sFLASH_WriteBuffer(Tx_Buffer, FLASH_WRITE_ADDRESS, 14);

//    /* Read data from SPI FLASH memory */
////    sFLASH_ReadBuffer(Rx_Buffer, FLASH_READ_ADDRESS, 14);
//		

    /* Check the correctness of erasing operation dada */
    for (i = 0; i < 14; i++)
    {
      if (Rx_Buffer[i] != Tx_Buffer[i])
      {
        return -1;
      }
    }
    /* TransferStatus2 = PASSED, if the specified sector part is erased */
    /* TransferStatus2 = FAILED, if the specified sector part is not well erased */
		return 0;
  }

//	Rx_Buffer[0]=0;



