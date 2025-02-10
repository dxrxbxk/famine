NAME = famine

src = $(wildcard src/*.c)
asm = $(wildcard src/*.s)
obj = $(src:.c=.o)
obj += $(asm:.s=.o)

first_obj = src/main.o

obj := $(filter-out $(first_obj), $(obj))

cflags = -g

sflags = -f elf64 -g

ldflags = -z noexecstack -nostdlib -m elf_x86_64

.PHONY: all clean fclean re

all: $(NAME)

$(NAME): $(first_obj) $(obj)
	ld -o $@ $^ $(ldflags)
	./ptester.sh $(NAME)

%.o: %.c
	gcc $(cflags) -c -o $@ $< -I./inc

%.o: %.s
	nasm $(sflags) -o $@ $<

clean:
	rm -f $(obj)

fclean: clean

re: fclean all

