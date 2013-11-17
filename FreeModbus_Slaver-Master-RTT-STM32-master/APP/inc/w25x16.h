/*
 * File      : w25x16.h
 * Change Logs:
 * Date           Author       Notes
 * 2013-10-17     beilijun     the first version
 */

#ifndef __W25X16_H__
#define __W25X16_H__

#include "stm32f10x.h"
#include <rtthread.h>

/****************************Flash空间分布表***********************
**
**
** 0x1000
** 0x2000
** 0x3000		遥控器命令代码学习数量
** 0x4000  		储存遥控器匹配命令数量
** 0x5000		储存遥控器匹配命令
** 0x6000		
** 0x7000
** 0x8000
** 0x9000
** 0xA000
** 0xB000
** 0xC000
** 0xD000
** 0xE000

LINK****************已经学习遥控器面板按键链表储存空间**********
** 0x010000		储存遥控器学习链表起始空间 16K	20*64*12= 15360
** 0x013FFF		储存遥控器学习链表结束空间 16K	
后面 4个区的查找根据这个表进行索引查找 
学习总表如果此处删除后面全部删除
1、PDES的描述可以不根据这个表，可以直接进行空间读取
2、CMD的数据的读取根据LINK表，然后转换为相应的地址，然后读取出来储存到RAM中
3、CODE的数据的读取可以不根据这个表，可以直接进行空间读取
4、DES的数据读取可以不根据这个表，可以直接进行读取。

PDES******************遥控器面板描述符储存空间******************
** 0x01A000		储存遥控器描述储存起始空间 4K 	20*128= 2560
** 0x01AFFF		储存遥控器描述储存结束空间 
				20个面板一个面板128个字的描述

chanel****************遥控器面板按键通道储存空间*************
** 0x01B000		储存遥控器匹配命令储存起始空间 4K 	20*64*1= 1280
** 0x01BFFF		储存遥控器匹配命令储存结束空间 4K 	

CMD******************遥控器面板按键匹配命令储存空间*************
** 0x020000		储存遥控器匹配命令储存起始空间 64K 	20*64*8= 10240
** 0x01FFFF		储存遥控器匹配命令储存结束空间 64K 	20*64*8= 10240

CODE******************遥控器面板按键代码储存空间****************
** 0X030000		遥控器代码储存开始地址 20块，每块可以放置64个遥控器码 
**				每个遥控器代码占用512个字节  	1扇区64K，128个遥控器按键
** 0X1CFFFF		遥控器代码储存结束地址  		共计占用：20*64*512= 10*64K = 10个扇区块

DES******************遥控器面板按键描述符储存空间***************
** 0X1D0000		遥控器代码描述储存开始地址
**				每个遥控器描述代码占用128个字节	
** 0X1FFFFF		遥控器代码描述储存结束地址  	共计占用：20*64*128= 3*64K = 3个扇区块

***************************************************************/

#define REMOTER_PANDEL_SIZE_ADD		0X1000 			/*	储存遥控器命令代码学习数量		*/

#define REMOTER_BOX_ADD				0X2000 			/*	储存遥控器命令代码学习数量		*/

#define REMOTER_LINK_ADD			0X3000 			/*	储存遥控器命令代码学习数量
													**	每个面板学习按键数量			1
													**	面板号、按键号、通道、学习状态	4*64
													**	命令匹配数据					8
													*/

#define REMOTER_PDES_START_ADD		0X01A000	  	/*	储存遥控器按键命令匹配起始地址	*/

#define REMOTER_CTLCMD_START_ADD	0X020000		/*	储存遥控器命令代码起始地址		*/

#define REMOTER_CODE_START_ADD		0X030000	  	/*	储存遥控器代码起始地址			*/

#define REMOTER_DES_START_ADD		0X1D0000	  	/*	储存遥控器描述起始地址			*/


/*******************************************************************************
* Function Name  : rmt_f_pdes
* Description    : 根据遥控器面板号，查找出该面板号描述符储存到flash中的首地址             
* Input          : pandel遥控器面板，addr在flash中的位置
* Output         : addr
* Return         : 一个pandel占用的地址空间为128个字节
*				 ：	
*******************************************************************************/
__inline void rmt_f_pdes(uint32_t pandel,uint32_t *addr)			
{												
	pandel = pandel << 7;						
	*addr = pandel + REMOTER_PDES_START_ADD;		
}
	
/*******************************************************************************
* Function Name  : rmt_f_ctlcmd
* Description    : 根据遥控器面板号，查找出该按键命令储存到flash中的首地址             
* Input          : no遥控器面板+按键号，addr在flash中的位置
* Output         : addr
* Return         : 一个no占用的地址空间为8个字节
*				 ：	
*******************************************************************************/
__inline void rmt_f_ctlcmd(uint32_t no, uint32_t *addr)
{												
	no = no << 3;								
	*addr = no + REMOTER_CTLCMD_START_ADD;		
}

/*******************************************************************************
* Function Name  : rmt_f_des
* Description    : 根据遥控器面板号，查找出该面板按键号描述符储存到flash中的首地址             
* Input          : no遥控器面板+按键号，addr在flash中的位置
* Output         : addr
* Return         : 一个no占用的地址空间为128个字节
*				 ：	
*******************************************************************************/
__inline void rmt_f_des(uint32_t no, uint32_t *addr)
{											
	no = no << 7;							
	*addr = no + REMOTER_DES_START_ADD;		
}



/*******************************************************************************
* Function Name  : rmt_f_code
* Description    : 根据遥控器面板号，查找出该面板号描述符储存到flash中的首地址             
* Input          : no遥控器面板+按键号，addr在flash中的位置
* Output         : addr
* Return         : 一个no占用的地址空间为128个字节
*				 ：	
*******************************************************************************/
__inline void rmt_f_code(uint32_t no, uint32_t *addr)			   
{											
	no = no << 9;							
	*addr = no + REMOTER_CODE_START_ADD;		
}


/*******************************************************************************
* Function Name  : RMT_ALIGN(size, align)
* Description    : 根据储存到flash中的地址,查找出遥控器面板号，该面板按键号描述符             
* Input          : no遥控器面板+按键号，addr在flash中的位置
* Output         : 返回最大的向上对齐关系
* Return         : 如RT_ALIGN(13, 4)，返回size 16
*				 ：	
*******************************************************************************/
#define RMT_ALIGN(size, align)			(((size) + (align) - 1) & ~((align)-1))

/*******************************************************************************
* Function Name  : RMT_ALIGN_DOWN(size, align)
* Description    : 根据储存到flash中的地址,查找出遥控器面板号，该面板按键号描述符             
* Input          : no遥控器面板+按键号，addr在flash中的位置
* Output         : 返回最大的向下对齐关系
* Return         : 如RT_ALIGN(13, 4)，返回size 12
*				 ：	
*******************************************************************************/
#define RMT_ALIGN_DOWN(size, align)		((size) & ~((align) -1))

/*******************************************************************************
* Function Name  : arr_f_des
* Description    : 根据储存到flash中的地址,查找出遥控器面板号，该面板按键号描述符起始地址             
* Input          : addr在flash中的位置
* Output         : addr
* Return         : 一个no占用的地址空间为128个字节
*				 ：	
*******************************************************************************/
__inline void arr_f_des(uint32_t *addr)					
{											
	*addr -=  REMOTER_DES_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,128);				
}

/*******************************************************************************
* Function Name  : arr_f_pdes
* Description    : 根据储存到flash中的地址,查找出遥控器面板号，该面板按键号描述符起始地址             
* Input          : addr在flash中的位置
* Output         : addr
* Return         : 一个no占用的地址空间为128个字节
*				 ：	
*******************************************************************************/
__inline void arr_f_pdes(uint32_t *addr)					
{											
	*addr -=  REMOTER_PDES_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,128);				
}

/*******************************************************************************
* Function Name  : arr_f_code
* Description    : 根据储存到flash中的地址,查找出遥控器面板号，该面板按键号遥控器代码起始地址             
* Input          : addr在flash中的位置
* Output         : addr
* Return         : 一个no占用的地址空间为512个字节
*				 ：	
*******************************************************************************/
__inline void arr_f_code(uint32_t *addr)				
{											
	*addr -=  REMOTER_CODE_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,512);			
}

/*******************************************************************************
* Function Name  : arr_f_ctlcmd
* Description    : 根据储存到flash中的地址,查找出遥控器面板号，该面板按键号遥控器代码起始地址             
* Input          : addr在flash中的位置
* Output         : addr
* Return         : 一个no占用的地址空间为512个字节
*				 ：	
*******************************************************************************/
__inline void arr_f_ctlcmd(uint32_t *addr)					
{											
	*addr -=  REMOTER_CTLCMD_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,8);					
}

					 
////////////////////////////////////////////////////////////////////////////

#define FLASH_ID 0XEF14
//指令表
#define W25X_WriteEnable		0x06   		/* Write enable instruction */
#define W25X_WriteDisable		0x04 		/* Write disable instruction */
#define W25X_ReadStatusReg		0x05 		/* Read Status Register instruction  */
#define W25X_WriteStatusReg		0x01 		/* Write Status Register instruction */ 
#define W25X_ReadData			0x03 		/*Read from Memory instruction */
#define W25X_FastReadData		0x0B 		/* FAST Read from Memory instruction */
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 		/* Write to Memory instruction */
#define W25X_BlockErase			0xD8 	   	/* Bulk Erase instruction */
#define W25X_SectorErase		0x20 		/* Sector Erase instruction */
#define W25X_ChipErase			0xC7 	   	/* 全片擦除*/
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 		/* Read 工厂ID */
#define W25X_JedecDeviceID		0x9F 


#define sFLASH_SPI                       SPI1
#define sFLASH_SPI_CLK                   RCC_APB2Periph_SPI1
#define sFLASH_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define sFLASH_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define sFLASH_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.06 */
#define sFLASH_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define sFLASH_SPI_MISO_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sFLASH_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.07 */
#define sFLASH_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define sFLASH_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOA
#define sFLASH_CS_PIN                    GPIO_Pin_14                  /* PA.14 */
#define sFLASH_CS_GPIO_PORT              GPIOB                       /* GPIOB */
#define sFLASH_CS_GPIO_CLK               RCC_APB2Periph_GPIOB

/* w25x16 FLASH SPI Interface pins  */ 
/*
#define sFLASH_SPI                           SPI2
#define sFLASH_SPI_CLK                       RCC_APB2Periph_SPI2
#define sFLASH_SPI_CLK_INIT                  RCC_APB2PeriphClockCmd

#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_5
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOA
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOA
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource13
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI2

#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_14
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_APB1Periph_GPIOB
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource14
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI2

#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_15
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_APB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource15
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI2

#define sFLASH_CS_PIN                        GPIO_Pin_3
#define sFLASH_CS_GPIO_PORT                  GPIOA
#define sFLASH_CS_GPIO_CLK                   RCC_APB1Periph_GPIOA
*/
/* Exported macro ------------------------------------------------------------*/
/* Select sFLASH: Chip Select pin low */
#define sFLASH_CS_LOW()       GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
/* Deselect sFLASH: Chip Select pin high */
#define sFLASH_CS_HIGH()      GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)   


uint8_t SPIx_ReadWriteByte(uint8_t byte);
uint32_t sFLASH_ReadID(void);

int rt_w25x16_init(void);
uint8_t	 SPI_Flash_ReadSR(void);        //读取状态寄存器 
void SPI_FLASH_Write_SR(uint8_t sr);  	//写状态寄存器
void SPI_FLASH_Write_Enable(void);  //写使能 
void SPI_FLASH_Write_Disable(void);	//写保护
void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite); //无检验写SPI FLASH
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //读取flash
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入flash
void SPI_Flash_Erase_Chip(void);    	  //整片擦除
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr);//扇区擦除
void SPI_Flash_Wait_Busy(void);           //等待空闲
void SPI_Flash_PowerDown(void);           //进入掉电模式
void SPI_Flash_WAKEUP(void);			  //唤醒




#endif
