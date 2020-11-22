; Assembly routine for the real time kernel

	.8086
	.MODEL HUGE

	INCLUDE kernel.i

	PUBLIC _TaskSwitch
	EXTRN  _Schedule: PROC
	EXTRN  _KernelBase: KERNEL_BASE


	.CODE


_TaskSwitch	PROC
	;Save current processor status
	push ax
	push bx
	push cx
	push dx
	push si
	push di
	push bp
	push ds
	push es

	;Save current stack pointer

	;get pointer to current task table
	mov ax, SEG _KernelBase
	mov es, ax                                     ; use only es for scratch
	mov ax, es:[_KernelBase.psttRunningTask.segm]
	mov bx, es:[_KernelBase.psttRunningTask.offs]
	mov es, ax                                     ; es:bx is the task tabel pointer

	;save current stack pointer in task table
	mov es:[bx+pu8StackReg.segm], SS               ; Done !
	mov es:[bx+pu8StackReg.offs], SP

	;If required store registers math coprocessor.

	;NEXT???
	call FAR PTR _Schedule   ; return value DX:AX far ptr

	;get pointer to task table
	mov ax, SEG _KernelBase
	mov es, ax                                     ; use only es for scratch
	mov ax, es:[_KernelBase.psttRunningTask.segm]
	mov bx, es:[_KernelBase.psttRunningTask.offs]
	mov es, ax                                     ; es:bx is the task tabel far pointer


	;Set new stack
	;NOTE: between these to instructions interrupts are not allowed !
	;else an invalid stack setting is possible !
	mov SS, es:[bx+pu8StackReg.segm]               ; Done !
	mov SP, es:[bx+pu8StackReg.offs]


	;test if this task is new
	mov ax, STATE_INITIALIZE
	cmp ax, es:[bx+u16State]
	jne not_new

	mov ax, STATE_RUNNING     ; new state is the running state
	mov es:[bx+u16State], ax


	mov al, 20h
	out 20h, al
	iret

not_new:


	;If required restore registers coprocessor

	;Restore processor status for running-task
	pop es
	pop ds
	pop bp
	pop di
	pop si
	pop dx
	pop cx
	pop bx
	pop ax

	;Leave TaskSwitch by ending interrupt routine
	mov al, 20h
	out 20h, al

	iret

	ENDP

	END