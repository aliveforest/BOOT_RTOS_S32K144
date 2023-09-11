/*
 * RTOS_Task.c
 *
 *  Created on: 2023年9月8日
 *      Author: dengtongbei
 */
#include "RTOS_Task.h"
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"

/* SDKincludes. */
#include "Cpu.h"
#include "HardwareLib.h"

#define APP_START_ADDRESS 0x00009000ul // app 初始化地址
uint32_t appEntry, appstack;

/* Bootloader to App  */
void Boot_to_App(uint32_t appEntry, uint32_t appstack);

void init_fcn(void); /* 外设初始化函数 */
/***********************************************************************/
/* RTOS入口函数 */
void rtos_start(void){

	init_fcn(); /* 外设初始化函数 */

    /* 从给定的内存地址中读取一32位的无符号整数，并将其存储在变量appstack */
	appstack = *(uint32_t *)(APP_START_ADDRESS);
	 /* 偏移4位，取出PC指针(Program Counter)的初始值--复位向量 */
	appEntry = *(volatile uint32_t *)(APP_START_ADDRESS + 4);

	xTaskCreate( (TaskFunction_t        ) start_task,
                 (const char *          ) "start_task",
                 (configSTACK_DEPTH_TYPE) START_TASK_STACK_SIZE,
                 (void *                ) NULL,
				 (UBaseType_t           ) START_TASK_PRIO,
                 (TaskHandle_t *        ) &start_task_handler );

    vTaskStartScheduler();  /* 启动任务和计时器。*/
}
/***********************************************************************/
/* 外设初始化函数 */
void init_fcn(void){

    CLOCK_DRV_Init(&clockMan1_InitConfig0);             /* Initialize clock     */
	systick_later_init();                               /* Initialize later fcn */
	LPUART1_init();                                     /* Initialize LPUART1   */
	RGB_LED_KEY_init();                                 /* Initialize LED KEY   */
	FLASH_DRV_Init(&Flash1_InitConfig0, &flashConfig);  /* Initialize flash     */
	LPIT0_init();
}
/***********************************************************************/
/* 开始任务创建 */
void start_task(void){
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate( (TaskFunction_t        ) key_task,
                 (const char *          ) "key_task",
                 (configSTACK_DEPTH_TYPE) KEY_TASK_STACK_SIZE,
                 (void *                ) NULL,
                 (UBaseType_t           ) KEY_TASK_PRIO,
                 (TaskHandle_t *        ) &key_task_handler );
    xTaskCreate( (TaskFunction_t        ) show_task,
                 (const char *          ) "show_task",
                 (configSTACK_DEPTH_TYPE) SHOW_TASK_STACK_SIZE,
                 (void *                ) NULL,
                 (UBaseType_t           ) SHOW_TASK_PRIO,
                 (TaskHandle_t *        ) &show_task_handler );
//	xTaskCreate( (TaskFunction_t        ) cpu_task,
//				 (const char *          ) "cpu_task",
//				 (configSTACK_DEPTH_TYPE) CPU_TASK_STACK_SIZE,
//				 (void *                ) NULL,
//				 (UBaseType_t           ) CPU_TASK_PRIO,
//				 (TaskHandle_t *        ) &cpu_task_handler );

    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}
/***********************************************************************/

/* key */
void key_task(void){
    for (;;){
//    	LPUART1_printf("key_task\r\n");
        if(SW2_key()){
			LPUART1_printf("KEY2 press!\r\n");
			later_ms(500);
			LPUART1_printf("	Enter APP...\r\n");
			later_ms(500);
			Boot_to_App(appEntry, appstack);
		}
		else if(SW3_key()){ /* update APP */
			LPUART1_printf("KEY3 press!\r\n");
			taskENTER_CRITICAL();               /* 进入临界区 */
			LPUART1_printf("Waiting for receive...\r\n");
			if(Ymodem_Receive_File(Flash_Start_Address, NAK_TIMEOUT)==RE_OK){
				LPUART1_printf("	Enter APP...\r\n");
				later_ms(500);
				Boot_to_App(appEntry, appstack);
			}
			taskEXIT_CRITICAL();                /* 退出临界区 */
		}
        vTaskDelay(200);
    }
}
/* show information */
void show_task(void){
    for (;;){
        LPUART1_printf("_____BootLoader_____ \r\n");
        vTaskDelay(500);
    }
}
/* CPU 负载率计算 task */
void cpu_task(void)
{
    uint8_t CPU_RunInfo[400];		//保存任务运行时间信息

    for (;;) {
        memset(CPU_RunInfo,0,400);				//信息缓冲区清零

        vTaskList((char *)&CPU_RunInfo);  //获取任务运行时间信息

        LPUART1_printf("-----------------------------------------------------\r\n");
        LPUART1_printf("TaskName	     Status    Prio  StackLeft TaskNum\r\n");
        LPUART1_printf("%s", CPU_RunInfo);
        LPUART1_printf("-----------------------------------------------------\r\n");

        memset(CPU_RunInfo,0,400);				//信息缓冲区清零

        vTaskGetRunTimeStats((char *)&CPU_RunInfo);

        LPUART1_printf("TaskName	      RunCount     CPU Utilization \r\n");
        LPUART1_printf("%s", CPU_RunInfo);
        LPUART1_printf("-----------------------------------------------------\r\n\n");
//        later_ms(500);
        vTaskDelay(1000);   /* 延时 */
    }
}

/* Bootloader to App  */
void Boot_to_App(uint32_t appEntry, uint32_t appstack) {
	static void (*jump_to_application)(void);
	static uint32_t stack_pointer;
	jump_to_application = (void (*)(void))appEntry; /*函数指针指向app的复位向量表的地址。注意将地址强转成函数入口地址 */
	stack_pointer = appstack;
	S32_SCB->VTOR = (uint32_t)APP_START_ADDRESS; // 设置中断向量
	INT_SYS_DisableIRQGlobal();					 // 关闭全局中断
	__asm volatile("MSR msp, %0\n" : : "r"(stack_pointer) : "sp"); // 设置堆栈指针
	__asm volatile("MSR psp, %0\n" : : "r"(stack_pointer) : "sp");
	jump_to_application(); // 进行跳转
}

