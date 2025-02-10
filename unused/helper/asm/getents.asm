BITS 32
GLOBAL _start

section .data
filename db '.', 0x0            ; Nom du répertoire à ouvrir
buffer_size equ 0x400           ; Taille du buffer pour getdents (1024 octets)
section .bss
buffer resb buffer_size         ; Buffer pour stocker les entrées de getdents

section .text
_start:
    jmp get_eip                 ; Saut à get_eip pour empiler l'adresse de `filename`

run:
    ; Ouvrir le répertoire courant "."
    xor eax, eax
    mov al, 5                   ; sys_open (5)
    pop ebx                     ; ebx = adresse de filename (".")
    xor ecx, ecx                ; flags = 0 (O_RDONLY)
    xor edx, edx                ; mode = 0 (non pertinent ici)
    int 0x80                    ; Appel système pour ouvrir le répertoire

    test eax, eax               ; Vérifie si l'ouverture a réussi
    js error                    ; Si erreur, saute à error
    mov edi, eax                ; Sauvegarde le descripteur de fichier dans edi

read_dir:
    ; Lire les entrées du répertoire
    mov eax, 0x8d               ; sys_getdents (141)
    mov ebx, edi                ; fd du répertoire
    mov ecx, buffer             ; Buffer pour stocker les entrées du répertoire
    mov edx, buffer_size        ; Taille du buffer
    int 0x80                    ; Appel système getdents

    test eax, eax               ; Vérifie si getdents a réussi
    js close_dir                ; Si erreur, saute à close_dir
    jz close_dir                ; Si aucun résultat, termine la boucle

    mov esi, eax                ; ESI = taille totale des données lues
    mov edi, buffer             ; EDI = début du buffer

next_entry:
    ; Chaque entrée est une structure linux_dirent :
    ; d_ino (4 octets), d_off (4 octets), d_reclen (2 octets), d_name (nom de fichier)

    mov ebx, [edi + 10]         ; Charge `d_reclen` (2 octets)
    add ebx, edi                ; Calcule la prochaine entrée en ajoutant `d_reclen` à l'adresse actuelle
    add edi, 10                 ; Décale l'index pour pointer vers `d_name` dans la structure

    ; Écriture du nom de fichier sur la sortie standard
    mov eax, 4                  ; sys_write (4)
    mov ebx, 1                  ; fd = 1 (stdout)
    mov ecx, edi                ; Pointeur vers le nom de fichier
    mov edx, 255                ; Taille max pour nom de fichier
    int 0x80                    ; Appel système pour écrire le nom

    ; Aller à l'entrée suivante
    mov edi, ebx                ; EDI = pointeur vers l'entrée suivante
    cmp edi, buffer_size + buffer ; Vérifie si on a atteint la fin du buffer
    jb next_entry               ; Revenir en arrière si des entrées restent à traiter

    ; Lire plus d'entrées
    jmp read_dir

close_dir:
    ; Fermer le descripteur du répertoire
    mov eax, 6                  ; sys_close (6)
    mov ebx, edi                ; Descripteur de fichier du répertoire
    int 0x80                    ; Appel système close

error:
    ; Quitter le programme avec sys_exit
    mov eax, 1                  ; sys_exit (1)
    xor ebx, ebx                ; Code de retour 0
    int 0x80

get_eip:
    call run                    ; Appelle run et empile l'adresse de `filename`
