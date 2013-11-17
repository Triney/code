

#include "spi.h"
#include "spi25xx.h" 
//#include "SysTickDelay.h" 
#include "stm32f10x_conf.h"




/**
** len + data
**	2	 1022
**  
**/




//4KbytesΪһ��Sector
//16������Ϊ1��Block
//W25X16
//����Ϊ2M�ֽ�,����32��Block,512��Sector 

//��ʼ��SPI w15xx_CS��IO��
void SPI_Flash_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /* Configure 25W16 CSpins: PB.14 ---------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	Set_SPI_W25XX_CS;
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
	Clr_SPI_W25XX_CS;                            			//ʹ������   
	SPIx_ReadWriteByte(W25X_ReadStatusReg);    				//���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPIx_ReadWriteByte(0Xff);             				//��ȡһ���ֽ�  
	Set_SPI_W25XX_CS;                            			//ȡ��Ƭѡ     
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
	Clr_SPI_W25XX_CS;                            			//ʹ������   
	SPIx_ReadWriteByte(W25X_WriteStatusReg);   				//����дȡ״̬�Ĵ�������    
	SPIx_ReadWriteByte(sr);               					//д��һ���ֽ�  
	Set_SPI_W25XX_CS;                            			//ȡ��Ƭѡ     	      
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
	Clr_SPI_W25XX_CS;                            			//ʹ������   
    SPIx_ReadWriteByte(W25X_WriteEnable);      				//����дʹ��  
	Set_SPI_W25XX_CS;                            			//ȡ��Ƭѡ     	      
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
	Clr_SPI_W25XX_CS;                            			//ʹ������   
    SPIx_ReadWriteByte(W25X_WriteDisable);    	 			//����д��ָֹ��    
	Set_SPI_W25XX_CS;                         			   	//ȡ��Ƭѡ     	      
} 	


		    
/*******************************************************************************
* Function Name  : SPI_Flash_ReadID
* Description    : ��ȡоƬID W25X16��ID:0XEF14
* Input          : 
* Output         : 
* Return         : 
*******************************************************************************/
uint16_t SPI_Flash_ReadID(void)
{
	uint16_t Temp = 0;	  
	Clr_SPI_W25XX_CS;				    
	SPIx_ReadWriteByte(0x90);								//���Ͷ�ȡID����	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	    
	SPIx_ReadWriteByte(0x00); 	 			   
	Temp =SPIx_ReadWriteByte(0xFF) << 8;  
	Temp|=SPIx_ReadWriteByte(0xFF);	 
	Set_SPI_W25XX_CS;				    
	return Temp;
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
	Clr_SPI_W25XX_CS;                            						//ʹ������   
    SPIx_ReadWriteByte(W25X_ReadData);         							//���Ͷ�ȡ����   
    SPIx_ReadWriteByte((uint8_t)((ReadAddr) >> 16)); 				 	//����24bit��ַ    
    SPIx_ReadWriteByte((uint8_t)((ReadAddr) >> 8));   
    SPIx_ReadWriteByte((uint8_t)ReadAddr);   
    for(i = 0; i < NumByteToRead; i ++)
	{ 
        pBuffer[i]=SPIx_ReadWriteByte(0XFF);   							//ѭ������  
    }
	Set_SPI_W25XX_CS;                            						//ȡ��Ƭѡ     	      
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
    SPI_FLASH_Write_Enable();                  									//SET WEL 
	Clr_SPI_W25XX_CS;                           			 					//ʹ������   
    SPIx_ReadWriteByte(W25X_PageProgram);      									//����дҳ����   
    SPIx_ReadWriteByte((uint8_t)((WriteAddr) >> 16)); 							//����24bit��ַ    
    SPIx_ReadWriteByte((uint8_t)((WriteAddr) >> 8));   
    SPIx_ReadWriteByte((uint8_t)WriteAddr);   
    for(i = 0; i < NumByteToWrite; i ++) SPIx_ReadWriteByte(pBuffer[i]);		//ѭ��д��  
	Set_SPI_W25XX_CS;                            								//ȡ��Ƭѡ 
	SPI_Flash_Wait_Busy();					   									//�ȴ�д�����
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
	pageremain = 256 - WriteAddr%256; 											//��ҳʣ����ֽ���		 	    
	if(NumByteToWrite <= pageremain) pageremain = NumByteToWrite;				//������256���ֽ�
	while(1)
	{	   
		SPI_Flash_Write_Page(pBuffer, WriteAddr, pageremain);
		if(NumByteToWrite == pageremain) break;									//д�������
	 	else 	//NumByteToWrite>pageremain
		{
			pBuffer += pageremain;
			WriteAddr += pageremain;	

			NumByteToWrite -= pageremain;			  							//��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite > 256) pageremain = 256; 							//һ�ο���д��256���ֽ�
			else pageremain = NumByteToWrite; 	  								//����256���ֽ���
		}
	};	    
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

	secpos = WriteAddr/4096;													//������ַ 0~511 for w25x16
	secoff = WriteAddr%4096;													//�������ڵ�ƫ��
	secremain = 4096-secoff;													//����ʣ��ռ��С   

	if(NumByteToWrite <= secremain) secremain = NumByteToWrite;					//������4096���ֽ�
	while(1) 
	{	
		SPI_Flash_Read(SPI_FLASH_BUF, secpos*4096, 4096);						//������������������
		for(i=0; i < secremain; i ++)											//У������
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;								//��Ҫ����  	  
		}
		if(i < secremain)														//��Ҫ����
		{
			SPI_Flash_Erase_Sector(secpos);										//�����������
			for(i = 0; i < secremain; i ++)	   									//����
			{
				SPI_FLASH_BUF[i + secoff] = pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF, secpos*4096, 4096);			//д����������  

		}
		else 
			SPI_Flash_Write_NoCheck(pBuffer, WriteAddr, secremain);				//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumByteToWrite == secremain) break;									//д�������
		else																	//д��δ����
		{
			secpos ++;															//������ַ��1
			secoff = 0;															//ƫ��λ��Ϊ0 	 

		   	pBuffer += secremain;  												//ָ��ƫ��
			WriteAddr += secremain;												//д��ַƫ��	   
		   	NumByteToWrite -= secremain;										//�ֽ����ݼ�
			if(NumByteToWrite > 4096) secremain = 4096;							//��һ����������д����
			else secremain = NumByteToWrite;									//��һ����������д����
		}	 
	};	 	 
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
    SPI_FLASH_Write_Enable();                  									//SET WEL 
    SPI_Flash_Wait_Busy();   
  	Clr_SPI_W25XX_CS;                            								//ʹ������   
    SPIx_ReadWriteByte(W25X_ChipErase);        									//����Ƭ��������  
	Set_SPI_W25XX_CS;                            								//ȡ��Ƭѡ     	      
	SPI_Flash_Wait_Busy();   				   									//�ȴ�оƬ��������
}   



/*******************************************************************************
* Function Name  : SPI_Flash_Erase_Sector
* Description    : ����һ������, Dst_Addr:������ַ 0~511 for w25x16 ,����һ��ɽ��������ʱ��:150ms         
* Input          : 
* Output         : None
* Return         : 
*******************************************************************************/
void SPI_Flash_Erase_Sector(uint32_t Dst_Addr)   
{   
	Dst_Addr *= 4096;
    SPI_FLASH_Write_Enable();                  									//SET WEL 	 
    SPI_Flash_Wait_Busy();   
  	Clr_SPI_W25XX_CS;                            								//ʹ������   
    SPIx_ReadWriteByte(W25X_SectorErase);      									//������������ָ�� 
    SPIx_ReadWriteByte((uint8_t)((Dst_Addr) >> 16));  							//����24bit��ַ    
    SPIx_ReadWriteByte((uint8_t)((Dst_Addr) >> 8));   
    SPIx_ReadWriteByte((uint8_t)Dst_Addr);  
	Set_SPI_W25XX_CS;                            								//ȡ��Ƭѡ     	      
    SPI_Flash_Wait_Busy();   				 		 	 						//�ȴ��������
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
	while ((SPI_Flash_ReadSR() & 0x01) == 0x01);   								// �ȴ�BUSYλ���
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
  	Clr_SPI_W25XX_CS;                            								//ʹ������   
    SPIx_ReadWriteByte(W25X_PowerDown);        									//���͵�������  
	Set_SPI_W25XX_CS;                            								//ȡ��Ƭѡ     	      
    delay_us(3);                               									//�ȴ�TPD  
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
  	Clr_SPI_W25XX_CS;                            								//ʹ������   
    SPIx_ReadWriteByte(W25X_ReleasePowerDown);   								//  send W25X_PowerDown command 0xAB    
	Set_SPI_W25XX_CS;                            								//ȡ��Ƭѡ     	      
    delay_us(3);                               									//�ȴ�TRES1
}   

























