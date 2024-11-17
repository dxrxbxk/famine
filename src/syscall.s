global _syscall

section .text
_syscall:
	mov rax, rdi
	mov rdi, rsi
	mov rsi, rdx
	mov rdx, rcx
	mov r10, r8
	mov r8, r9
	mov r9, qword [rsp + 8] ; get the 7th argument
	syscall

	test rax, rax
	js .error

	ret

.error:
	mov rax, -1
	ret




