#ifndef buffer_ring_h
#define buffer_ring_h

#include "stm32f10x_conf.h"

#define BufferSize 512
typedef struct CIRBUF
{
	/* buffer for reception */
	uint16_t read_index, save_index;
	uint8_t buffer[BufferSize];
}CirBufType;

bool WriteBuffer(CirBufType *pCirBuffer,uint8_t data);
bool ReadBuffer(CirBufType *pCirBuffer,uint8_t *p_data,uint16_t nNum);
void ClearBuffer(CirBufType *pCirBuffer);
uint16_t Buffer_Hold_Num(CirBufType *pCirBuffer);

#endif
