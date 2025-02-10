#include <stdio.h>
#include <string.h>

void vuln_function(char *input) {
    char buffer[64];
    // Cette fonction est vulnérable à un dépassement de tampon
    strcpy(buffer, input);
    printf("Input copied to buffer: %s\n", buffer);
}

int main() {
    char large_input[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
    vuln_function(large_input);
    return 0;
}

