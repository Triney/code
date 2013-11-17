#include "app_task.h"
#include "myiic.h"
#include "spi_flash.h"
#include "lds_485.h"
#include "lds_process_cmd.h"

#ifdef __CC_ARM
extern int Image$$RW_IRAM1$$ZI$$Limit;
#elif __ICCARM__
#pragma section="HEAP"
#else
extern int __bss_end;
#endif

uint8_t CpuUsageMajor, CpuUsageMinor; //CPU使用率
USHORT  usModbusUserData[MB_PDU_SIZE_MAX];
UCHAR   ucModbusUserData[MB_PDU_SIZE_MAX];
//====================操作系统各线程优先级==================================
#define thread_lds_uart_prio  				12
#define thread_SysMonitor_Prio		    	11
#define thread_ModbusSlavePoll_Prio      	10
#define thread_ModbusMasterPoll_Prio      	 9

ALIGN(RT_ALIGN_SIZE)
//====================操作系统各线程堆栈====================================
static rt_uint8_t thread_lds_uart_stack[4096];
static rt_uint8_t thread_SysMonitor_stack[256];
static rt_uint8_t thread_ModbusSlavePoll_stack[512];
static rt_uint8_t thread_ModbusMasterPoll_stack[512];

struct rt_thread thread_lds_uart;
struct rt_thread thread_SysMonitor;
struct rt_thread thread_ModbusSlavePoll;
struct rt_thread thread_ModbusMasterPoll;


uint8_t master_slave = 0;

//***************************系统监控线程***************************
//函数定义: void thread_entry_SysRunLed(void* parameter)
//入口参数：无
//出口参数：无
//备    注：Editor：Armink   2013-08-02   Company: BXXJS
//******************************************************************
uint8_t result;
void thread_entry_SysMonitor(void* parameter)
{
	while (1)
	{
		LED_LED1_ON;
		LED_LED2_ON;
		rt_thread_delay(DELAY_SYS_RUN_LED);
		LED_LED1_OFF;
		LED_LED2_OFF;
		rt_thread_delay(DELAY_SYS_RUN_LED);
		IWDG_Feed(); //喂狗
		//Test Modbus Master
		usModbusUserData[0] = (USHORT)(rt_tick_get()/10);
		usModbusUserData[1] = (USHORT)(rt_tick_get()%10);
		ucModbusUserData[0] = 0x1F;
//		eMBMasterReqReadDiscreteInputs(1,3,8);
//		eMBMasterReqWriteMultipleCoils(1,3,5,ucModbusUserData);
//		eMBMasterReqWriteCoil(1,8,0xFF00);
//		eMBMasterReqReadCoils(1,3,8);
//		eMBMasterReqReadInputRegister(1,3,2);
//		eMBMasterReqWriteHoldingRegister(1,3,usModbusUserData[0]);
//		eMBMasterReqWriteMultipleHoldingRegister(1,3,2,usModbusUserData);
//		eMBMasterReqReadHoldingRegister(1,3,2);
//		eMBMasterReqReadWriteMultipleHoldingRegister(1,3,2,usModbusUserData,5,2);
	}
}

void thread_entry_lds_uart(void* parameter)
{
	uint8_t i,receive_data[8],send_data[8],update_data[300];
	uint16_t last_index = 0,ReceNum;

	receive_mode = WaitCMD;
	lds_uart_init();
	spi_test();
	timeout_init(TIM4); 
	timeout_init(TIM5);
//	sm_lds_485 = lds_RQ_send;
	while(1)
	{
		if(sm_lds_485 == lds_timeout)
		{
			ReceNum = Buffer_Hold_Num(&lds_uart_buffer);
			if(ReceNum < 8)
			{
				ReadBuffer(&lds_uart_buffer,receive_data,ReceNum);
				sm_lds_485 = lds_idle;
			}
			else
				if(ReceNum == 8)
				{
					ReadBuffer(&lds_uart_buffer,receive_data,ReceNum);
					if(receive_data[7] == CheckSum(receive_data,7))
					{
						switch(receive_data[0])
						{
							case 0xfa:
								break;
							case 0xfe:
								if(head_fe(receive_data,send_data,&receive_mode,update_data,0,&last_index,0))
									sm_lds_485 = lds_RQ_send;
								break;
							default:break;
						}
					}
					else
					{
						sm_lds_485 = lds_idle;
					}
						
				}
				else
				{
					if(ReceNum >264)
						ClearBuffer(&lds_uart_buffer);
					else
					{
						ReadBuffer(&lds_uart_buffer,update_data,ReceNum);
						if(update_data[ReceNum-1] == CheckSum(update_data,ReceNum-1))
						{
							if(head_fe(update_data,send_data,&receive_mode,update_data,0,&last_index,7))
								sm_lds_485 = lds_RQ_send;
						}

					}
				}

		}
		if(sm_lds_485 == lds_RQ_send )
		{
			sm_lds_485 = lds_sending;
			lds_uart_send(USART2,send_data,8);
			if(receive_mode == ChangeToUpdate)
			{
//				USART_ReConfig(USART2,115200);
				receive_mode = WaitUpdatedate;
			}
			else
			{
				if(receive_mode == ChangeToCMD)
				{
//					USART_ReConfig(USART2,9600);
					receive_mode = WaitCMD;
					last_index = 0;
				}
			}
			sm_lds_485 = lds_idle;
		}
	}
}



//************************ Modbus从机轮训线程***************************
//函数定义: void thread_entry_ModbusSlavePoll(void* parameter)
//入口参数：无
//出口参数：无
//备    注：Editor：Armink   2013-08-02    Company: BXXJS
//******************************************************************
void thread_entry_ModbusSlavePoll(void* parameter)
{
	eMBInit(MB_RTU, 0x01, 1, 9600,  MB_PAR_NONE);
	eMBEnable();
	while (1)
	{
		eMBPoll();
		rt_thread_delay(DELAY_MB_SLAVE_POLL);
	}
}

//************************ Modbus主机轮训线程***************************
//函数定义: void thread_entry_ModbusMasterPoll(void* parameter)
//入口参数：无
//出口参数：无
//备    注：Editor：Armink   2013-08-28    Company: BXXJS
//******************************************************************
void thread_entry_ModbusMasterPoll(void* parameter)
{
	eMBMasterInit(MB_RTU, 1, 9600,  MB_PAR_NONE);
	eMBMasterEnable();
	while (1)
	{
		eMBMasterPoll();
		rt_thread_delay(DELAY_MB_MASTER_POLL);
	}
}

//**********************系统初始化函数********************************
//函数定义: int rt_application_init(void)
//入口参数：无
//出口参数：无
//备    注：Editor：Liuqiuhu   2013-1-31   Company: BXXJS
//********************************************************************
int rt_application_init(void)
{
	rt_thread_init(&thread_lds_uart,"LDS_UART",thread_entry_lds_uart,
			RT_NULL, thread_lds_uart_stack,sizeof (thread_lds_uart_stack),
			thread_lds_uart_prio,5);
	rt_thread_startup(&thread_lds_uart);
			
	
	rt_thread_init(&thread_SysMonitor, "SysMonitor", thread_entry_SysMonitor,
			RT_NULL, thread_SysMonitor_stack, sizeof(thread_SysMonitor_stack),
			thread_SysMonitor_Prio, 5);
	rt_thread_startup(&thread_SysMonitor);
  
//	if(master_slave !=0)
//	{
//		rt_thread_init(&thread_ModbusSlavePoll, "MBSlavePoll",
//				thread_entry_ModbusSlavePoll, RT_NULL, thread_ModbusSlavePoll_stack,
//				sizeof(thread_ModbusSlavePoll_stack), thread_ModbusSlavePoll_Prio,
//				5);
//		rt_thread_startup(&thread_ModbusSlavePoll);
//	}
//	else
//	{
//		rt_thread_init(&thread_ModbusMasterPoll, "MBMasterPoll",
//				thread_entry_ModbusMasterPoll, RT_NULL, thread_ModbusMasterPoll_stack,
//				sizeof(thread_ModbusMasterPoll_stack), thread_ModbusMasterPoll_Prio,
//				5);
//		rt_thread_startup(&thread_ModbusMasterPoll);
//	}
	return 0;
}

//**************************初始化RT-Thread函数*************************************
//函数定义: void rtthread_startup(void)
//入口参数：无
//出口参数：无
//备    注：Editor：Armink 2011-04-04    Company: BXXJS
//**********************************************************************************
void rtthread_startup(void)
{
	/* init board */
	rt_hw_board_init();

	/* show version */
	rt_show_version();

	/* init tick */
	rt_system_tick_init();

	/* init kernel object */
	rt_system_object_init();

	/* init timer system */
	rt_system_timer_init();

#ifdef RT_USING_HEAP
#ifdef __CC_ARM
	rt_system_heap_init((void*)&Image$$RW_IRAM1$$ZI$$Limit, (void*)STM32_SRAM_END);
#elif __ICCARM__
	rt_system_heap_init(__segment_end("HEAP"), (void*)STM32_SRAM_END);
#else
	/* init memory system */
	rt_system_heap_init((void*)&__bss_end, (void*)STM32_SRAM_END);
#endif
#endif

	/* init scheduler system */
	rt_system_scheduler_init();

	/* init all device */
	rt_device_init_all();

	/* init application */
	rt_application_init();

#ifdef RT_USING_FINSH
	/* init finsh */
	finsh_system_init();
	finsh_set_device("uart1");
#endif

	/* init timer thread */
	rt_system_timer_thread_init();

	/* init idle thread */
	rt_thread_idle_init();

	/* Add CPU usage to system */
	cpu_usage_init();

	/* start scheduler */
	rt_system_scheduler_start();

	/* never reach here */
	return;
}

