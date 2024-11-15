
const char hello[] __attribute__((section(".text"))) = "Hello, World!\n";
const int hello_len __attribute__((section(".text"))) = 14;
const int fd __attribute__((section(".text"))) = 1;

long	sys_write(int fd, const void *buf, unsigned long count)
{
	long ret;
	__asm__ __volatile__ (
		"movq %0, %%rdi\n"
		"movq %1, %%rsi\n"
		"movq %2, %%rdx\n"
		"movq $1, %%rax\n"
		"syscall"
		: : "g" (fd), "g" (buf), "g" (count));
	__asm__ ("movq %%rax, %0" : "=r" (ret));

	return ret;
}

void	sys_exit(int status)
{
	__asm__ __volatile__ (
		"movq %0, %%rdi\n"
		"movq $60, %%rax\n"
		"syscall"
		: : "g" (status));
}

void	_start()
{
	sys_write(fd, hello, hello_len);
	sys_exit(0);
}



