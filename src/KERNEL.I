STATE_READY      EQU      0
STATE_RUNNING    EQU      1
STATE_INITIALIZE EQU      2


FARPTR          STRUC
		offs   DW  ?
		segm   DW  ?
		ENDS


NODE		STRUC
		psndSucc	FARPTR <>
		psndPred	FARPTR <>
		i16ID		DW  ?
		i16Priority	DW  ?
		i16Type		DW  ?
		ENDS

LIST		STRUC
		psndHead        FARPTR <>
		psndTail        FARPTR <>
		psndTailPred    FARPTR <>
		ENDS


TASK_TABLE	STRUC
		sndPrimaryLink	 NODE  <>
		sndSecondaryLink NODE  <>
		pu8StackBase	 DD DATAPTR PTR ?
		pu8StackTop	 DD DATAPTR PTR ?
		pu8StackReg	 FARPTR ?
		u16State	 DW ?
		u32Timer         DD ?
		ENDS

KERNEL_BASE     STRUC
		slsReadyList    LIST <>
		slsTimedList    LIST <>
		slsWaitList     LIST <>
		psttRunningTask FARPTR <>
		ENDS