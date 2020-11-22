/* implementation of semaphores for real time kernel */

#include <stdlib.h>
#include <dos.h>
#include "kernel.h"
#include "Semaphor.h"



PSEMAPHORE rtkCreateSema(INT16 i16SemaType, UINT16 u16InitialValue)
{
  PSEMAPHORE pssmTemp;
  if ( (pssmTemp = (PSEMAPHORE)malloc(sizeof(SEMAPHORE))) != NULL) {
    pssmTemp->u16Value = u16InitialValue;
    pssmTemp->i16SemaType = i16SemaType;
    InitList(&pssmTemp->slsWaitTaskLinks);
  }
  return pssmTemp;
}


VOID rtkDeleteSema(PSEMAPHORE pssmS)
{
  /* check if there are waiting tasks in list !!! */
  if(pssmS != NULL) {
    free(pssmS);
  }
}


UINT16 rtkSemaValue(PSEMAPHORE pssmS)
{
  return pssmS->u16Value;
}


VOID rtkSignal(PSEMAPHORE pssmS)
{
  PTASK_TABLE psttTask;
  PNODE psndSecondaryLink;
  PLIST pslsWaitTaskLinks = &pssmS->slsWaitTaskLinks;
  /* Are there task waiting for this signal ? */
  disable();
  if (!EmptyList(pslsWaitTaskLinks)) {

	/* awake first task in this list */
	psndSecondaryLink = GetFirstNode(pslsWaitTaskLinks);

	/* remove secondary link */
	RemoveNode(psndSecondaryLink);

	/* determine from SecondaryLink address the Tasktable address */
	psttTask = (PTASK_TABLE) (psndSecondaryLink - 1);

	/* move task to ready list.*/
	RemoveNode((PNODE)psttTask);
	EnqueueNode(&KernelBase.slsReadyList, (PNODE)psttTask);
	psttTask->u16State = READY;

	/* if task becomes the new one schedule */
	enable();
	/* test if this task becomes new one */
	/* this can be done by examine the priority of the current
	   running task and this task */
	ForceTaskSwitch();
  }
  else {			 /* no one to wake, so leave a signal. */
	pssmS->u16Value++;
	enable();
  }
}


/* WaitExt function tries to execute a down operation on semaphore S */
/* if the down operation failed, the calling task's new state is
   assigned via param 'NewState' */
VOID WaitExt(PSEMAPHORE pssmS, UINT16 u16NewState)
{
  PTASK_TABLE psttTask;

  disable();
  if ( pssmS->u16Value > 0){  /* test if there is a signal. */
	pssmS->u16Value--;
	enable();
  }
  else {
	psttTask = KernelBase.psttRunningTask;
	psttTask->u16State = u16NewState;
	AddTailNode(&pssmS->slsWaitTaskLinks, &psttTask->sndSecondaryLink);
	enable();
	ForceTaskSwitch();
  }
}


BOOL rtkWaitCond(PSEMAPHORE pssmS)
{
  BOOL bSucces = FALSE;
  disable();
  if ( pssmS->u16Value > 0){  /* test if there is a signal. */
	pssmS->u16Value--;
	bSucces = TRUE;
  }
  enable();
  return bSucces;
}


BOOL WaitTimedExt(PSEMAPHORE pssmS, UINT16 u16NewState, UINT32 u32TimeOut)
{
  /* for now only binary and counter sem. */
  PTASK_TABLE psttTask;
  disable();
  if ( pssmS->u16Value > 0){
	pssmS->u16Value--;
	enable();
	return TRUE;  /* operation succesfull return 'succes' */
  }
  else {          /* task is not able to continu. */
	psttTask = KernelBase.psttRunningTask;
	psttTask->u16State = u16NewState;
	psttTask->u32Timer = KernelBase.u32Time + u32TimeOut;

	/* set up secondary link */
	AddTailNode(&pssmS->slsWaitTaskLinks, &psttTask->sndSecondaryLink);
	enable();
	ForceTaskSwitch();
	return !psttTask->bTimedOut;  /* return succes */
 }
}


