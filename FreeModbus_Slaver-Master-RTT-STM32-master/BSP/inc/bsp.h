
#ifndef  BSP_PRESENT
#define  BSP_PRESENT

/*
*********************************************************************************************************
*                                                 EXTERNS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                              INCLUDE FILES
*********************************************************************************************************
*/



#include <stm32f10x_conf.h>

/*
*********************************************************************************************************
*                                                 DEFINES
*********************************************************************************************************
*/
/* board configuration */
// <o> SDCard Driver <1=>SDIO sdcard <0=>SPI MMC card
// 	<i>Default: 1
#define STM32_USE_SDIO			0

/* whether use board external SRAM memory */
// <e>Use external SRAM memory on the board
// 	<i>Enable External SRAM memory
#define STM32_EXT_SRAM          0
//	<o>Begin Address of External SRAM
//		<i>Default: 0x68000000
#define STM32_EXT_SRAM_BEGIN    0x68000000 /* the begining address of external SRAM */
//	<o>End Address of External SRAM
//		<i>Default: 0x68080000
#define STM32_EXT_SRAM_END      0x68080000 /* the end address of external SRAM */
// </e>

// <o> Internal SRAM memory size[Kbytes] <8-64>
//	<i>Default: 64
#define STM32_SRAM_SIZE         48
#define STM32_SRAM_END          (0x20000000 + STM32_SRAM_SIZE * 1024)


#define LED_LED1_ON                GPIO_SetBits  (GPIOA,GPIO_Pin_11)  	   //LED1 
#define LED_LED1_OFF               GPIO_ResetBits(GPIOA,GPIO_Pin_11) 	   //LED1

#define LED_LED2_ON                GPIO_SetBits  (GPIOB,GPIO_Pin_12)  	   //LED2 
#define LED_LED2_OFF               GPIO_ResetBits(GPIOB,GPIO_Pin_12)
void rt_hw_board_init(void);

void IWDG_Configuration(void);
void IWDG_Feed(void);




/*********************************************************************************************************/
/**                                                 MACRO'S												 */
/***********************************************************************************************************/


//����Ӳ���汾�Ŷ���
#define VERSION_SOFTWARE_MAJOR		1
#define VERSION_SOFTWARE_MINOR		0
#define VERSION_HARDWARE_MAJOR 		1
#define VERSION_HARDWARE_MINOR 		0   

/***********************************************************************************************************/
/*                                               DATA TYPES												 */
/***********************************************************************************************************/


/**********************************************************************************************************
*                                            GLOBAL VARIABLES
**********************************************************************************************************/




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void BSP_Init(void);

void rt_hw_board_init(void);
static void RCC_Configuration(void);
static void NVIC_Configuration(void);
static void GPIO_Configuration(void);
static void USART1_Configuration(void);
static void IWDG_Configuration(void);
void IWDG_Feed(void);
static void SysTick_Configuration(void);
void rt_hw_timer_handler(void);

uint8_t AvoidTimeout(uint32_t TimeOfTimeout,uint32_t Period,uint8_t (*DetectCondition)(),uint8_t ConditionValue);
void Delay(vu32 nCount);
/*
*********************************************************************************************************
*                                           INTERRUPT SERVICES
*********************************************************************************************************
*/


#endif                                                          /* End of module include.                               */
