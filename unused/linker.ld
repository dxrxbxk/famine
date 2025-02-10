ENTRY(_start)

SECTIONS
{
    . = 0x1000;               /* Adresse de départ */
    .text : {
		src/main.o(.text)       /* Place le code de main.o */
        *(.text)              /* Place le reste du code */
    }
}
