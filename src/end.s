default rel
global end
section .text

sign:
	db "Famine (c)oded by Francis", 0x0a, 0
end:
	; exit
	mov rax, 60
	xor rdi, rdi
	syscall
