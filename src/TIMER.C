#include <stdio.h>
#include "timer.h"



TIME stmAbsTime;
TIME stmAlarm;
BOOL bAlarmSet;


void interrupt (*OldTimerIsr)(void);
void interrupt (*Scheduler)(void);





void SetPITCounter(UINT16 u16SubTick)
{
  outportb(PIT1_CTRL, CNT0 | MODE2 | RWLBHB);
  outportb(PIT1_CNT0, u16SubTick & 0xff);
  outportb(PIT1_CNT0, u16SubTick >> 8);
}


void interrupt NewTimerIsr(void)
{
  stmAbsTime.u32Tick++;
  if(bAlarmSet == TRUE) {
    if(stmAlarm.u32Tick == 0) {
      bAlarmSet = FALSE;
      Scheduler();
    } else {
      stmAlarm.u32Tick--;
    }
  }
  OldTimerIsr();
}


void TimeClear(PTIME pstmTime)
{
  pstmTime->u32Tick = 0;
}


void TimeGet(PTIME pstmTime)
{
  pstmTime->u32Tick = stmAbsTime.u32Tick;
}


void TimeConvTo(PTIME pstmConvTo, UINT32 u32Sec, UINT32 u32USec)
{
  pstmConvTo->u32Tick = (double)u32Sec * TICK_SEC +
			(double)u32USec * TICK_SEC / 1000000;
}


void TimeConvBack(PTIME pstmConvBack, UINT32 *pu32Sec, UINT32 *pu32USec)
{
  double dTemp;

  dTemp = (double)pstmConvBack->u32Tick * 0.001;
  *pu32Sec = (UINT32)dTemp;
  *pu32USec = (dTemp - *pu32Sec) * 1000000;
}


void TimeAdd(PTIME pstmAccu, PTIME pstmAdd)
{
  UINT32 u32Temp;

  pstmAccu->u32Tick += pstmAdd->u32Tick;
}


BOOL TimeComp(PTIME pstmTime1, PTIME pstmTime2)
/* TRUE if Time1 > Time2 */
{
  if(pstmTime1->u32Tick > stmAbsTime.u32Tick) {
    if(pstmTime2->u32Tick > stmAbsTime.u32Tick) {
      return(pstmTime1->u32Tick > pstmTime2->u32Tick);
    } else {
      return(FALSE);
    }
  } else {
    if(pstmTime2->u32Tick > stmAbsTime.u32Tick) {
      return(FALSE);
    } else {
      return(pstmTime1->u32Tick > pstmTime2->u32Tick);
    }
  }
}


void TimeSetAlarm(PTIME pstmAlarm)
{
  disable();
  if(bAlarmSet == TRUE) {
    printf("TimeSetAlarm(): Alarm allready set.\n");
  } else {
    stmAlarm.u32Tick = pstmAlarm->u32Tick;
    bAlarmSet = TRUE;
  }
  enable();
}



void InitTimer(void interrupt (*SchedFunc)(void))
{
  TimeClear(&stmAlarm);
  bAlarmSet = FALSE;
  Scheduler = SchedFunc;

  disable();

  OldTimerIsr = getvect(TIMER_INT_VEC);
  setvect(TIMER_INT_VEC, NewTimerIsr);

  SetPITCounter(SUBTICK_TICK);

  enable();
}


void RemoveTimer(void)
{
  SetPITCounter(0);
  setvect(TIMER_INT_VEC, OldTimerIsr);
}

