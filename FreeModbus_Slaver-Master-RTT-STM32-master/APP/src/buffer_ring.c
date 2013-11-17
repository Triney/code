#include "stm32f10x_conf.h"
#include "buffer_ring.h"
bool WriteBuffer(CirBufType *pCirBuffer,uint8_t data)
{
	if (pCirBuffer->read_index==((pCirBuffer->save_index+1)%BufferSize))
		return FALSE;
	else
	{ 
		pCirBuffer->buffer[pCirBuffer->save_index++]=data;
		pCirBuffer->save_index=pCirBuffer->save_index%BufferSize;
		return TRUE; 
	}
}

bool ReadBuffer(CirBufType *pCirBuffer,uint8_t *p_data,uint16_t nNum)
{
	if (pCirBuffer->read_index==pCirBuffer->save_index)

		return FALSE;
	else
	{ 
		while(nNum--)
		{
			*p_data++ = pCirBuffer->buffer[pCirBuffer->read_index++];
			pCirBuffer->read_index=pCirBuffer->read_index%BufferSize;
		}
		return TRUE;
	}
}

void ClearBuffer(CirBufType *pCirBuffer)
{
	pCirBuffer->read_index = pCirBuffer->save_index;
}

uint16_t Buffer_Hold_Num(CirBufType *pCirBuffer)
{
	uint16_t ucNum;
	if(pCirBuffer->read_index == pCirBuffer->save_index)
		return ucNum = 0;
	else
		if(pCirBuffer->read_index < pCirBuffer->save_index)
		{
			ucNum = pCirBuffer->save_index - pCirBuffer->read_index;
		}
		else 
		{
			ucNum = BufferSize + pCirBuffer->save_index - pCirBuffer->read_index;
		}
		
	return ucNum;		
}
