#ifndef TIMER_H
#define TIMER_H

#include <dos.h>
#include "c_types.h"

/****************************************/
/* I/O-addresses for accesing the PIT 1 */
/****************************************/

#define PIT1_CNT0	0x40	/* I/O address counter register 0 of PIT 1 */
#define PIT1_CNT1	0x41    /* I/O address counter register 1 of PIT 1 */
#define PIT1_CNT2	0x42    /* I/O address counter register 2 of PIT 1 */
#define PIT1_CTRL	0x43    /* I/O address control register of PIT 1 */


/*********************************************/
/* Defines for initializing control register */
/*********************************************/

/* Count operation */
/*------------------------------*/

/* Select counter bits */
#define CNT0		0x00	/* Selects counter 0 */
#define CNT1		0x40    /* Selects counter 1 */
#define CNT2		0x80	/* Selects counter 2 */
/* Define read/write mode */
#define LATCH		0x00	/* Latches counter */
#define RWLB		0x10	/* Read/write low counter byte */
#define RWHB		0x20    /* Read/write high counter byte */
#define RWLBHB		0x30    /* Read/write low and high counter byte */
/* Define count mode of PIT */
#define MODE0		0x00	/* Generate interrupts */
#define MODE1		0x02	/* Programmable monoflipflop */
#define MODE2		0x04	/* Clock generator */
#define MODE3		0x06	/* Squarewave generator */
#define MODE4		0x08	/* Trigger output via software */
#define MODE5		0x0a	/* Trigger output via hardware */

/* Readback operation */
/*-------------------------------*/

/* Select counter */
#define RB_CNT0		0xa2	/* Select counter 0 for readback */
#define RB_CNT1		0xa4    /* Select counter 1 for readback */
#define RB_CNT2		0xa8    /* Select counter 2 for readback */
/* What should be read back? */
#define RB_CNTR		0x10	/* Current value of counter */
#define RB_STAT		0x20	/* Status byte */
#define RB_NOTH		0x30	/* Nothing */


/*****************************/
/* Timer interrupt vector(s) */
/*****************************/

#define TIMER_INT_VEC	0x08

#define SUBTICK_TICK	1193.0
#define SUBTICK_SEC	1193180.0     	     /* Number subticks per second */
#define TICK_SEC	(SUBTICK_SEC / SUBTICK_TICK)
#define SEC_TICK	(1.0 / TICK_SEC)


typedef struct tagTime
{                      		/* Registration of time in: */
  UINT32 u32Tick;		/*  - Ticks */
} TIME, *PTIME;			/* Prefix: stm, pstm        */


void TimeClear(PTIME pstmTime);
void TimeGet(PTIME pstmTime);
void TimeConvTo(PTIME pstmConvTo, UINT32 u32Sec, UINT32 u32USec);
void TimeConvBack(PTIME pstmConvBack, UINT32 *pu32Sec, UINT32 *pu32USec);
void TimeAdd(PTIME pstmAccu, PTIME pstmAdd);
BOOL TimeComp(PTIME pstmTime1, PTIME pstmTime2);
void TimeSetAlarm(PTIME pstmAlarm);
void InitTimer(void interrupt (*SchedFunc)(void));
void RemoveTimer(void);



#endif
