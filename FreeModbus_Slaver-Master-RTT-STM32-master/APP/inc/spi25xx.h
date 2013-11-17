#ifndef __FLASH_H
#define __FLASH_H			    
#include "stm32f10x.h"
//Mini STM32������
//W25X16 �������� 
//����ԭ��@ALIENTEK
//2010/6/13
//V1.0

#define SPI_W25XX_CS_PORT              GPIOB								/* GPIOB */
#define SPI_W25XX_CS_CLK               RCC_APB2Periph_GPIOB  
#define SPI_W25XX_CS_PIN               GPIO_Pin_14						    /* PB.14 */

#define Set_SPI_W25XX_CS  			{GPIO_SetBits(SPI_W25XX_CS_PORT,SPI_W25XX_CS_PIN);}
#define Clr_SPI_W25XX_CS  			{GPIO_ResetBits(SPI_W25XX_CS_PORT,SPI_W25XX_CS_PIN);} 
					 
////////////////////////////////////////////////////////////////////////////
//W25X16��д
#define FLASH_ID 0XEF14
//ָ���
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
#define W25X_ChipErase			0xC7 	   	/* ȫƬ����*/
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 		/* Read ����ID */
#define W25X_JedecDeviceID		0x9F 

void SPI_Flash_Init(void);
uint16_t  SPI_Flash_ReadID(void);  	    //��ȡFLASH ID
uint8_t	 SPI_Flash_ReadSR(void);        //��ȡ״̬�Ĵ��� 
void SPI_FLASH_Write_SR(uint8_t sr);  	//д״̬�Ĵ���
void SPI_FLASH_Write_Enable(void);  //дʹ�� 
void SPI_FLASH_Write_Disable(void);	//д����
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //��ȡflash
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//д��flash
void SPI_Flash_Erase_Chip(void);    	  //��Ƭ����
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr);//��������
void SPI_Flash_Wait_Busy(void);           //�ȴ�����
void SPI_Flash_PowerDown(void);           //�������ģʽ
void SPI_Flash_WAKEUP(void);			  //����
#endif















