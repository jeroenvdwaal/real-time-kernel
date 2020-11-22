
#include "queues.h"
/* abstract data type circular queue */


/*  Attention !!!! This module does some NON reentrant DOS calls.  */


PQUEUE CreateQueue(UINT16 u16MaxItems, UINT16 u16DataLen)
{
  PQUEUE psquCreate;
  psquCreate = (PQUEUE) malloc(sizeof(QUEUE));
  if (psquCreate != NULL) {
    psquCreate->pu8BufferBase= calloc((size_t)u16MaxItems, (size_t)u16DataLen);
    if (psquCreate->pu8BufferBase == NULL) {
      free(psquCreate);
      psquCreate = NULL;
    }
    else {
      psquCreate->u16Count = 0;
      psquCreate->u16MaxItems = u16MaxItems;
      psquCreate->u16DataLen  = u16DataLen;
      psquCreate->pu8BufferEnd = psquCreate->pu8BufferBase + (u16MaxItems * u16DataLen);
      psquCreate->pu8PutPtr    = psquCreate->pu8BufferBase;
      psquCreate->pu8GetPtr    = psquCreate->pu8BufferBase;
    }
  }
  return psquCreate;
}

void DeleteQueue(PQUEUE psquQueue)
{
  if(psquQueue != NULL) {
    if (psquQueue->pu8BufferBase != NULL) {
      free(psquQueue->pu8BufferBase);
    }
    free(psquQueue);
  }
}


void AppendQueue(PQUEUE psquQueue, VOID *pvItem)
{
  if (psquQueue->u16Count < psquQueue->u16MaxItems)
  {                         /* buffer is not full */
    psquQueue->u16Count++;
    /* copy Item to buffer */
    memcpy(psquQueue->pu8PutPtr, pvItem, (size_t)psquQueue->u16DataLen);

    /* Update Put pointer. */
    psquQueue->pu8PutPtr += psquQueue->u16DataLen;       /* advance to next position */
    if (psquQueue->pu8PutPtr >= psquQueue->pu8BufferEnd){ /* wrap around in buffer */
      psquQueue->pu8PutPtr = psquQueue->pu8BufferBase;
    }
  }
  // else printf("\nTrying to put in full buffer.");
}


void ServeQueue(PQUEUE psquQueue, VOID *pvItem)
{
  if (psquQueue->u16Count > 0)
  {                     /* Queue contains items */
    psquQueue->u16Count--;

    /* copy buffer to item */
    memcpy( pvItem, psquQueue->pu8GetPtr, (size_t)psquQueue->u16DataLen);

    /* Update Get pointer */
    psquQueue->pu8GetPtr += psquQueue->u16DataLen;       /* advance to next position */
    if (psquQueue->pu8GetPtr >= psquQueue->pu8BufferEnd){ /* wrap around in buffer */
      psquQueue->pu8GetPtr = psquQueue->pu8BufferBase;
    }
  }
  //else printf("\nTrying to get from an empty buffer.");
}


BOOL QueueFull(PQUEUE psquQueue)
{
  return (psquQueue->u16Count >= (psquQueue->u16MaxItems));
}

BOOL QueueEmpty(PQUEUE psquQueue)
{
  return (psquQueue->u16Count == 0);
}


UINT16 QueueCount(PQUEUE psquQueue)
{
  return psquQueue->u16Count;
}

