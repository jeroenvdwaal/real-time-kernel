#ifndef MAILBOX_H
#define MAILBOX_H

#include "kernel.h"
#include "queues.h"

typedef struct tagMAILBOX
{
  PQUEUE psquMail;        /* Abstract data type 'Queue' */
  Semaphore smPutMail;    /* Tasks not able to rtkPut data */
  Semaphore smGetMail;    /* Tasks waiting on a rtkGet */
} MAILBOX, *PMAILBOX, *MailBox;


/* user functions */
PMAILBOX rtkCreateMailBox(UINT16 u16Slots, UINT16 u16DataSize);
VOID rtkPut(PMAILBOX psmbBox, VOID *pvData);
BOOL rtkPutTimed(PMAILBOX psmbBox, VOID *pvData, UINT32 u32TimeOut);
VOID rtkGet(PMAILBOX psmbBox, VOID *pvData);
BOOL rtkGetTimed(PMAILBOX mbBox, VOID *pvData, UINT32 u32TimeOut);

#endif
