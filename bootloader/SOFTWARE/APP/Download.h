/*-------------------------------------------------------------------------

                            下载部分头文件

                            
-------------------------------------------------------------------------*/


#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_type.h"


void FLASH_ProgramStart(void);
void FLASH_ProgramDone(void);
u32 FLASH_WriteBank(u8 *pData, u32 addr, u16 size);



#endif
/********************** END ***************************************************/

