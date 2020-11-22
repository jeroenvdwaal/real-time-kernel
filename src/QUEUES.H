#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <mem.h>
#include "c_types.h"

typedef struct tagQUEUE
{
  UINT16 u16Count;     /* no. items in buffer */
  UINT16 u16MaxItems;  /* Max number of items in this queue. */
  UINT16 u16DataLen;   /* Length of one item. */
  UINT8 *pu8PutPtr;     /* Data put pointer */
  UINT8 *pu8GetPtr;     /* Data get pointer */
  UINT8 *pu8BufferBase; /* Pointer to begin of buffer. */
  UINT8 *pu8BufferEnd;  /* Pointer to the end of the buffer. */
} QUEUE, *PQUEUE;


/* User functions */

PQUEUE CreateQueue(UINT16 u16MaxItems, UINT16 u16DataLen);
VOID DeleteQueue(PQUEUE psquQueue);
VOID AppendQueue(PQUEUE psquQueue, VOID *pvItem);
VOID ServeQueue(PQUEUE psquQueue, VOID *pvItem);
BOOL QueueFull(PQUEUE psquQueue);
BOOL QueueEmpty(PQUEUE psquQueue);
UINT16 QueueCount(PQUEUE psquQueue);

#endif