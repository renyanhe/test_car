/*-----------------------------------------------------------------------------------*/
/* 文件名称  OSState.c
 * 功能: 统计任务初始化，建立统计任务. 
 *作者;lee-2013-6-9
*/
#include "FreeRTOS.h"
#include "task.h"
#include "cpustate.h"
#include "StackMacros.h"

#if OS_TASK_STAT_EN>0

volatile  unsigned int  OSIdleCtr;                                 /* Idle counter                   */
unsigned int      OSIdleCtrMax;             /* Max. value that idle ctr can take in 1 sec.     */
unsigned int      OSIdleCtrRun;             /* Val. reached by idle ctr at run time in 1 sec.  */
float     OSCPUUsage;               /* Percentage of CPU used  */
void OS_TaskStat(void * pvParameters);//统计任务

#if OS_TASK_STAT_EN>0

/*-----------------------------------------------------------------------------------*/
/* 函数名称  OSStatInit
 * 函数功能: 统计任务初始化，建立统计任务. 
 *   入口参数：
 *  无
 *注意事项：本函数必须在空闲函数调用后，且其他函数没掉用前调用！！
 *   返回参数：无
 *作者;lee-2013-6-9
*/
void  OSStatInit (void)
{
	  taskENTER_CRITICAL();						 //关中断
      OSIdleCtr    = 0;                          /* Clear idle counter                                 */
      taskEXIT_CRITICAL();
      vTaskDelay(100);           /* Determine MAX. idle counter value for 1/10 second ,100ms延时 */
      taskENTER_CRITICAL();
      OSIdleCtrMax = OSIdleCtr;                    /* Store maximum idle counter count in 1/10 second，获取100ms内OSIdleCtrMax加到的最大值    */
 
      taskEXIT_CRITICAL();						//开中断


	xTaskCreate(OS_TaskStat, "OS_TaskStat", TaskStat_STACK_SIZE, ( void * ) NULL,STAT_TASK_PRIO, NULL);//创建统计任务，优先级仅高于空闲任务
}

void vApplicationIdleHook( void )//空闲任务的钩子函数，空闲的时候，次函数会被执行
{
   taskENTER_CRITICAL();						 //关中断
   OSIdleCtr++;//空闲计数器加一
   taskEXIT_CRITICAL();						//开中断
}

void OS_TaskStat(void * pvParameters)//统计任务
{
   	  OSIdleCtrMax /= 100;
    if (OSIdleCtrMax == 0)
	{
       OSCPUUsage = 0;
    }
	    for (;;)
	{
        taskENTER_CRITICAL();
        OSIdleCtrRun = OSIdleCtr;                /* Obtain the of the idle counter for the past second */
        OSIdleCtr    = 0;                      /* Reset the idle counter for the next second         */
        taskEXIT_CRITICAL();	
        OSCPUUsage   = (100 - OSIdleCtrRun /(float) OSIdleCtrMax);
		OSCPUUsage =ABS(OSCPUUsage);//取绝对值，防止意外产生
        vTaskDelay(100);       /* Accumulate OSIdleCtr for the next 1/10 second   100ms统计一次   */
		//STM_EVAL_LEDToggle(LED1);
    }
}
#endif	
#endif /* __CPUSTATE_H__ */
