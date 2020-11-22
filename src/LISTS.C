#include <stdlib.h>
#include "lists.h"

/* linked lists */

void InitList (PLIST pslsThisOne)
{
  /* initialize empty list structure */
  pslsThisOne->psndHead = (PNODE) &pslsThisOne->psndTail;
  pslsThisOne->psndTail = NULL;
  pslsThisOne->psndTailPred = (PNODE) pslsThisOne;
}

/* lineair list search */
PNODE SearchList(PLIST pslsThisOne, const void *pvKey,
		 BOOL (*Compare)(const void *, const PNODE))
{
  PNODE psndTmp = GetFirstNode(pslsThisOne);
  while(!TailNode(psndTmp)) {
    if (Compare(pvKey, psndTmp)) break;
  }
  return psndTmp->psndPred;
}



/* insert node in list */
void InsertNode(PLIST pslsThisList, PNODE psndInsertAfter, PNODE psndInsertMe)
{
  /* if psndInsertAfter == NULL insert at the begin of the list */
  if (psndInsertAfter == NULL) {
    psndInsertAfter = (PNODE) pslsThisList;
  }
  psndInsertMe->psndPred = psndInsertAfter;
  psndInsertMe->psndSucc = psndInsertAfter->psndSucc;
  psndInsertAfter->psndSucc = psndInsertMe;
  psndInsertMe->psndSucc->psndPred = psndInsertMe;
}


/* Enqueue node */
void EnqueueNode(PLIST pslsThisList, PNODE psndEnqueueMe)
{
  PNODE psndInsertBefore;
  psndInsertBefore = (PNODE) pslsThisList;
  do {
    psndInsertBefore = NextNode(psndInsertBefore);
  }
  while((psndInsertBefore->i16Priority > psndEnqueueMe->i16Priority) &&
	(psndInsertBefore->psndSucc != NULL));

  InsertNode(pslsThisList, psndInsertBefore->psndPred, psndEnqueueMe);
}


/* remove node from list */
void RemoveNode(PNODE psndRemoveMe)
{
  PNODE psndSucc, psndPred;
  psndSucc = psndRemoveMe->psndSucc;
  psndPred = psndRemoveMe->psndPred;
  psndPred->psndSucc = psndSucc;
  psndSucc->psndPred = psndPred;
}






