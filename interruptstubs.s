; PROBLEM WITH THIS FILE:
;
; The macros are properly expanded and can be pointed to by the C++ code.
; However, this code will not call handleInterrupt().
; int_bottom is essentially a straight copy of AM's code.
; Changing the first line in the macro was problematic (and potentially erroneous).

IRQ_BASE equ 0x20

extern _ZN16InterruptManager15handleInterruptEhm

global _ZN16InterruptManager22IgnoreInterruptRequestEv

;%macro HandleException 1
;global _ZN16InterruptManager16HandleException %+ %1()Ev
;_ZN16InterruptManager16HandleException %+ %1()Ev:
;	movb $\%1, (interruptnumber)
;	jmp int_bottom
;%endmacro

%macro HandleInterruptRequest 1
global _ZN16InterruptManager26HandleInterruptRequest %+ %1Ev
_ZN16InterruptManager26HandleInterruptRequest %+ %1Ev:
	mov dword [%1 + IRQ_BASE], interruptnumber
	jmp int_bottom
%endmacro

section .text

HandleInterruptRequest 0x00
HandleInterruptRequest 0x01

int_bottom:

	pusha
	push ds
	push es
	push fs
	push gs

	push esp
	push (interruptnumber)
	call _ZN16InterruptManager15handleInterruptEhm
	mov eax, esp
	
	pop gs
	pop fs
	pop es
	pop ds
	popa

_ZN16InterruptManager22IgnoreInterruptRequestEv:	

	iret

section .data
	interruptnumber: db 0
