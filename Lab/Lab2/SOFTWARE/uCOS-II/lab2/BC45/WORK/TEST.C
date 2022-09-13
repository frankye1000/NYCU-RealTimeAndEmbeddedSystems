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
#define  N_TASKS                        3       /* Number of identical tasks                          */
//#define debug

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
struct period{
    int exeTime;
    int period;
};
char CtxSwMessage[CtxSwMessageSize][100];
int CtxSwMessageTop = 0;
char **debugMessage;
int debugMessTop= 0;
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
void ArgumentSet(void);
void printTCBList();
void printDebugMess();
        void printCtxSwMessage();
        void  Task();                       /* Function prototypes of tasks                  */
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
    // int i=0;
    // debugMessage=(char**)malloc(sizeof(char*)*1000);
    // for(;i<1000;i++){
    //     debugMessage[i] = (char*)malloc(100);
    // }
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */
    OSTCBPrioTbl[OS_IDLE_PRIO]->deadLine = 1002; //????

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSTCBPrioTbl[0]->deadLine = 1;               //????
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
    int i=0;

    pdata = pdata;                                         /* Prevent compiler warning                 */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1u);    //自己設的tickrate 越小越好
    OS_EXIT_CRITICAL();


    TaskStartCreateTasks();                                /* Create all the application tasks         */
    ArgumentSet(); //設定任務相關參數 例如:deadline,period
    OSTimeSet(0);
    for (i=0;;i++) {
        #ifdef debug
        //sprintf(debugMessage[debugMessTop++],"--- enter taskStart\n");
        #endif
        if(i!=0) printCtxSwMessage();

        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        //OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
        OSTimeDly (100);  // delay and wait (P-C) times  ?????
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

    for (i = 0; i < N_TASKS; i++) {                        /* Create N_TASKS identical tasks           */
        err=OSTaskCreate(Task, (void *)0, &TaskStk[i][TASK_STK_SIZE - 1],i+1 );
        if(err!=OS_NO_ERR){
            printf("create task %d errorno %d \n",i,err);
        } 
    }
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void Task()
{
    int start;  //the start time 
    int end;    //the end time
    int toDelay;
    int c = OSTCBCur->compTime;
    start=0;
        
    while(1)
    {
        while(OSTCBCur->compTime>0)  //C ticks
        {
        // do nothing
        }
        end=OSTimeGet() ; // end time
        toDelay=(OSTCBCur->period)-(end-start) ;
        if(end > start+OSTCBCur->period){
            //printf("!!! over deadline time:%d task:%d exceed deadline %d\n",end,OSTCBCur->OSTCBPrio,OSTCBCur->deadLine);
            sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d task %d exceed deadline\n",(int)OSTime, OSTCBCur->OSTCBPrio);
        }
        OS_ENTER_CRITICAL();
        start += (OSTCBCur->period) ;  // 下一次開始時間
        OSTCBCur->compTime = c;    // reset the counter (c ticks for computation)
        OSTCBCur->deadLine = start + OSTCBCur->period ;// reset the deadline
        OS_EXIT_CRITICAL();
        #ifdef debug
        //sprintf(&debugMessage[debugMessTop++],"!!!!!end task,time %d,prio:%d,deadLine %d\n",(int)OSTime,(int)OSTCBCur->OSTCBPrio,(int)OSTCBCur->deadLine);
        #endif
        OSTimeDly (toDelay);  // delay and wait (P-C) times
    }
}


void printCtxSwMessage(){
        static int i=0;
        for(;i<CtxSwMessageTop;i++){
            printf("%s",CtxSwMessage[i]);

            OSTimeDly(100);  //故意放慢print速度
            // CtxSwMessage[i]="";
            // sprintf(&CtxSwMessage[i], "tetetetetetetet\0");
            // if (err < 0)
            // {
            //     printf("print error \n");
            // }
        }

        // for(;k<CtxSwMessageTop;k++){
        //     for(;j<100;j++){
                
        //     }
        // }
        // // char temp[CtxSwMessageSize][100];
        // // CtxSwMessage = temp;
        // CtxSwMessageTop=0;



}
void ArgumentSet(void){  //設定任務參數
    OS_TCB* ptcb;
    ptcb = OSTCBList;
    while(ptcb->OSTCBPrio==1 || ptcb->OSTCBPrio==2 || ptcb->OSTCBPrio==3){
        //printf("Priority: %d set argument\n", ptcb->OSTCBPrio);
        if(ptcb->OSTCBPrio==1){
            ptcb->compTime = 1;
            ptcb->period = 4;
            ptcb->deadLine = 4;
        }
        else if(ptcb->OSTCBPrio==2){
            ptcb->compTime = 2;
            ptcb->period = 5;
            ptcb->deadLine = 5;
        }
        else if(ptcb->OSTCBPrio==3){
            ptcb->compTime = 2;
            ptcb->period = 10;
            ptcb->deadLine = 10;
        }
        ptcb = ptcb->OSTCBNext;
    }
}