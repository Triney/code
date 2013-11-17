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
	//======================ʱ�ӳ�ʼ��=======================================
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//	//======================IO��ʼ��=======================================	

	//USART2_TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//USART2_RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//����485���ͺͽ���ģʽ
//    TODO   ��ʱ��дB13 ��֮����������ʱ���޸�
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//======================���ڳ�ʼ��=======================================
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_HardwareFlowControl =
			USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	GPIO_SetBits(GPIOC ,GPIO_Pin_3);

	ENTER_CRITICAL_SECTION(); //��ȫ���ж�

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART2, ENABLE);
	EXIT_CRITICAL_SECTION(); //��ȫ���ж�	
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

	ENTER_CRITICAL_SECTION(); //��ȫ���ж�
	
	USART_Init(USARTx, &USART_InitStructure);
	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);
  USART_Cmd(USARTx, ENABLE);
	EXIT_CRITICAL_SECTION(); //��ȫ���ж�	
}

//***************************LDS RS485���ڷ��ͺ���***************************
//��������: uint32_t lds_uart_send(USART_TypeDef* USARTx, const void* buffer, uint32_t size)
//��ڲ�������
//���ڲ�������
//��    ע��Editor��Jin   2013-11-06   Company: lds
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
	   while ((USARTx->SR & USART_FLAG_TC)==0);							/* ���ݷ��ͽ���			*/
	   while ((USARTx->SR & USART_FLAG_TXE)==0);						/* ���ݷ��ͽ���			*/
	}
	Set_LDS_485_RECEIVE();
	return (uint32_t)(ptr-(uint8_t*)buffer);		
}

//***************************LDS RS485���ڽ��պ���***************************
//��������: uint32_t lds_uart_receive(USART_TypeDef* USARTx)
//��ڲ�������
//���ڲ�������
//��    ע��Editor��Jin   2013-11-06   Company: lds
//�����ȫ�ֱ�����sm_lds_485
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
			//д�뻷�λ�����
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
			//д�뻷�λ�����
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
	//�����ж�
	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
	{
//		UartTimer_CMD_Expire_Enable();
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		lds_uart_receive(USART2);
	}
	//�����ж�
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
	//====================================ʱ�ӳ�ʼ��===========================	
	if(TIMx == TIM4)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	else
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	//====================================��ʱ����ʼ��===========================
	//��ʱ��ʱ�������˵��
	//HCLKΪ72MHz��APB1����2��ƵΪ36MHz
	//TIM3��ʱ�ӱ�Ƶ��Ϊ72MHz��Ӳ���Զ���Ƶ,�ﵽ���
	//TIM3�ķ�Ƶϵ��Ϊ3599��ʱ���Ƶ��Ϊ72 / (1 + Prescaler) = 20KHz,��׼Ϊ50us
	//TIM������ֵΪusTim1Timerout50u
	
	PrescalerValue = (uint16_t) (SystemCoreClock / 20000) - 1;
	//��ʱ��1��ʼ��
	TIM_TimeBaseStructure.TIM_Period = (uint16_t) usTim1Timerout50us;
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure);
	//Ԥװ��ʹ��
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	//====================================�жϳ�ʼ��===========================
	//����NVIC���ȼ�����ΪGroup2��0-3��ռʽ���ȼ���0-3����Ӧʽ���ȼ�
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
	//�������жϱ�־λ
	TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
	//��ʱ��4����жϹر�
	TIM_ITConfig(TIMx, TIM_IT_Update, DISABLE);
	//��ʱ��4����
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
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);	     //���жϱ��
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //�����ʱ��TIM2����жϱ�־λ
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
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);	     //���жϱ��
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	 //�����ʱ��TIM2����жϱ�־λ
		UartTimer_Update_Expire_Disable();
		receive_mode = WaitCMD;
	}
	rt_interrupt_leave();
}
