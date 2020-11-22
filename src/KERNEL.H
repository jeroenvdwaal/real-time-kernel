#ifndef KERNEL_H
#define KERNEL_H

/* forward declaration */
typedef struct tagTASK_TABLE *PTASK_TABLE;

#include "c_types.h"
#include "lists.h"
#include "mesgpass.h"
#include "semaphor.h"
#include "mailbox.h"

#define SCHEDULE_INTR 	0x60 //1C

#define EXIT_INTR       0x05 //05


#define READY            0
#define RUNNING          1
#define INITIALIZE       2
#define KILLED           3
#define DELAYING         4
#define BLOCKEDWAIT      5
#define TIMEDWAIT        6
#define BLOCKEDPUT       7
#define BLOCKEDGET       8
#define TIMEDPUT         9
#define TIMEDGET        10
#define BLOCKEDSEND     11
#define BLOCKEDRECEIVE  12
#define TIMEDSEND       13
#define TIMEDRECEIVE    14

/* number of states defined */
#define NUM_STATES      15


//extern BOOL bTimed[];
//extern BOOL bBlocked[];
#define IsTimed(STATUS)           (bTimed[(STATUS)])
#define IsBlocked(STATUS)         (bBlocked[(STATUS)])
#define IsSecondaryLinked(STATUS) (bSecondaryLinked[(STATUS)])

/* Task Table structure */
typedef struct tagTASK_TABLE
{
  NODE sndPrimaryLink;      /* linkage within Kernel base structure */
  NODE sndSecondaryLink;    /* linkage with other structures e.g. semaphores mailboxes... */
  UINT8 *pu8StackBase;      /* process stack */
  UINT8 *pu8StackTop;
  UINT8 *pu8StackReg;
  UINT16 u16State;
  UINT32 u32Timer;          /* Time to restart this task */
  MSGPASS smpMsgPassing;    /* Structure for message passing. */
  BOOL bTimedOut;           /* If a task is timed out this field is TRUE */
} TASK_TABLE, *PTASK_TABLE;


typedef struct
{
  LIST slsReadyList;             /* List of ready tasks */
  LIST slsTimedList;             /* Tasks waiting for a certain point in time */
  LIST slsWaitList;              /* Tasks waiting indefently */
  PTASK_TABLE psttRunningTask;   /* Current running task. */
  UINT32 u32Time;                /* for temporary use !! */
  void (*OldIsr1)();
  void (*OldIsr2)();
} KERNEL_BASE, *PKERNEL_BASE;

typedef PTASK_TABLE TaskHandle;




/* Functions */
void ForceTaskSwitch(void);
void SetVect(UINT8 u8VectNo, void (*IsrHandler)(void));
void *GetVect(UINT8 u8VectNo);
TaskHandle CreateTask(void (*TaskCode)(void), INT16 i16StackSize,
		      INT16 i16Priority, INT16 i16PID);
TaskHandle InitKernel(INT16 i16MainPriority, INT16 i16MainPID);
void rtkDelay(UINT32 u32Ticks);

extern KERNEL_BASE KernelBase;

#endif
