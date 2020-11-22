#include <stdio.h>
#include <conio.h>
#include "timer.h"


BOOL Temp;


void interrupt Test(void)
{
  Temp = TRUE;
}


void main(void)
{
  TIME stmTime;
  UINT32 u32Sec, u32USec;

  clrscr();
  InitTimer(Test);

  printf("Wacht abs tijd 0 sec, 10000 usec\n");
  Temp= FALSE;
  TimeConvTo(&stmTime, 0, 10000);
  TimeSetAlarm(&stmTime);
  while(!Temp);
  TimeGet(&stmTime);
  TimeConvBack(&stmTime, &u32Sec, &u32USec);
  printf("%lu Sec, %lu uSec\n", u32Sec, u32USec);

  printf("Wacht abs tijd 0 sec, 60000 usec\n");
  Temp= FALSE;
  TimeConvTo(&stmTime, 0, 50000);
  TimeSetAlarm(&stmTime);
  while(!Temp);
  TimeGet(&stmTime);
  TimeConvBack(&stmTime, &u32Sec, &u32USec);
  printf("%lu Sec, %lu uSec\n", u32Sec, u32USec);

  printf("Wacht abs tijd 1 sec, 60000 usec\n");
  Temp= FALSE;
  TimeConvTo(&stmTime, 1, 0);
  TimeSetAlarm(&stmTime);
  while(!Temp);
  TimeGet(&stmTime);
  TimeConvBack(&stmTime, &u32Sec, &u32USec);
  printf("%lu Sec, %lu uSec\n", u32Sec, u32USec);

  printf("Wacht abs tijd 3 sec, 60000 usec\n");
  Temp= FALSE;
  TimeConvTo(&stmTime, 2, 0);
  TimeSetAlarm(&stmTime);
  while(!Temp);
  TimeGet(&stmTime);
  TimeConvBack(&stmTime, &u32Sec, &u32USec);
  printf("%lu Sec, %lu uSec\n", u32Sec, u32USec);

  RemoveTimer();
}
