//#include "port.h"
#include "LDS_485.h"
#include "buffer_ring.h"

uint8_t CheckSum(uint8_t *buffer,uint16_t Num)
{
	uint8_t sum=0;
	uint16_t i;
	for(i=Num;i!=0;i--)
	{
		sum+=*buffer++;
		sum = sum%256;
	}
	sum = 256-sum;
	return sum;
}

void lds_uart_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//	USART_ClockInitTypeDef USART_ClockInitStructure;
	//	NVIC_InitTypeDef NVIC_InitStructure;
	//======================时钟初始化=======================================
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	//======================IO初始化=======================================	

	//USART2_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART2_RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//配置485发送和接收模式
//    TODO   暂时先写B13 等之后组网测试时再修改
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//======================串口初始化=======================================
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	GPIO_SetBits(GPIOC ,GPIO_Pin_3);

	ENTER_CRITICAL_SECTION(); //关全局中断

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);
	EXIT_CRITICAL_SECTION(); //开全局中断	
}

void USART_ReConfig(USART_TypeDef* USARTx,uint32_t baud)
{ 
  USART_InitTypeDef USART_InitStructure; 
  USART_Cmd(USARTx, DISABLE);
  USART_InitStructure.USART_BaudRate =baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	ENTER_CRITICAL_SECTION(); //关全局中断
	
	USART_Init(USARTx, &USART_InitStructure);
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
  USART_Cmd(USARTx, ENABLE);
	EXIT_CRITICAL_SECTION(); //开全局中断	
}

//***************************LDS RS485串口发送函数***************************
//函数定义: uint32_t lds_uart_send(USART_TypeDef* USARTx, const void* buffer, uint32_t size)
//入口参数：无
//出口参数：无
//备    注：Editor：Jin   2013-11-06   Company: lds
//******************************************************************
uint32_t lds_uart_send(USART_TypeDef* USARTx, const void* buffer, uint32_t size)
{
	uint8_t* ptr;
	uint8_t a =255;
	ptr = (uint8_t*)buffer;
	Set_LDS_485_SEND();
//	while(a--);
	while (size)
	{
		USARTx->DR = (*ptr & 0x1FF);
		++ptr; --size;
	   while ((USARTx->SR & USART_FLAG_TC)==0);							/* 数据发送结束			*/
	   while ((USARTx->SR & USART_FLAG_TXE)==0);						/* 数据发送结束			*/
	}
	Set_LDS_485_RECEIVE();
	return (uint32_t)(ptr-(uint8_t*)buffer);		
}

//***************************LDS RS485串口接收函数***************************
//函数定义: uint32_t lds_uart_receive(USART_TypeDef* USARTx)
//入口参数：无
//出口参数：无
//备    注：Editor：Jin   2013-11-06   Company: lds
//所需的全局变量：sm_lds_485
//******************************************************************
lds_state sm_lds_485;
CirBufType lds_uart_buffer;
uint8_t RcvNum;
void lds_uart_receive(USART_TypeDef* USARTx)
{
	uint8_t dat;
	dat = USART_ReceiveData(USARTx);
	if(sm_lds_485== lds_idle)
	{
		if((dat == 0xfa)
				||(dat == 0xfc)
				||(dat == 0xf5)
				||(dat == 0xfe))
		{
			sm_lds_485 = lds_receiving;
			//写入环形缓冲区
//			ENTER_CRITICAL_SECTION();
			WriteBuffer(&lds_uart_buffer,dat);
			RcvNum = 7;
			UartTimer_CMD_Expire_Enable();
//			EXIT_CRITICAL_SECTION();
		}
	}
	else 
		if(sm_lds_485 == lds_receiving)
		{
			//写入环形缓冲区
			WriteBuffer(&lds_uart_buffer,dat);
//			RcvNum--;
//			if(RcvNum == 0)
//			{
//				sm_lds_485 = lds_idle;
////				UartTimer_Expire_Disable();
//			}
//			else
				UartTimer_CMD_Expire_Enable();
		}
}

void USART2_IRQHandler(void)
{
	rt_interrupt_enter();
	//接收中断
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
//		UartTimer_CMD_Expire_Enable();
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		lds_uart_receive(USART2);
	}
	//发送中断
	if (USART_GetITStatus(USART2, USART_IT_TXE) == SET)
	{
//		USART_ClearITPendingBit(USART2, USART_IT_TXE);
	}
	rt_interrupt_leave();
}



void timeout_init(TIM_TypeDef* TIMx)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		uint16_t PrescalerValue = 0;
	uint16_t usTim1Timerout50us = 240;
	
	if((TIMx != TIM4)&&(TIMx != TIM5))
		return;	
	//====================================时钟初始化===========================	
	if(TIMx == TIM4)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	//====================================定时器初始化===========================
	//定时器时间基配置说明
	//HCLK为72MHz，APB1经过2分频为36MHz
	//TIM3的时钟倍频后为72MHz（硬件自动倍频,达到最大）
	//TIM3的分频系数为3599，时间基频率为72 / (1 + Prescaler) = 20KHz,基准为50us
	//TIM最大计数值为usTim1Timerout50u
	
	PrescalerValue = (uint16_t) (SystemCoreClock / 20000) - 1;
	//定时器1初始化
	TIM_TimeBaseStructure.TIM_Period = (uint16_t) usTim1Timerout50us;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	//预装载使能
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	//====================================中断初始化===========================
	//设置NVIC优先级分组为Group2：0-3抢占式优先级，0-3的响应式优先级
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	if(TIMx == TIM4)
	{
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	}
	else
	{
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	}
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//清除溢出中断标志位
	TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
	//定时器4溢出中断关闭
	TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);
	//定时器4禁能
	TIM_Cmd(TIMx, DISABLE);
//	TIM_Cmd(TIM4, ENABLE);		
}

void UartTimer_CMD_Expire_Enable(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	/* Set current timer mode,don't change it.*/

	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 20000) - 1;;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 100;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM4, 0);
	TIM_Cmd(TIM4, ENABLE);
}

void UartTimer_Expire_Disable(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
	TIM_SetCounter(TIM4, 0);
	TIM_Cmd(TIM4, DISABLE);
}

void UartTimer_Update_Expire_Enable(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	/* Set current timer mode,don't change it.*/

	TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 2000) - 1;;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = 10000;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM5, 0);
	TIM_Cmd(TIM5, ENABLE);
}

void UartTimer_Update_Expire_Disable(void)
{
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	TIM_ITConfig(TIM5, TIM_IT_Update, DISABLE);
	TIM_SetCounter(TIM5, 0);
	TIM_Cmd(TIM5, DISABLE);
}



void TIM4_IRQHandler(void)
{
	rt_interrupt_enter();
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);	     //清中断标记
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //清除定时器TIM2溢出中断标志位
		UartTimer_Expire_Disable();
		sm_lds_485 = lds_timeout;
	}
	rt_interrupt_leave();
}

void TIM5_IRQHandler(void)
{
	rt_interrupt_enter();
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);	     //清中断标记
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //清除定时器TIM2溢出中断标志位
		UartTimer_Update_Expire_Disable();
		receive_mode = WaitCMD;
	}
	rt_interrupt_leave();
}

