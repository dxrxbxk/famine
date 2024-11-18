%macro write_string 2
	mov rax, 1
	mov rdi, 0
	mov rsi, %1
	mov rdx, %2
	syscall
%endmacro

%macro write_format 2
	push rdi
	push rsi
	push rax
	mov rdi, %1
	mov rsi, %2
	mov rax, 0
	call printf
	pop rax
	pop rsi
	pop rdi
%endmacro
