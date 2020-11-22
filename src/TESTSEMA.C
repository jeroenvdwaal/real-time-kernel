/* test of semafore function. */

#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include "kernel.h"


PSEMAPHORE S;

void SuperTask(void)
{
  int i = 0;
  BOOL b;
  while(1)
  {
	i++;

	b = rtkWaitTimed(S, 200000);

	disable();
	printf(b ? "T1 " : "W1 ");
	enable();

	/* disable();
	sound(440);
	enable();
	rtkDelay(1000);
	disable();
	nosound();
	enable(); */
  }
}

void SuperTask2(void)
{
  int i = 0;
  BOOL b;
  while(1)
  {
	i++;

	b = rtkWaitTimed(S, 100000);

	disable();
	printf(b ? "T2 " : "W2 ");
	enable();


/*	disable();
	sound(880);
	enable();
	rtkDelay(1000);
	disable();
	nosound();
	enable();
	ForceTaskSwitch(); */
  }
}


void main()
{
  int i = 0;
  clrscr();

  InitKernel(10,10);
  S = rtkCreateSema(Binary, 0);
  CreateTask(SuperTask, 60, 100, 20);
  CreateTask(SuperTask2, 60, 100, 10);

  rtkDelay(160000);

  while (1)
  {
    i++;
    rtkDelay(10000);
    if (i<10) rtkSignal(S);
    ForceTaskSwitch();
  }
}