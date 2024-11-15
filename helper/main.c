
const char hello[] __attribute__((section(".text"))) = "Hello, World!\n";
const int hello_len __attribute__((section(".text"))) = 14;
const int fd __attribute__((section(".text"))) = 1;

/* g (fd) g is either a register or a memory location or constant.
 * f is a floating point register.
 * r is a general purpose register.
 * m is a memory location.
 * i is an immediate value.
 * a is an address.
 * =r means that the compiler can choose any register to use.
 * __attribute__ is used to specify special attributes when making a declaration.
*/

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

void	*sys_map(void *addr, unsigned long length, int prot, int flags, int fd, unsigned long offset)
{
	void *ret;
	__asm__ __volatile__ (
		"movq %0, %%rdi\n"
		"movq %1, %%rsi\n"
		"movq %2, %%rdx\n"
		"movq %3, %%r10\n"
		"movq %4, %%r8\n"
		"movq %5, %%r9\n"
		"movq $9, %%rax\n"
		"syscall"
		: : "g" (addr), "g" (length), "g" (prot), "g" (flags), "g" (fd), "g" (offset));
	__asm__ ("movq %%rax, %0" : "=r" (ret));

	return ret;
}

int	sys_fstat(int fd, void *buf)
{
	int ret;
	__asm__ __volatile__ (
		"movq %0, %%rdi\n"
		"movq %1, %%rsi\n"
		"movq $5, %%rax\n"
		"syscall"
		: : "g" (fd), "g" (buf));
	__asm__ ("movq %%rax, %0" : "=r" (ret));

	return ret;
}

int	sys_open(const char *pathname, int flags, int mode)
{
	int ret;
	__asm__ __volatile__ (
		"movq %0, %%rdi\n"
		"movq %1, %%rsi\n"
		"movq %2, %%rdx\n"
		"movq $2, %%rax\n"
		"syscall"
		: : "g" (pathname), "g" (flags), "g" (mode));
	__asm__ ("movq %%rax, %0" : "=r" (ret));

	return ret;
}


int	sys_close(int fd)
{
	int ret;
	__asm__ __volatile__ (
		"movq %0, %%rdi\n"
		"movq $3, %%rax\n"
		"syscall"
		: : "g" (fd));
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
	int fd = sys_open("/tmp/test1/hello", 0, 0);
	int size = sys_fstat(fd, 0);
	sys_write(fd, hello, hello_len);
	sys_exit(0);
}
