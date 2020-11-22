/* example & test of real time kernel's mailbox functions. */

#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include "kernel.h"


MailBox mbDisplay;
typedef struct tagTEXT
{
  int x;
  int y;
  char str[81];
} XYTEXT;

void DisplayTask(void)
{
  XYTEXT text;
  while(1)
  {
    rtkGet(mbDisplay, &text);
    gotoxy(text.x, text.y);
    cprintf("%s", text.str);
  }
}


MailBox mbInt;
void IntReceiverTask(void)
{
  XYTEXT text;
  BOOL b;
  int i = 0;
  text.x = 40;

  text.y = 1;
  sprintf(text.str, "Int receiving task");
  rtkPut(mbDisplay, &text);

  text.y = 3;
  sprintf(text.str, "Received :");
  rtkPut(mbDisplay, &text);

  text.y = 4;
  sprintf(text.str, "Status :");
  rtkPut(mbDisplay, &text);

  text.x = 52;
  text.y = 3;
  while(1)
  {
	b = rtkGetTimed(mbInt, &i, 100000);
	text.y = 3;
	sprintf(text.str, "%d", i);
	rtkPut(mbDisplay, &text);
	text.y = 4;
	sprintf(text.str, "%s", b ? "SUCCES" : "TIMED OUT");
	rtkPut(mbDisplay, &text);

  }
}

void IntSendingTask(void)
{
  XYTEXT text;
  int i = 0;
  text.x = 4;

  text.y = 11;
  sprintf(text.str, "Int sending task");
  rtkPut(mbDisplay, &text);

  text.y = 13;
  sprintf(text.str, "Sending :");
  rtkPut(mbDisplay, &text);

  text.y = 14;
  sprintf(text.str, "Status :");
  rtkPut(mbDisplay, &text);

  text.x = 16;
  text.y = 13;
  while(1)
  {
	rtkDelay(30000);
	i++;
	if (i < 10) {
	  rtkPut(mbInt, &i);
	}
	sprintf(text.str, "%d", i);
	rtkPut(mbDisplay, &text);
  }
}

void main()
{
  int i = 0;
  UINT8 pu8Buffer[4000];
  clrscr();
  InitKernel(100,1);
  mbDisplay = rtkCreateMailBox(10, sizeof(XYTEXT));
  mbInt = rtkCreateMailBox(10, sizeof(int));
  CreateTask(DisplayTask, 1024, 100, 2);
  CreateTask(IntReceiverTask, 1024, 100, 3);
  CreateTask(IntSendingTask, 1024, 100, 4);

  while (1)
  {
	i++;
	rtkDelay(10000);
	rtkListTasks(pu8Buffer, 4000);
	disable();
	gotoxy(1, 15);
	cprintf("%s",pu8Buffer);
	enable();
  }
}
