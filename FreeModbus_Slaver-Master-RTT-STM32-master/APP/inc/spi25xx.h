#ifndef __FLASH_H
#define __FLASH_H			    
#include "stm32f10x.h"
//Mini STM32开发板
//W25X16 驱动函数 
//正点原子@ALIENTEK
//2010/6/13
//V1.0

#define SPI_W25XX_CS_PORT              GPIOB								/* GPIOB */
#define SPI_W25XX_CS_CLK               RCC_APB2Periph_GPIOB  
#define SPI_W25XX_CS_PIN               GPIO_Pin_14						    /* PB.14 */

#define Set_SPI_W25XX_CS  			{GPIO_SetBits(SPI_W25XX_CS_PORT,SPI_W25XX_CS_PIN);}
#define Clr_SPI_W25XX_CS  			{GPIO_ResetBits(SPI_W25XX_CS_PORT,SPI_W25XX_CS_PIN);} 
					 
////////////////////////////////////////////////////////////////////////////
//W25X16读写
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

void SPI_Flash_Init(void);
uint16_t  SPI_Flash_ReadID(void);  	    //读取FLASH ID
uint8_t	 SPI_Flash_ReadSR(void);        //读取状态寄存器 
void SPI_FLASH_Write_SR(uint8_t sr);  	//写状态寄存器
void SPI_FLASH_Write_Enable(void);  //写使能 
void SPI_FLASH_Write_Disable(void);	//写保护
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //读取flash
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//写入flash
void SPI_Flash_Erase_Chip(void);    	  //整片擦除
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr);//扇区擦除
void SPI_Flash_Wait_Busy(void);           //等待空闲
void SPI_Flash_PowerDown(void);           //进入掉电模式
void SPI_Flash_WAKEUP(void);			  //唤醒
#endif















