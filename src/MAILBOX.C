/* implementation of mail boxes for 'real time kernel' */

#include "mailbox.h"
#include <dos.h>

PMAILBOX rtkCreateMailBox(UINT16 u16Slots, UINT16 u16DataSize)
{
  PMAILBOX psmbBox;
  psmbBox = (PMAILBOX) malloc(sizeof(MAILBOX));
  if (psmbBox != NULL) {
	psmbBox->psquMail = CreateQueue(u16Slots, u16DataSize);
	psmbBox->smPutMail = rtkCreateSema(Counting, u16Slots);
	psmbBox->smGetMail = rtkCreateSema(Counting, (UINT16) 0);
	if (psmbBox->psquMail  == NULL || psmbBox->smPutMail == NULL ||
	psmbBox->smGetMail == NULL) /* memory not granted */
	{
	  DeleteQueue(psmbBox->psquMail);   /* functions are protected for NULL pointers */
	  rtkDeleteSema(psmbBox->smPutMail);
	  rtkDeleteSema(psmbBox->smGetMail);
	  free(psmbBox);
	  psmbBox = NULL;
	}
  }
  return psmbBox;
}


VOID rtkPut(PMAILBOX psmbBox, VOID *pvData)
{
  WaitExt(psmbBox->smPutMail, BLOCKEDPUT);
  AppendQueue(psmbBox->psquMail, pvData);
  rtkSignal(psmbBox->smGetMail);
}

VOID rtkGet(PMAILBOX psmbBox, VOID *pvData)
{
  WaitExt(psmbBox->smGetMail, BLOCKEDGET);
  ServeQueue(psmbBox->psquMail, pvData);
  rtkSignal(psmbBox->smPutMail);
}

BOOL rtkPutTimed(PMAILBOX psmbBox, VOID *pvData, UINT32 u32TimeOut)
{
  BOOL bSucces;
  bSucces = WaitTimedExt(psmbBox->smPutMail, TIMEDPUT, u32TimeOut);
  if (bSucces) {
	AppendQueue(psmbBox->psquMail, pvData);
	rtkSignal(psmbBox->smGetMail);
  }
  return bSucces;
}


BOOL rtkGetTimed(PMAILBOX psmbBox, VOID *pvData, UINT32 u32TimeOut)
{
  BOOL bSucces;
  bSucces = WaitTimedExt(psmbBox->smGetMail, TIMEDGET, u32TimeOut);
  if (bSucces) {
	ServeQueue(psmbBox->psquMail, pvData);
	rtkSignal(psmbBox->smPutMail);
  }
  return bSucces;
}

