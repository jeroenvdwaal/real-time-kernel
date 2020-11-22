/* test of rtkDelay() */

#include <dos.h>
#include <conio.h>
#include "kernel.h"

extern KERNEL_BASE KernelBase;

void SuperTask(void)
{
  int i = 0;
  unsigned char *screen;

  while(1)
  {
    i++;
    screen = MK_FP(0xb800, KernelBase.psttRunningTask->sndPrimaryLink.i16ID);
    *screen = i%30 + 'a';

    disable();
    sound(400);
    enable();
    rtkDelay(10000);

    disable();
    nosound();
    enable();

    rtkDelay(120000);
    ForceTaskSwitch();
  }
}


void main()
{
  int i, x;
  unsigned char *screen = MK_FP(0xb800,0x0);
  clrscr();

  InitKernel(10,1);
  CreateTask(SuperTask, 60, 100, 2);
  while (1)
  {
    i++;
    *screen = i%30 + 'a';
    disable();
    sound(1000);
    enable();

    disable();
    rtkDelay(2000);
    nosound();
    enable();
    rtkDelay(110000);
    ForceTaskSwitch();
  }
}