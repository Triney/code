#ifndef _LDS_485
#define _LDS_485

#include <stm32f10x_conf.h>
//#include "mbconfig.h"
//#include <rthw.h>
//#include <rtthread.h>

#include <assert.h>
#include <inttypes.h>
#include "buffer_ring.h"

#include "lds_process_cmd.h"
typedef enum _lds_485_state
{
	lds_idle = 0,
	lds_receiving,
	lds_RQ_send,
	lds_sending,
	lds_timeout
}lds_state;

#define Set_LDS_485_SEND() {GPIO_ResetBits(GPIOC ,GPIO_Pin_3);}
#define Set_LDS_485_RECEIVE() {GPIO_SetBits(GPIOC ,GPIO_Pin_3);}

extern lds_state sm_lds_485;
extern CirBufType lds_uart_buffer;
extern uint8_t RcvNum;

uint8_t CheckSum(uint8_t *buffer,uint16_t Num);
void lds_uart_init(void);
void USART_ReConfig(USART_TypeDef* USARTx,uint32_t baud);
uint32_t lds_uart_send(USART_TypeDef* USARTx, const void* buffer, uint32_t size);
void lds_uart_receive(USART_TypeDef* USARTx);
void timeout_init(TIM_TypeDef* TIMx);
void UartTimer_CMD_Expire_Enable(void);
void UartTimer_Expire_Disable(void);
void UartTimer_Update_Expire_Enable(void);
void UartTimer_Update_Expire_Disable(void);
#endif
