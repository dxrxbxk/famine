BITS 64

default rel

extern famine
extern end


section .text
global _start


_start:
	call famine

	jmp end


	;exit

