/* test for the queue module. */

#include <stdio.h>
#include <conio.h>
#include "queue.h"


void main()
{
  PQUEUE q;
  char key;
  int x = 0;
  int a;
  q = CreateQueue(10, sizeof(int));
  key = getch();
  while (key != 'q')
  {
    switch(key)
    {
      case 'a':
	   AppendQueue(q, &x);
	   x++;
	   break;

      case 's':
	   ServeQueue(q, &a);
	   break;
	    }
    printf("\n x = %d     a = %d", x, a);
    key = getch();
  }

}