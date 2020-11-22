/* enquiry functions for the rt kernel */


#include "kernel.h"
#include <string.h>

#include <stdio.h>

UINT8 *pu8TaskStates[] = {
  "ready",
  "running",
  "initialize",
  "killed",
  "delayed",
  "blocked wait",
  "timed wait",
  "blocked put",
  "blocked get",
  "timed put",
  "timed get",
  "blocked send",
  "blocked receive",
  "timed send",
  "timed receive"
};

void GetTaskInfo(PTASK_TABLE psttThisTask, UINT8 *pu8Buffer)
{
  UINT16 u16CharCount;
  sprintf(pu8Buffer,"%3d  %3d  %18s\n\r",
	  psttThisTask->sndPrimaryLink.i16ID,
	  psttThisTask->sndPrimaryLink.i16Priority,
	  pu8TaskStates[psttThisTask->u16State]);

}

void rtkListTasks(UINT8 *pu8Buffer, UINT16 u16BufferLen)
{
  PNODE psndNode[3];
  UINT8 pu8TempBuffer[80];
  UINT16 u16CharCount = 0, u16StringLen, u16ListNo;
  BOOL bToBig = FALSE;


  /* there are three lists in the kernel base (ready, waiting and timed) */
  psndNode[0] = GetFirstNode(&KernelBase.slsReadyList);
  psndNode[1] = GetFirstNode(&KernelBase.slsWaitList);
  psndNode[2] = GetFirstNode(&KernelBase.slsTimedList);
  if (u16BufferLen > 0) {
    strcpy(pu8Buffer,"\n");
    for (u16ListNo = 0; u16ListNo < 3; u16ListNo++) {
      while(!TailNode(psndNode[u16ListNo]) && !bToBig) {
	GetTaskInfo((PTASK_TABLE)psndNode[u16ListNo], pu8TempBuffer);
	u16StringLen = strlen(pu8TempBuffer);
	if ( (u16CharCount + u16StringLen) < u16BufferLen) {
	  strcat(pu8Buffer, pu8TempBuffer);
	}
	else {
	  bToBig = TRUE;
	}
	psndNode[u16ListNo] = NextNode(psndNode[u16ListNo]);
      }
    }
  }
}

