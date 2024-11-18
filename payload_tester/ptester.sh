#!/usr/bin/env -S zsh --no-rcs --no-globalrcs --pipefail

declare -rg bin=$1
declare -rg raw='.text'
declare -rg src='shellcode.c'

declare -rg cc='gcc'
declare -rg cflags=('-O0' '-std=c17' '-Wall' '-Wextra' '-Werror')
declare -rg output='shellcode_tester'

declare -rg includes='#include <sys/mman.h>\n#include <stdint.h>\n#include <unistd.h>\n\n'
declare -rg array_start='unsigned char shellcode[] = {\n'
declare -rg array_end='\n};\n'
declare -rg main='
int main() {

    mprotect((void *)((uintptr_t)shellcode & ~(sysconf(_SC_PAGE_SIZE) - 1)), sysconf(_SC_PAGE_SIZE), PROT_READ | PROT_WRITE | PROT_EXEC);

    void (*func)() = (void (*)())((unsigned char *)shellcode);

    func();  // Appel du shellcode

    return 0;
}\n'


# clean up
function cleanup() {
    rm -f $raw $src $output
}


# extract .text section
if ! objcopy '--output-target=binary' '--only-section=.text' $bin $raw; then
    echo 'Failed to extract .text section'
    cleanup
    exit 1
fi


# write includes
if ! echo -n $includes > $src; then
    echo 'Failed to write source file'
    cleanup
    exit 1
fi

# write array start
if ! echo -n $array_start >> $src; then
    echo 'Failed to write source file'
    cleanup
    exit 1
fi

# format as hex string
if ! hexdump '--no-squeezing' '--format' '"0\x\" 1/1 "%02x,"' $raw >> $src; then
    echo 'Failed to format as hex string'
    cleanup
    exit 1
fi

# write source file
if ! echo -n $array_end >> $src; then
    echo 'Failed to write source file'
    cleanup
    exit 1
fi

# write main
if ! echo -n $main >> $src; then
    echo 'Failed to write source file'
    cleanup
    exit 1
fi

# compile
if ! $cc $cflags $src '-o' $output; then
    echo 'Failed to compile source file'
    cleanup
    exit 1
fi

# execute
./$output;

cleanup

exit 0
