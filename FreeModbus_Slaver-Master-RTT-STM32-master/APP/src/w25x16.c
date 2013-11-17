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

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t SPI_Flash_ReadSR(void)   
{  
	uint8_t byte=0;   
  sFLASH_CS_LOW();  //使能器件  
	SPIx_ReadWriteByte(W25X_ReadStatusReg);  //发送读取状态寄存器命令    
	byte=SPIx_ReadWriteByte(0Xff); //读取一个字节  
  sFLASH_CS_HIGH();  //取消片选   
	return byte;   
} 

/*******************************************************************************
* Function Name  : SPI_FLASH_Write_SR
* Description    : 写SPI_FLASH状态寄存器，只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_FLASH_Write_SR(uint8_t sr)   
{   
  sFLASH_CS_LOW();  //使能器件 
	SPIx_ReadWriteByte(W25X_WriteStatusReg); //发送写取状态寄存器命令    
	SPIx_ReadWriteByte(sr);	//写入一个字节  
  sFLASH_CS_HIGH();  //取消片选 	      
}  


/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Enable
* Description    : SPI_FLASH写使能，将WEL置位 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/ 
void SPI_FLASH_Write_Enable(void)   
{
  sFLASH_CS_LOW();  //使能器件  
  SPIx_ReadWriteByte(W25X_WriteEnable); //发送写使能  
  sFLASH_CS_HIGH();  //取消片选    	      
} 

/*******************************************************************************
* Function Name  : SPI_Flash_Wait_Busy
* Description    : 等待空闲           
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_Wait_Busy(void)   
{   
	while ((SPI_Flash_ReadSR() & 0x01) == 0x01);   	// 等待BUSY位清空
}  



/*******************************************************************************
* Function Name  : SPI_FLASH_Write_Disable
* Description    : SPI_FLASH写禁止	，将WEL清零 
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
void SPI_FLASH_Write_Disable(void)   
{  
	sFLASH_CS_LOW();  //使能器件 
  SPIx_ReadWriteByte(W25X_WriteDisable); //发送写禁止指令    
  sFLASH_CS_HIGH();  //取消片选      	      
} 	


/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Sector
* Description    : 擦除一个扇区, Dst_Addr:扇区地址 0~511 for w25x16 ,擦除一个扇区的最少时间:150ms         
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr *= 4096;
  SPI_FLASH_Write_Enable(); //SET WEL 	 
  SPI_Flash_Wait_Busy();   
  sFLASH_CS_LOW();  //使能器件 
  SPIx_ReadWriteByte(W25X_SectorErase); //发送扇区擦除指令 
  SPIx_ReadWriteByte((uint8_t)((Dst_Addr) >> 16));  //发送24bit地址    
  SPIx_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));   
  SPIx_ReadWriteByte((uint8_t)Dst_Addr);  
  sFLASH_CS_HIGH();  //取消片选         
  SPI_Flash_Wait_Busy(); //等待擦除完成
}  

/*******************************************************************************
* Function Name  : SPI_Flash_Write_Page
* Description    : SPI在一页(0~65535)内写入少于256个字节的数据,在指定地址开始写入最大256字节的数据
* Input          : pBuffer:数据存储区,WriteAddr:开始写入的地址(24bit),NumByteToWrite:要写入的字节数(最大65535)
* Output         : 该数不应该超过该页的剩余字节数!!!
* Return         : 
*******************************************************************************/	 
void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
 	uint16_t i;  
    	
	SPI_FLASH_Write_Enable();	//SET WEL 
  sFLASH_CS_LOW();  //使能器件 
  SPIx_ReadWriteByte(W25X_PageProgram); //发送写页命令   
  SPIx_ReadWriteByte((uint8_t)((WriteAddr) >> 16)); //发送24bit地址    
  SPIx_ReadWriteByte((uint8_t)((WriteAddr) >> 8));   
  SPIx_ReadWriteByte((uint8_t)WriteAddr);   
  for(i = 0; i < NumByteToWrite; i ++) SPIx_ReadWriteByte(pBuffer[i]);	//循环写数  
  sFLASH_CS_HIGH();  //取消片选        
	SPI_Flash_Wait_Busy();	//等待写入结束
} 

/*******************************************************************************
* Function Name  : SPI_Flash_Write_NoCheck
* Description    : 无检验写SPI FLASH 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
*                : 具有自动换页功能,在指定地址开始写入指定长度的数据,但是要确保地址不越界!
* Input          : pBuffer:数据存储区,WriteAddr:开始写入的地址(24bit),NumByteToWrite:要写入的字节数(最大65535)
* Output         : CHECK OK
* Return         : 
*******************************************************************************/
void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)   
{ 			 		 
	uint16_t pageremain;	   

	pageremain = 256 - WriteAddr%256;	//单页剩余的字节数		 	    
	if(NumByteToWrite <= pageremain) pageremain = NumByteToWrite;	//不大于256个字节
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer, WriteAddr, pageremain);
		if(NumByteToWrite == pageremain) break;	//写入结束了
	 	else 	//NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;	//减去已经写入了的字节数
			if(NumByteToWrite > 256) pageremain = 256; //一次可以写入256个字节
			else pageremain = NumByteToWrite;	//不够256个字节了
		}
	};	    
} 


 
/*******************************************************************************
* Function Name  : SPI_Flash_Read
* Description    : 读取SPI FLASH ,在指定地址开始读取指定长度的数据
* Input          : pBuffer:数据存储区,ReadAddr:开始读出的地址(24bit),NumByteToWrite:要写入的字节数(最大65535)
* Output         : 该数不应该超过该页的剩余字节数!!!
* Return         : 
*******************************************************************************/	 
void SPI_Flash_Read(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)   
{ 
 	uint16_t i;  
	
  /* 进入临界区 */
									    
  sFLASH_CS_LOW();  //使能器件   
	SPIx_ReadWriteByte(W25X_ReadData);  //发送读取命令   
  SPIx_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); 	//发送24bit地址    
  SPIx_ReadWriteByte((uint8_t)((ReadAddr) >> 8));   
  SPIx_ReadWriteByte((uint8_t)ReadAddr);   
  for(i = 0; i < NumByteToRead; i ++)
	{ 
    pBuffer[i]=SPIx_ReadWriteByte(0XFF); //循环读数  
  }
  sFLASH_CS_HIGH();  //取消片选   
	
  /* 退出临界区 */
	
}  


/*******************************************************************************
* Function Name  : SPI_Flash_Write
* Description    : 写SPI FLASH, 在指定地址开始写入指定长度的数据,该函数带擦除操作! 
* Input          : pBuffer:数据存储区,WriteAddr:开始写入的地址(24bit),NumByteToWrite:要写入的字节数(最大65535)
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

 /* 进入临界区 */

	secpos = WriteAddr/4096;	//扇区地址 0~511 for w25x16
	secoff = WriteAddr%4096;	//在扇区内的偏移
	secremain = 4096-secoff;	//扇区剩余空间大小   

	if(NumByteToWrite <= secremain) secremain = NumByteToWrite;					//不大于4096个字节
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF, secpos*4096, 4096);	//读出整个扇区的内容
		for(i = 0; i < secremain; i ++)		//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;	//需要擦除  	  
		}
		if(i < secremain)	//需要擦除
		{
			SPI_Flash_Erase_Sector(secpos);	//擦除这个扇区
			for(i = 0; i < secremain; i ++)	//复制
			{
				SPI_FLASH_BUF[i + secoff] = pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF, secpos*4096, 4096);	//写入整个扇区  

		}
		else 
			SPI_Flash_Write_NoCheck(pBuffer, WriteAddr, secremain);	//写已经擦除了的,直接写入扇区剩余区间. 				   
		if(NumByteToWrite == secremain) break;	//写入结束了
		else	//写入未结束
		{
			secpos ++;	//扇区地址增1
			secoff = 0;	//偏移位置为0 	 

		   	pBuffer += secremain;	//指针偏移
			WriteAddr += secremain;	//写地址偏移	   
		   	NumByteToWrite -= secremain;	//字节数递减
			if(NumByteToWrite > 4096) secremain = 4096;	//下一个扇区还是写不完
			else secremain = NumByteToWrite;	//下一个扇区可以写完了
		}	 
	};
	
   /* 退出临界区 */	 	
	
}

/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Chip
* Description    : 擦除整个芯片 ,W25X16:25s, W25X32:40s,W25X64:40s,等待时间超长...      
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_Erase_Chip(void)   
{                                             
  SPI_FLASH_Write_Enable();	//SET WEL 
  SPI_Flash_Wait_Busy();   
  sFLASH_CS_LOW();
  SPIx_ReadWriteByte(W25X_ChipErase); //发送片擦除命令  
  sFLASH_CS_HIGH();
	SPI_Flash_Wait_Busy();	//等待芯片擦除结束
}   

/*******************************************************************************
* Function Name  : SPI_Flash_PowerDown
* Description    : 进入掉电模式            
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_PowerDown(void)   
{ 
  sFLASH_CS_LOW();
  SPIx_ReadWriteByte(W25X_PowerDown); //发送掉电命令  
  sFLASH_CS_HIGH();
  rt_thread_delay( 2 );	//delay_us(3);等待TPD  
}   

/*******************************************************************************
* Function Name  : SPI_Flash_WAKEUP
* Description    : 唤醒             
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_WAKEUP(void)   
{  
  sFLASH_CS_LOW(); 
  SPIx_ReadWriteByte(W25X_ReleasePowerDown); //  send W25X_PowerDown command 0xAB    
  sFLASH_CS_HIGH();
   rt_thread_delay( 2 ); //delay_us(3);等待TRES1
}   

/*******************************************************************************
* Function Name  : SPI_Flash_ReadID
* Description    : 读取芯片ID W25X16的ID:0XEF14
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



