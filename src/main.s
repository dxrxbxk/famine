BITS 64

extern famine
extern end

default rel

section .text
global _start

_start:
	push rdx
	call famine

	pop rdx

	jmp end


	;exit

