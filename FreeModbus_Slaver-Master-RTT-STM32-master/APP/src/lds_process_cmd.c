#include "port.h"
#include "LDS_485.h"
#include "buffer_ring.h"
#include "lds_process_cmd.h"
#include "w25x16.h"

#include "app_task.h"

LDS_Receive receive_mode;

bool head_fa(uint8_t *pRData,uint8_t *pSData)
{
	if(*(pRData+1) == 0x08)
	{
		;
	}
	return FALSE;
}

bool head_f5(uint8_t *pRData,uint8_t *pSData)
{
	return FALSE;
}
/*********************************************************************************************************
** Function name:       head_fe
** Descriptions:        处理帧头为0XFE的命令包
** input parameters:    *pRData  :收到数据的数组指针;												
												box			 :本机的BOX号
												offset	 :pSData的偏移量
** output parameters:   *pSData  :发送数据的数组指针;
** Returned value:      is_need_ack
*********************************************************************************************************/
bool head_fe(uint8_t *pRData,uint8_t *pSData,LDS_Receive *mode,uint8_t *pFlashData,
							uint8_t box,uint16_t *last_index,uint16_t offset)
{
	uint8_t data,is_need_ack = 0;
	uint8_t data_ack[260];
	uint16_t i,this_index;
	data = *(pRData+1);
	if(data == 0x00)
	{
		data = *(pRData+2);
		switch(data)
		{
			case 0xf0:
			{
				for(i=0;i<60;i++)
				{
					SPI_Flash_Erase_Sector(i);
				}
				if(*mode ==WaitCMD)
				{
					*mode = ChangeToUpdate;
					*last_index = 0;
					is_need_ack = 1;
					*(pSData++) = 0xfe;
					*(pSData++) = box;
					*(pSData++) = 0xe0;
					*(pSData++) = 0;
					*(pSData++) = 0;
					*(pSData++) = 0;
					*(pSData++) = 0;
					*pSData = CheckSum(pSData-7,7);
					
				}
				else
					is_need_ack = 0;

				break;
			}
			case 0xf1:
			{
//				USART_ReConfig(USART2,9600);
				if(*mode != WaitUpdatedate)
				{
					is_need_ack = 0;
				}
				else
				{
					*mode =	ChangeToCMD;
//					*last_index = 0;
					is_need_ack = 1;
					*(pSData++) = 0xfe;
					*(pSData++) = box;
					*(pSData++) = 0xe0;
					*(pSData++) = *(pRData+3);
					*(pSData++) = *(pRData+4);
					*(pSData++) = *(pRData+5);
					*(pSData++) = *(pRData+6);
					*pSData = CheckSum(pSData-7,7);		
				}					
				break;
			}
			case 0xf3:
			{
				if(*mode == WaitUpdatedate)
				{

					this_index = (uint16_t) (((*(pRData+5))<<8)|(*(pRData+6)));
					pFlashData = pFlashData+offset;
//					if(*last_index == (this_index-1))
					{
						SPI_Flash_Write(pFlashData,(this_index-1)*256,256);
						SPI_Flash_Read(data_ack,(this_index-1)*256,256);
						for(i=0;i<256;i++)
						{
							if(data_ack[i] != *(pFlashData+i))
							{
								break;
							}
						}
						if(i>=256)
						{
							is_need_ack = 1;
//							*last_index = this_index;
							*(pSData++) = 0xfe;
							*(pSData++) = box;
							*(pSData++) = 0xe0;
							*(pSData++) = *(pRData+3);
							*(pSData++) = *(pRData+4);
							*(pSData++) = *(pRData+5);
							*(pSData++) = *(pRData+6);
							*pSData = CheckSum(pSData-7,7);	
						}	
//						else
//						{
//							is_need_ack = 0;
//						}							
					}
//					else
//					{
//						if(*last_index == this_index)
//						{
//							*last_index -=1;
//						}
//						is_need_ack = 1;
//						*(pSData++) = 0xfe;
//						*(pSData++) = box;
//						*(pSData++) = 0xe1;
//						*(pSData++) = *(pRData+3);
//						*(pSData++) = *(pRData+4);
//						*(pSData++) = (uint8_t) (*last_index >>8);
//						*(pSData++) = (uint8_t) (*last_index);
//						*pSData = CheckSum(pSData-7,7);	
//					}
				}
				else
					is_need_ack = 0;
				break;
			}
			default:break;
		}
//		UartTimer_Update_Expire_Enable();
	}
	return is_need_ack;
}

//bool error_code(uint8_t *pRData,uint8_t *pSData,LDS_Receive *mode,uint8_t box)
//{
//	*(pSData++) = 0xfe;
//	*(pSData++) = box;
//	*(pSData++) = 0xe2;
//	*(pSData++) = *(pRData+3);
//	*(pSData++) = *(pRData+4);
//	*(pSData++) = *(pRData+5);
//	*(pSData++) = *(pRData+6);
//	*pSData = CheckSum(pSData-7,7);	
//	return 1;
//}

