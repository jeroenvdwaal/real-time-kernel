#ifndef __HUGE__
#error Compile this program uses HUGE model
#endif


/*
november 1995
RtKernel, een Real Time Kernel welke vanuit DOS gestart wordt.
*/

#include <stdlib.h>
#include <stdio.h>
#include <dos.h>
#include "c_types.h"
#include "kernel.h"


// TEMP
#include <conio.h>

#define MIN_STACKSIZE 1024



KERNEL_BASE KernelBase;

//extern void interrupt TaskSwitch(void);

void TaskIdle(void);
void Scheduler(void);
void TaskSwitch(void);
void ExitTaskHandler(void);
void ExitTask(void);


/* Classifying different states in blocked and timed */

/* classifying van states via lookup's */

BOOL bTimed[NUM_STATES] = {
FALSE,  /* 0  READY          */
FALSE,  /* 1  RUNNING        */
FALSE,  /* 2  INITIALIZE     */
FALSE,  /* 3  KILLED         */
TRUE,   /* 4  DELAYING       */
FALSE,  /* 5  BLOCKEDWAIT    */
TRUE,   /* 6  TIMEDWAIT      */
FALSE,  /* 7  BLOCKEDPUT     */
FALSE,  /* 8  BLOCKEDGET     */
TRUE,   /* 9  TIMEDPUT       */
TRUE,   /* 10 TIMEDGET       */
FALSE,  /* 11 BLOCKEDSEND    */
FALSE,  /* 12 BLOCKEDRECEIVE */
TRUE,   /* 13 TIMEDSEND      */
TRUE    /* 14 TIMEDRECEIVE   */
};

BOOL bBlocked[NUM_STATES] = {
FALSE,  /* 0  READY          */
FALSE,  /* 1  RUNNING        */
FALSE,  /* 2  INITIALIZE     */
FALSE,  /* 3  KILLED         */
FALSE,  /* 4  DELAYING       */
TRUE,   /* 5  BLOCKEDWAIT    */
FALSE,  /* 6  TIMEDWAIT      */
TRUE,   /* 7  BLOCKEDPUT     */
TRUE,   /* 8  BLOCKEDGET     */
FALSE,  /* 9  TIMEDPUT       */
FALSE,  /* 10 TIMEDGET       */
TRUE,   /* 11 BLOCKEDSEND    */
TRUE,   /* 12 BLOCKEDRECEIVE */
FALSE,  /* 13 TIMEDSEND      */
FALSE   /* 14 TIMEDRECEIVE   */
};

BOOL bSecondaryLinked[NUM_STATES] = {
FALSE,  /* 0  READY          */
FALSE,  /* 1  RUNNING        */
FALSE,  /* 2  INITIALIZE     */
FALSE,  /* 3  KILLED         */
FALSE,  /* 4  DELAYING       */
TRUE,   /* 5  BLOCKEDWAIT    */
TRUE,   /* 6  TIMEDWAIT      */
TRUE,   /* 7  BLOCKEDPUT     */
TRUE,   /* 8  BLOCKEDGET     */
TRUE,   /* 9  TIMEDPUT       */
TRUE,   /* 10 TIMEDGET       */
TRUE,   /* 11 BLOCKEDSEND    */
TRUE,   /* 12 BLOCKEDRECEIVE */
TRUE,   /* 13 TIMEDSEND      */
TRUE    /* 14 TIMEDRECEIVE   */
};


void ForceTaskSwitch(void)
{
  asm int 0x60
}

void SetVect(UINT8 u8VectNo, void (*IsrHandler)(void))
{
  UINT16 *pu16TablePtr = MK_FP(0x0000, ((INT16) u8VectNo) << 2);
  *(pu16TablePtr+1) = FP_SEG(IsrHandler);
  *pu16TablePtr = FP_OFF(IsrHandler);
}


void *GetVect(UINT8 u8VectNo)
{
  UINT16 *pu16TablePtr = MK_FP(0x0000, ((INT16) u8VectNo) << 2);
  return MK_FP(*(pu16TablePtr+1), *pu16TablePtr);
}


TaskHandle CreateTask(void (*TaskCode)(void), INT16 i16StackSize,
		      INT16 i16Priority, INT16 i16PID)
{
  PTASK_TABLE psttTmp;
  PNODE psndTmp;
  UINT16 *pu16StackReg;

  /* allocate new task table */
  disable();  /* malloc is not reentrant! */
  psttTmp = (PTASK_TABLE) malloc(sizeof(TASK_TABLE));
  if (psttTmp == NULL) {
    return NULL;
  }
  i16StackSize = max(MIN_STACKSIZE, i16StackSize);

  /* alloc stack space */
  psttTmp->pu8StackBase = (UINT8 *) calloc(i16StackSize, sizeof(UINT8));
  enable();

  if (psttTmp->pu8StackBase != NULL) {    /* memory granted */
    /* The stack expands from top to bottom, so we need it's top
       address. */
    psttTmp->pu8StackTop = psttTmp->pu8StackBase + sizeof(UINT8) * (i16StackSize);
    psttTmp->pu8StackReg = psttTmp->pu8StackTop;

    psndTmp = &psttTmp->sndPrimaryLink;
    psndTmp->i16Priority = i16Priority;
    psndTmp->i16Type = EXEC_TYPE;
    psndTmp->i16ID = i16PID;

    psttTmp->u16State = INITIALIZE;

    /* Store flags, cs and ip on stack for returing with a iret instruction */
    pu16StackReg = (UINT16 *)psttTmp->pu8StackReg;
    *--pu16StackReg = FP_SEG(ExitTask);
    *--pu16StackReg = FP_OFF(ExitTask);
    *--pu16StackReg = 0x3246;   /* initial flags */
    *--pu16StackReg = FP_SEG(TaskCode);
    *--pu16StackReg = FP_OFF(TaskCode);

    psttTmp->pu8StackReg = (UINT8 *) pu16StackReg;

    /* initialize time field */
    psttTmp->u32Timer = 0L;

    /* reset time out flag */
    psttTmp->bTimedOut = FALSE;

    /* Initialize struct for direct interprocess comm.*/
    //InitList(&psttTmp->smpMsgPassing.lslBlockedSend);

    /* Link task in 'ready' list. NOTE: interrupts disabled during this operation*/
    disable();
    EnqueueNode(&KernelBase.slsReadyList, (PNODE)psttTmp);
    enable();
    return psttTmp;
  }
  else
  {
    free(psttTmp->pu8StackBase);
    free(psttTmp);
    return NULL;
  }
}

void ExitKernelIsr(void)
{
  exit(1);
}

void ExitKernel(void)
{
  /* restore interrupt vectors */
  SetVect(SCHEDULE_INTR, KernelBase.OldIsr1);
  SetVect(EXIT_INTR, KernelBase.OldIsr2);
  /* reset interrupt vectors */
  printf("\nGracefull exit.\n");
}


TaskHandle InitKernel(INT16 i16MainPriority, INT16 i16MainPID)
{
  PTASK_TABLE psttTaskTable;
  PNODE psndNode;

  /* Initialize list structures */
  InitList(&KernelBase.slsReadyList);
  InitList(&KernelBase.slsTimedList);
  InitList(&KernelBase.slsWaitList);

  /* allocate new task table */
  KernelBase.psttRunningTask = (PTASK_TABLE) malloc(sizeof(TASK_TABLE));
  if (KernelBase.psttRunningTask == NULL) {
    return NULL;
  }

  /* init task table for MainTask */
  psttTaskTable = KernelBase.psttRunningTask;
  psttTaskTable->pu8StackBase = NULL;
  psttTaskTable->pu8StackTop  = NULL;
  psttTaskTable->pu8StackReg  = NULL;
  psttTaskTable->u16State = RUNNING;
  psndNode = &psttTaskTable->sndPrimaryLink;
  psndNode->i16Priority = i16MainPriority;
  psndNode->i16Type = EXEC_TYPE;
  psndNode->i16ID = i16MainPID;

  /* initialize time field */
  psttTaskTable->u32Timer = 0L;

  /* reset time out flag */
  psttTaskTable->bTimedOut = FALSE;

  /* Initialize struct for direct interprocess comm.*/
  //InitList(&psttTaskTable->smpMsgPassing.lslBlockedSend);

  /* Insert main task in running task list */
  EnqueueNode(&KernelBase.slsReadyList, (PNODE) psttTaskTable);

  /* create idle task with lowest priority */
  CreateTask(TaskIdle, 1024, 0, 0);


  /* register exit function */
  atexit(ExitKernel);

  /* Clear time field */
  KernelBase.u32Time = 0;

  /* interrupts */
  KernelBase.OldIsr1 = GetVect(SCHEDULE_INTR);/* save present isr handler */
  KernelBase.OldIsr2 = GetVect(EXIT_INTR);    /* save present isr handler */
  disable();                                  /* disable interrupts for a while.. */
  SetVect(SCHEDULE_INTR, TaskSwitch);	      /* Set Schedule interrupt */
  SetVect(EXIT_INTR, ExitKernelIsr);	      /* Set Exit interrupt */
  enable();                                   /* we're multitasking !! */
  return psttTaskTable;
}


void ExitTask(void)
{
  unsigned char *screen = MK_FP(0xb800, 0x200);
  /* check if resources are deallocated */

  /* set task status REMOVE */
  //printf("Task ID .. Ended");
  *screen = 'X';
  KernelBase.psttRunningTask->u16State = KILLED;
  /* activate scheduler via int instruction */
  ForceTaskSwitch();
}



void Schedule(void)
{
  PTASK_TABLE psttCurrent, psttNext, psttTimed;
  PNODE psndTimedWaitInsert;
  PNODE psndTimed;
  UINT16 *pu16CurrentState;

  /* advance kernel time */
  KernelBase.u32Time++;    /* This is not!! the right place for advancing the
				  the kernel time. Hence, time should only be updated
				  if the IRQ is caused by the timer hardware.
				  But sometimes a rescheduling is achieved by
				  software interrupting. If this is the case a
				  the kernel timer must NOT be updated.
			   */


  /* TIME OUT */
  /* If necessary wake up tasks which are timed out. */
  psttTimed = (PTASK_TABLE) GetFirstNode(&KernelBase.slsTimedList);
  while(!TailNode((PNODE)psttTimed))
  {
	if(psttTimed->u32Timer <= KernelBase.u32Time) {
	  RemoveNode((PNODE)psttTimed);
	  EnqueueNode(&KernelBase.slsReadyList, (PNODE)psttTimed);

	  /* if this task becomes ready by a time out, remove the
	 task table's secondary link.
	  */
	  if (IsSecondaryLinked(psttTimed->u16State)) {
		 RemoveNode(&psttTimed->sndSecondaryLink);
		 psttTimed->bTimedOut = TRUE;
	  }
	  psttTimed->u16State = READY;
	  psttTimed = (PTASK_TABLE) GetFirstNode(&KernelBase.slsTimedList);
	}
	else break;
  }

  /* prepare for round robin scheduling. */
  psttCurrent = KernelBase.psttRunningTask;
  pu16CurrentState = &(psttCurrent->u16State);
  psttNext = (PTASK_TABLE) NextNode((PNODE)psttCurrent);

  /* A current running task becomes ready */
  if (*pu16CurrentState == RUNNING) {
	*pu16CurrentState = READY;
  }
  else if (IsTimed(*pu16CurrentState)) /* current task is halted because of timed blocking */
  {
	/* reset time out flag */
	psttTimed->bTimedOut = FALSE;

	/* search for a place to insert the task table */
	psndTimedWaitInsert = GetFirstNode(&KernelBase.slsTimedList);
	while(!TailNode(psndTimedWaitInsert)) {
	  if (((PTASK_TABLE)psndTimedWaitInsert)->u32Timer > psttCurrent->u32Timer) {
		 break;
	   }
	   psndTimedWaitInsert = NextNode(psndTimedWaitInsert);
	}
	RemoveNode((PNODE)psttCurrent);
	InsertNode(&KernelBase.slsTimedList, psndTimedWaitInsert->psndPred, (PNODE) psttCurrent);
  }
  else if (IsBlocked(*pu16CurrentState))
  {
	RemoveNode((PNODE)psttCurrent);
	AddTailNode(&KernelBase.slsWaitList, (PNODE)psttCurrent);
  }
  else if(*pu16CurrentState == KILLED)
  {
	RemoveNode((PNODE) psttCurrent);
	free(psttCurrent->pu8StackBase);  /* PAS OP: free() = DOS call en is niet reentrant! */
	free(psttCurrent);
  }

  /* Round robin scheduling */
  if (TailNode((PNODE)psttNext)) {
	psttNext = (PTASK_TABLE) GetFirstNode(&KernelBase.slsReadyList);
  }
  KernelBase.psttRunningTask = psttNext;
}


void TaskIdle(void)
{
  int i;
  unsigned char *screen = MK_FP(0xb800,0x8);
  while (1)
  {
    i++;
    *screen = i%30 + 'a';
    ForceTaskSwitch();
  }
}


void rtkDelay(UINT32 u32Ticks)
{
  disable();
  KernelBase.psttRunningTask->u32Timer = KernelBase.u32Time + u32Ticks;
  KernelBase.psttRunningTask->u16State = DELAYING;
  enable();
  ForceTaskSwitch();
}



