/* message passing for the real time kernel */
#ifndef MESGPASS_H
#define MESGPASS_H

#include "kernel.h"
#include "semaphore.h"

typedef struct tagMSGPASS
{
  VOID *pvSendData;       /* pointer to send buffer */
  PSEMAPHORE smSender;    /* Sender task ready for transmitting data */
  PSEMAPHORE smDone;      /* transport of data ready */
} MSGPASS, *PMSGPASS;

VOID rtkSend(PTASK_TABLE psttReceivingTask, VOID *pvData);
BOOL rtkSendCond(PTASK_TABLE psttReceivingTask, VOID *pvData);
BOOL rtkSendTimed(PTASK_TABLE psttReceivingTask, VOID *pvData, UINT32 u32TimeOut);

VOID rtkReceive(VOID *pvData, UINT16 u16DataLen);
BOOL rtkReceiveCond(VOID *pvData, UINT16 u16DataLen);
BOOL rtkReceiveTimed(VOID *pvData, UINT16 u16DataLen, UINT32 u32TimeOut);

#endif