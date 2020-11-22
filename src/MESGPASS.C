/* message passing for the real time kernel */

#include "kernel.h"
#include "mesgpass.h"
#include <dos.h>





VOID rtkSend(PTASK_TABLE psttReceivingTask, VOID *pvData)
{
  PMSGPASS psmpReceiver = &psttReceivingTask->smpMsgPassing;
  disable();
  if (EmptyList(&psmpReceiver->lslBlockedSend) &&
      (psttReceivingTask->u16State == STATE_BLOCKEDRECEIVE ||
       psttReceivingTask->u16State == STATE_TIMEDRECEIVE)) {
    /* receiver task is suspended for communications */
    memcpy(psmpReceiver->pvRecvData, pvData, (size_t) psmpReceiver->u16DataLen);
    /* resume receiving task */
    ResumeTask(psttReceivingTask);
  }
  else {
    /* store communiation param. */
    KernelBase.psttRunningTask->smpMsgPassing.pvSendData = pvData;
    /* receiver is not ready to communicate. */
    SuspendTask(&psmpReceiver->lslBlockedSend, STATE_BLOCKEDSEND);
  }
  enable();
}

BOOL rtkSendCond(PTASK_TABLE psttReceivingTask, VOID *pvData)
{
  PMSGPASS psmpReceiver = &psttReceivingTask->smpMsgPassing;
  BOOL bSucces = FALSE;
  disable();
  if ( EmptyList(&psmpReceiver->lslBlockedSend) &&
      (psttReceivingTask->u16State == STATE_BLOCKEDRECEIVE ||
       psttReceivingTask->u16State == STATE_TIMEDRECEIVE)) {
    /* receiver task is suspended for communications */
    memcpy(psmpReceiver->pvRecvData, pvData, (size_t) psmpReceiver->u16DataLen);
    /* resume receiving task */
    ResumeTask(psttReceivingTask);
    bSucces = TRUE;
  }
  enable();
  return bSucces;
}

BOOL rtkSendTimed(PTASK_TABLE psttReceivingTask, VOID *pvData, UINT32 u32TimeOut)
{
  BOOL bSucces = TRUE;
  PMSGPASS psmpReceiver = &psttReceivingTask->smpMsgPassing;
  disable();
  if (EmptyList(&psmpReceiver->lslBlockedSend) &&
      (psttReceivingTask->u16State == STATE_BLOCKEDRECEIVE ||
       psttReceivingTask->u16State == STATE_TIMEDRECEIVE)) {
    /* receiver task is suspended for communications */
    memcpy(psmpReceiver->pvRecvData, pvData, (size_t) psmpReceiver->u16DataLen);
    /* resume receiving task */
    ResumeTask(psttReceivingTask);
  }
  else {
    /* store communiation param. */
    KernelBase.psttRunningTask->smpMsgPassing.pvSendData = pvData;
    /* receiver is not ready to communicate. */
    bSucces = !SuspendTaskTimed(&psmpReceiver->lslBlockedSend, STATE_TIMEDSEND, u32TimeOut);
  }
  enable();
  return bSucces;
}


VOID rtkReceive(VOID *pvData, UINT16 u16DataLen)
{
  PMSGPASS psmpMsg;
  PTASK_TABLE psttThisTask, psttSendingTask;
  PNODE psndSecondaryLink;

  disable();
  psttThisTask = KernelBase.psttRunningTask;
  psmpMsg = &psttThisTask->smpMsgPassing;

  if (!EmptyList(&psmpMsg->lslBlockedSend)) {
    /* there is already a task waiting to communicate */
    psndSecondaryLink = GetFirstNode(&psmpMsg->lslBlockedSend); /* pump over data */
    psttSendingTask = (PTASK_TABLE) (psndSecondaryLink - 1);
    memcpy(pvData, psttSendingTask->smpMsgPassing.pvSendData ,(size_t) u16DataLen);
    ResumeTaskInList(&psmpMsg->lslBlockedSend);
  }
  else {
    /* no task to communicate with */
    psmpMsg->pvRecvData = pvData;     /* leave communcation params */
    psmpMsg->u16DataLen = u16DataLen;
    psttThisTask->u16State = STATE_BLOCKEDRECEIVE;
    ForceTaskSwitch();
  }
  enable();
}

BOOL rtkReceiveCond(VOID *pvData, UINT16 u16DataLen)
{
  PMSGPASS psmpMsg;
  PTASK_TABLE psttThisTask, psttSendingTask;
  PNODE psndSecondaryLink;
  BOOL bSucces = FALSE;

  disable();
  psttThisTask = KernelBase.psttRunningTask;
  psmpMsg = &psttThisTask->smpMsgPassing;

  if (!EmptyList(&psmpMsg->lslBlockedSend)) {
    /* there is already a task waiting to communicate */
    psndSecondaryLink = GetFirstNode(&psmpMsg->lslBlockedSend); /* pump over data */
    psttSendingTask = (PTASK_TABLE) (psndSecondaryLink - 1);
    memcpy(pvData, psttSendingTask->smpMsgPassing.pvSendData ,(size_t) u16DataLen);
    ResumeTaskInList(&psmpMsg->lslBlockedSend);
    bSucces = TRUE;
  }
  enable();
  return bSucces;
}


BOOL rtkReceiveTimed(VOID *pvData, UINT16 u16DataLen, UINT32 u32TimeOut)
{
  PMSGPASS psmpMsg;
  PTASK_TABLE psttThisTask, psttSendingTask;
  PNODE psndSecondaryLink;
  BOOL bSucces = TRUE;

  disable();
  psttThisTask = KernelBase.psttRunningTask;
  psmpMsg = &psttThisTask->smpMsgPassing;

  if (!EmptyList(&psmpMsg->lslBlockedSend)) {
    /* there is already a task waiting to communicate */
    psndSecondaryLink = GetFirstNode(&psmpMsg->lslBlockedSend); /* pump over data */
    psttSendingTask = (PTASK_TABLE) (psndSecondaryLink - 1);
    memcpy(pvData, psttSendingTask->smpMsgPassing.pvSendData ,(size_t) u16DataLen);
    ResumeTaskInList(&psmpMsg->lslBlockedSend);
  }
  else {
    /* no task to communicate with */
    psmpMsg->pvRecvData = pvData;     /* leave communcation params */
    psmpMsg->u16DataLen = u16DataLen;
    psttThisTask->u16State = STATE_TIMEDRECEIVE;
    psttThisTask->u32Timer = KernelBase.u32Time + u32TimeOut;
    ForceTaskSwitch();
    bSucces = !psttThisTask->bTimedOut;
  }
  enable();
  return bSucces;
}