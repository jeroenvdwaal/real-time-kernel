#ifndef LISTS_H
#define LISTS_H

#include "c_types.h"


#define AddHeadNode(plist,pnode) (InsertNode((plist),(NULL),(pnode)))
#define AddTailNode(plist,pnode) (InsertNode((plist),(PNODE)(((PLIST)(plist))->psndTailPred),(pnode)))

#define EmptyList(plist)            (((plist)->psndTailPred) == ((PNODE)plist))
#define GetFirstNode(plist)         ((plist)->psndHead)
#define TailNode(pnode)             ((pnode)->psndSucc == NULL)
#define NextNode(pnode)             ((pnode)->psndSucc)

#define MoveNode(pslsToList, psndAfterMe, psndThis) \
  RemoveNode((psndThis)); \
  InsertNode((pslsToList),(psndInsertMe),(psndThis));

#define MoveNodeTail(pslsToList, psndThis) \
  RemoveNode((psndThis)); \
  AddTailNode((pslsToList),(psndThis));

#define MoveNodeHead(pslsToList, psndThis) \
  RemoveNode((psndThis)); \
  AddHeadNode((pslsToList),(psndThis));

#define MoveNodeEnqueued(pslsToList, psndAfterMe, psndThis) \
  RemoveNode((psndThis)); \
  EnqueueNode((pslsToList),(psndInsertMe),(psndThis));



typedef struct tagNODE
{
  struct tagNODE *psndSucc;   /* Successor */
  struct tagNODE *psndPred;   /* Predicessor */
  INT16 i16ID;                /* Identifier */
  INT16 i16Priority;          /* Priority */
  INT16 i16Type;              /* Type */
} NODE, *PNODE;


typedef struct tagLIST        /* list header */
{
  PNODE psndHead;
  PNODE psndTail;     /* always NULL */
  PNODE psndTailPred;
} LIST, *PLIST;



/* exported functions */
void InsertNode(PLIST pslsList, PNODE psndInsertAfter, PNODE psndInsertMe);
void RemoveNode(PNODE psndRemoveMe);
void InitList(PLIST pslsThisOne);
void EnqueueNode(PLIST pslsThisList, PNODE psndEnqueueMe);
PNODE SearchList(PLIST pslsThisOne, const void *pvKey,
		 BOOL (*Compare)(const void *, const PNODE));


#endif