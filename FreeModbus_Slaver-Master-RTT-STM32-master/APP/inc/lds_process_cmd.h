#ifndef _LDS_PROCESS_CMD_H
#define _LDS_PROCESS_CMD_H

#include "stm32f10x_conf.h"

typedef enum
{
	WaitCMD =0,
	WaitDebugData,
	ChangeToUpdate,
	WaitUpdatedate,
	ChangeToCMD
}LDS_Receive;

extern LDS_Receive receive_mode;

bool head_fa(uint8_t *pRData,uint8_t *pSData);
bool head_f5(uint8_t *pRData,uint8_t *pSData);
bool head_fe(uint8_t *pRData,uint8_t *pSData,LDS_Receive *mode,uint8_t *pFlashData,
							uint8_t box,uint16_t *last_index,uint16_t offset);
//bool error_code(uint8_t *pRData,uint8_t *pSData,LDS_Receive *mode,uint8_t box);


#endif

