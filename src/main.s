BITS 64

extern famine
extern _end

section .text
global _start

_start:
	push rdx
	call famine

	;exit

	pop rdx
	mov rax, 60
	xor rdi, rdi
	syscall
	ret
