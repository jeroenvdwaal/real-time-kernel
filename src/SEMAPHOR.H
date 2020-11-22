#ifndef SEMAPHORE_H
#define SEMAPHORE_H


enum SemaType {Counting, Binary, Resource};

/*
  structure SEMAPHORE contains all the elements for implementation
  of semaphores for the rtk. Value u16Semaphore is value of the semaphore.
  Three different types of semaphores are possible:
	- binary
	- counting
	- resources
  The type of semaphore is held in the u16Type field.
  Both following fields are for system administration. When a task tries
  to change the semaphore and is not allowed to, the task is suspended for
  a while. Suspended tasks are removed from the system's ready list and
  inserted in one of the two listst in semaphore structure.
  Tasks which are suspended because of rtkWait() will be moved to the
  slsWaitTasks list.
*/


typedef struct tagSEMAPHORE
{
  UINT16 u16Value;          /* semaphore value */
  INT16  i16SemaType;       /* type of semaphore */
  LIST   slsWaitTaskLinks;  /* Links to suspended tasks caused by rtkWait */
} SEMAPHORE, *PSEMAPHORE, *Semaphore;


/* user functions */
PSEMAPHORE rtkCreateSema(INT16 i16SemaType, UINT16 u16InitialValue);
VOID   rtkDeleteSema(PSEMAPHORE pssmS);
UINT16 rtkSemaValue(PSEMAPHORE pssmS);
BOOL   rtkWaitCond(PSEMAPHORE pssmS);
VOID   rtkSignal(PSEMAPHORE pssmS);

/* private kernel functions */
VOID   WaitExt(PSEMAPHORE pssmS, UINT16 u16NewState);
BOOL   WaitTimedExt(PSEMAPHORE pssmS, UINT16 u16NewState, UINT32 u32TimeOut);

/* defines */
#define rtkWait(pssmS) WaitExt((pssmS), (BLOCKEDWAIT))
#define rtkWaitTimed(pssmS, u32TimeOut) WaitTimedExt((pssmS), (TIMEDWAIT), (u32TimeOut))


#endif