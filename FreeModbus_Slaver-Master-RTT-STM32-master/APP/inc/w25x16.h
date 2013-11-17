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

/****************************Flash�ռ�ֲ���***********************
**
**
** 0x1000
** 0x2000
** 0x3000		ң�����������ѧϰ����
** 0x4000  		����ң����ƥ����������
** 0x5000		����ң����ƥ������
** 0x6000		
** 0x7000
** 0x8000
** 0x9000
** 0xA000
** 0xB000
** 0xC000
** 0xD000
** 0xE000

LINK****************�Ѿ�ѧϰң������尴��������ռ�**********
** 0x010000		����ң����ѧϰ������ʼ�ռ� 16K	20*64*12= 15360
** 0x013FFF		����ң����ѧϰ��������ռ� 16K	
���� 4�����Ĳ��Ҹ������������������� 
ѧϰ�ܱ�����˴�ɾ������ȫ��ɾ��
1��PDES���������Բ��������������ֱ�ӽ��пռ��ȡ
2��CMD�����ݵĶ�ȡ����LINK��Ȼ��ת��Ϊ��Ӧ�ĵ�ַ��Ȼ���ȡ�������浽RAM��
3��CODE�����ݵĶ�ȡ���Բ��������������ֱ�ӽ��пռ��ȡ
4��DES�����ݶ�ȡ���Բ��������������ֱ�ӽ��ж�ȡ��

PDES******************ң�����������������ռ�******************
** 0x01A000		����ң��������������ʼ�ռ� 4K 	20*128= 2560
** 0x01AFFF		����ң����������������ռ� 
				20�����һ�����128���ֵ�����

chanel****************ң������尴��ͨ������ռ�*************
** 0x01B000		����ң����ƥ���������ʼ�ռ� 4K 	20*64*1= 1280
** 0x01BFFF		����ң����ƥ�����������ռ� 4K 	

CMD******************ң������尴��ƥ�������ռ�*************
** 0x020000		����ң����ƥ���������ʼ�ռ� 64K 	20*64*8= 10240
** 0x01FFFF		����ң����ƥ�����������ռ� 64K 	20*64*8= 10240

CODE******************ң������尴�����봢��ռ�****************
** 0X030000		ң�������봢�濪ʼ��ַ 20�飬ÿ����Է���64��ң������ 
**				ÿ��ң��������ռ��512���ֽ�  	1����64K��128��ң��������
** 0X1CFFFF		ң�������봢�������ַ  		����ռ�ã�20*64*512= 10*64K = 10��������

DES******************ң������尴������������ռ�***************
** 0X1D0000		ң���������������濪ʼ��ַ
**				ÿ��ң������������ռ��128���ֽ�	
** 0X1FFFFF		ң���������������������ַ  	����ռ�ã�20*64*128= 3*64K = 3��������

***************************************************************/

#define REMOTER_PANDEL_SIZE_ADD		0X1000 			/*	����ң�����������ѧϰ����		*/

#define REMOTER_BOX_ADD				0X2000 			/*	����ң�����������ѧϰ����		*/

#define REMOTER_LINK_ADD			0X3000 			/*	����ң�����������ѧϰ����
													**	ÿ�����ѧϰ��������			1
													**	���š������š�ͨ����ѧϰ״̬	4*64
													**	����ƥ������					8
													*/

#define REMOTER_PDES_START_ADD		0X01A000	  	/*	����ң������������ƥ����ʼ��ַ	*/

#define REMOTER_CTLCMD_START_ADD	0X020000		/*	����ң�������������ʼ��ַ		*/

#define REMOTER_CODE_START_ADD		0X030000	  	/*	����ң����������ʼ��ַ			*/

#define REMOTER_DES_START_ADD		0X1D0000	  	/*	����ң����������ʼ��ַ			*/


/*******************************************************************************
* Function Name  : rmt_f_pdes
* Description    : ����ң�������ţ����ҳ����������������浽flash�е��׵�ַ             
* Input          : pandelң������壬addr��flash�е�λ��
* Output         : addr
* Return         : һ��pandelռ�õĵ�ַ�ռ�Ϊ128���ֽ�
*				 ��	
*******************************************************************************/
__inline void rmt_f_pdes(uint32_t pandel,uint32_t *addr)			
{												
	pandel = pandel << 7;						
	*addr = pandel + REMOTER_PDES_START_ADD;		
}
	
/*******************************************************************************
* Function Name  : rmt_f_ctlcmd
* Description    : ����ң�������ţ����ҳ��ð�������浽flash�е��׵�ַ             
* Input          : noң�������+�����ţ�addr��flash�е�λ��
* Output         : addr
* Return         : һ��noռ�õĵ�ַ�ռ�Ϊ8���ֽ�
*				 ��	
*******************************************************************************/
__inline void rmt_f_ctlcmd(uint32_t no, uint32_t *addr)
{												
	no = no << 3;								
	*addr = no + REMOTER_CTLCMD_START_ADD;		
}

/*******************************************************************************
* Function Name  : rmt_f_des
* Description    : ����ң�������ţ����ҳ�����尴�������������浽flash�е��׵�ַ             
* Input          : noң�������+�����ţ�addr��flash�е�λ��
* Output         : addr
* Return         : һ��noռ�õĵ�ַ�ռ�Ϊ128���ֽ�
*				 ��	
*******************************************************************************/
__inline void rmt_f_des(uint32_t no, uint32_t *addr)
{											
	no = no << 7;							
	*addr = no + REMOTER_DES_START_ADD;		
}



/*******************************************************************************
* Function Name  : rmt_f_code
* Description    : ����ң�������ţ����ҳ����������������浽flash�е��׵�ַ             
* Input          : noң�������+�����ţ�addr��flash�е�λ��
* Output         : addr
* Return         : һ��noռ�õĵ�ַ�ռ�Ϊ128���ֽ�
*				 ��	
*******************************************************************************/
__inline void rmt_f_code(uint32_t no, uint32_t *addr)			   
{											
	no = no << 9;							
	*addr = no + REMOTER_CODE_START_ADD;		
}


/*******************************************************************************
* Function Name  : RMT_ALIGN(size, align)
* Description    : ���ݴ��浽flash�еĵ�ַ,���ҳ�ң�������ţ�����尴����������             
* Input          : noң�������+�����ţ�addr��flash�е�λ��
* Output         : �����������϶����ϵ
* Return         : ��RT_ALIGN(13, 4)������size 16
*				 ��	
*******************************************************************************/
#define RMT_ALIGN(size, align)			(((size) + (align) - 1) & ~((align)-1))

/*******************************************************************************
* Function Name  : RMT_ALIGN_DOWN(size, align)
* Description    : ���ݴ��浽flash�еĵ�ַ,���ҳ�ң�������ţ�����尴����������             
* Input          : noң�������+�����ţ�addr��flash�е�λ��
* Output         : �����������¶����ϵ
* Return         : ��RT_ALIGN(13, 4)������size 12
*				 ��	
*******************************************************************************/
#define RMT_ALIGN_DOWN(size, align)		((size) & ~((align) -1))

/*******************************************************************************
* Function Name  : arr_f_des
* Description    : ���ݴ��浽flash�еĵ�ַ,���ҳ�ң�������ţ�����尴������������ʼ��ַ             
* Input          : addr��flash�е�λ��
* Output         : addr
* Return         : һ��noռ�õĵ�ַ�ռ�Ϊ128���ֽ�
*				 ��	
*******************************************************************************/
__inline void arr_f_des(uint32_t *addr)					
{											
	*addr -=  REMOTER_DES_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,128);				
}

/*******************************************************************************
* Function Name  : arr_f_pdes
* Description    : ���ݴ��浽flash�еĵ�ַ,���ҳ�ң�������ţ�����尴������������ʼ��ַ             
* Input          : addr��flash�е�λ��
* Output         : addr
* Return         : һ��noռ�õĵ�ַ�ռ�Ϊ128���ֽ�
*				 ��	
*******************************************************************************/
__inline void arr_f_pdes(uint32_t *addr)					
{											
	*addr -=  REMOTER_PDES_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,128);				
}

/*******************************************************************************
* Function Name  : arr_f_code
* Description    : ���ݴ��浽flash�еĵ�ַ,���ҳ�ң�������ţ�����尴����ң����������ʼ��ַ             
* Input          : addr��flash�е�λ��
* Output         : addr
* Return         : һ��noռ�õĵ�ַ�ռ�Ϊ512���ֽ�
*				 ��	
*******************************************************************************/
__inline void arr_f_code(uint32_t *addr)				
{											
	*addr -=  REMOTER_CODE_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,512);			
}

/*******************************************************************************
* Function Name  : arr_f_ctlcmd
* Description    : ���ݴ��浽flash�еĵ�ַ,���ҳ�ң�������ţ�����尴����ң����������ʼ��ַ             
* Input          : addr��flash�е�λ��
* Output         : addr
* Return         : һ��noռ�õĵ�ַ�ռ�Ϊ512���ֽ�
*				 ��	
*******************************************************************************/
__inline void arr_f_ctlcmd(uint32_t *addr)					
{											
	*addr -=  REMOTER_CTLCMD_START_ADD;	
	*addr = RMT_ALIGN_DOWN(*addr,8);					
}

					 
////////////////////////////////////////////////////////////////////////////

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
uint8_t	 SPI_Flash_ReadSR(void);        //��ȡ״̬�Ĵ��� 
void SPI_FLASH_Write_SR(uint8_t sr);  	//д״̬�Ĵ���
void SPI_FLASH_Write_Enable(void);  //дʹ�� 
void SPI_FLASH_Write_Disable(void);	//д����
void SPI_Flash_Write_Page(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_Flash_Write_NoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite); //�޼���дSPI FLASH
void SPI_Flash_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);   //��ȡflash
void SPI_Flash_Write(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);//д��flash
void SPI_Flash_Erase_Chip(void);    	  //��Ƭ����
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr);//��������
void SPI_Flash_Wait_Busy(void);           //�ȴ�����
void SPI_Flash_PowerDown(void);           //�������ģʽ
void SPI_Flash_WAKEUP(void);			  //����




#endif
