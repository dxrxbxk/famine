NAME = famine

src = src/famine.c \
      src/data.c \
      src/map.c \
      src/silvio.c \
      src/utils.c 

asm = src/syscall.s \
      src/end.s

# Génération des fichiers objets correspondants
obj = $(src:.c=.o) $(asm:.s=.o)

cflags = -g -fno-stack-protector -fpic 

sflags = -f elf64 -g

ldflags = -z noexecstack -nostdlib 

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(obj)
	gcc -o $(NAME) $(obj) $(ldflags)

src/%.o: src/%.c
	gcc $(cflags) -c -o $@ $< -I./inc

src/%.o: src/%.s
	nasm $(sflags) -o $@ $<

clean:
	rm -f $(obj)

fclean: clean
	rm -f $(NAME)

re: fclean all
