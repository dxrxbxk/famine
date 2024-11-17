#include "famine.h"
#include "utils.h"

void destroy_data(data_t *data) {

	if (data == NULL)
		return;

	if (data->_file_map != NULL)
		_syscall(SYS_MUNMAP, (long)data->_file_map, data->_file_size);

	data->_file_map = NULL;

	if (data->_fd != -1)
		_syscall(SYS_CLOSE, data->_fd);

	data->_fd = -1;
}


int init_data(data_t* data, const char *filename) {

	data->_fd            = -1;
	data->_file_map      = NULL;
	data->_file_size     = 0U;
	data->_oentry_off    = 0U;

	data->_fd = _syscall(SYS_OPEN, (long)filename, O_RDONLY);

	if (data->_fd == -1) {
		ERR_SYS("open");
		return -1;
	}

	{
		char ehdr[5U];
		ehdr[4] = 0;

		if (_syscall(SYS_READ, data->_fd, (long)ehdr, 5U) == -1) {
			ERR_SYS("read");
			_syscall(SYS_WRITE, 1, (long)"read\n", 5);
			return -1;
		}

		if (ft_memcmp(ehdr, "\x7f""ELF", 4) != 0) {
			ERR("Not an ELF file");
			return -1;
		}
		else if (ft_memcmp(ehdr + 4, "\x02", 1) != 0) {
			ERR("Not a 64-bit ELF file");
			return -1;
		}
	}


	const ssize_t file_size = _syscall(SYS_LSEEK, data->_fd, 0, SEEK_END);

	if (file_size == -1) {
		ERR_SYS("lseek");
		return -1;
	}

	data->_file_size = (size_t)file_size;

	void *file_map = (long *)_syscall(SYS_MMAP, 0, data->_file_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, data->_fd, 0);

	if (file_map == MAP_FAILED) {
		ERR_SYS("mmap");
		return -1;
	}

	data->_file_map = (uint8_t *)file_map;

	return 0;
}

static void	infect(const char *filename)
{
	data_t data;
	ft_memset(&data, 0, sizeof(data_t));
	int ret = 0;

	ret = init_data(&data, filename);
	destroy_data(&data);
	
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
	ft_strcpy(path, dir);
	ft_strlcat(path, "/", 1024);
	ft_strlcat(path, file, 1024);
}

static void	open_files(const char *path)
{
	int fd = _syscall(SYS_OPEN, (long)path, O_RDONLY);
	if (fd == -1) _syscall(SYS_EXIT, 1);

	char buf[1024];
	unsigned long ret = 0;

	// getents64
	while ((ret = _syscall(SYS_GETDENTS64, fd, buf, 1024)) > 0)
	{
		for (unsigned long i = 0; i < ret;)
		{
			struct linux_dirent *dir = (struct linux_dirent *)(buf + i);
			if (dir->d_type == DT_REG)
			{
				char *file = dir->d_name;
				PRINT("file: %s\n", file);
				char new_path[1024];
				make_path(new_path, path, file);

				infect(new_path);
				_syscall(SYS_EXIT, 0);
			}
			else if (dir->d_type == DT_DIR)
			{
				char *dir_name = dir->d_name;
				if (dir_name[0] != '.')
				{
					char new_path[1024];
					PRINT("dir: %s\n", dir_name);
					open_files(new_path);
				}
			}
			i += dir->d_reclen;
		}
	}
}

void	famine(void)
{
	static const char *v_path[2] = {"/tmp/test", "/tmp/test2"};
	unsigned long v_size = sizeof(v_path) / sizeof(v_path[0]);
	for (unsigned long i = 0; i < v_size; i++)
	{
		open_files(v_path[i]);
	}
	return ;
}

//void	_start(void)
//{
//	famine();
//	_syscall(SYS_EXIT, 0);
//}
//
int main(void)
{
	famine();
	return 0;
}
