/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        2       /* Number of identical tasks                          */
//#define debug
#define case1

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
OS_EVENT *R1,*R2;
struct period{
    int exeTime;
    int period;
};
char CtxSwMessage[CtxSwMessageSize][50];
int CtxSwMessageTop = 0;
OS_EVENT     *printCtxSwMbox;                               /* Message mailboxes for Tasks #4 and #5         */

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
OS_STK        TaskPrintCtxSwStk[TASK_STK_SIZE];
struct period TaskData[N_TASKS];                      /* Parameters to pass to each task               */

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  printCtxSwMessage();
        void  Task1(void *data);                       /* Function prototypes of tasks                  */
        void  Task2(void *data);                       /* Function prototypes of tasks                  */
        void  Task3(void *data);                       /* Function prototypes of tasks                  */
        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);
///

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
    INT8U err1,err2;

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */


    R1 = OSMutexCreate(1,&err1);                      // resource 1
    R2 = OSMutexCreate(2,&err2);                      // resource 2
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;

    pdata = pdata;                                         /* Prevent compiler warning                 */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1u);                                    /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    printCtxSwMbox= OSMboxCreate((void *)0);               /* Create message mailboxes               */

    TaskStartCreateTasks();                                /* Create all the application tasks         */
    OSTimeSet(0);
    for (;;) {
        #ifdef debug
        printf("--- enter taskStart\n");
        #endif
        printCtxSwMessage();

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
    }
}




/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;
    INT8U err;

    OSTaskCreate(Task1, (void *)0, &TaskStk[0][TASK_STK_SIZE - 1], 3);
    OSTaskCreate(Task2, (void *)0, &TaskStk[1][TASK_STK_SIZE - 1], 4);
    #ifdef case1
    OSTaskCreate(Task3, (void *)0, &TaskStk[2][TASK_STK_SIZE - 1], 5);
    #endif
}


void printCtxSwMessage(){
        static int i=0;
        for(;i<CtxSwMessageTop;i++){
            printf("%s", CtxSwMessage[i]);
            OSTimeDly(100);                         //set print interval******************************
        }
}
/************************** case 1 *******************************************/
/* priority:                     R1=1, R2=2, Task1=3, Task2=4, Task3=5 
   arrival time:                             Task1=8, Task2=4, Task3=0
   execution&resource used Time: (c,R1,R2)
                                 Task1:(2,2,2)
                                 Task2:(2,0,4)
                                 Task3:(2,7,0)              

******************************************************************************/
#ifdef case1
void Task1(void *pdata)
{
    int start=8;                         
    int startDelayTime;
    INT8U err;
    pdata = pdata;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);
    //execution Time
    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}

    //R1 used Time
    OSTCBCur->compTime=2;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}
    
    //R2 used Time
    OSTCBCur->compTime=2;
    OSMutexPend(R2,0,&err);
    while(OSTCBCur->compTime>0){}

    OSMutexPost(R2);
    OSMutexPost(R1);

    OSTaskDel(OS_PRIO_SELF);
}
void Task2(void *pdata)
{
    int start=4;
    INT8U err;
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);
        
    pdata = pdata;
    
    //execution Time
    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}

    //R2 used Time
    OSTCBCur->compTime=4;
    OSMutexPend(R2,0,&err);
    while(OSTCBCur->compTime>0){}

    OSMutexPost(R2);

    OSTaskDel(OS_PRIO_SELF);
}

void Task3(void *pdata)
{
    int start=0;
    INT8U err;
    int startDelayTime;
    pdata = pdata;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);
    
    //execution Time
    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}
    //R1 used Time
    OSTCBCur->compTime=7;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}

    OSMutexPost(R1);

    OSTaskDel(OS_PRIO_SELF);
}
#else


/************************** case 2 *******************************************/
/* priority:                     R1=1, R2=2, Task1=3, Task2=4 
   arrival time:                             Task1=5, Task2=0
   execution&resource used Time: 
                                 Task1:(c:2,R2:3, R1:3, R2:3)
                                 Task2:(c:2,R1:6, R2:2, R1:2)             

******************************************************************************/
void Task1(void *pdata)
{
    int start=5;
    int startDelayTime;
    INT8U err;
    pdata = pdata;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);
    
    //execution Time
    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}
    //R2 used Time
    OSTCBCur->compTime=3;
    OSMutexPend(R2,0,&err);
    while(OSTCBCur->compTime>0){}
    //R1 used Time
    OSTCBCur->compTime=3;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R1);
    //R2 used Time
    OSTCBCur->compTime=3;
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R2);

    OSTaskDel(OS_PRIO_SELF);
}
void Task2(void *pdata)
{
    int start=0;
    INT8U err;
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);
        
    pdata = pdata;
    
    //execution Time
    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}
    //R1 used Time
    OSTCBCur->compTime=6;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}
    //R2 used Time
    OSTCBCur->compTime=2;
    OSMutexPend(R2,0,&err);
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R2);
    //R1 used Time
    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R1);

    OSTaskDel(OS_PRIO_SELF);
}
#endif