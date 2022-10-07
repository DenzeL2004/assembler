#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"

int main ()
{
    int fd = open ("input.txt", O_RDONLY, 0);
    
    Convert_operations (fd); 

    close (fd);
    
    fd = open ("convert_input.bin", O_RDONLY, 0);

    char *buf = (char*) calloc (100, sizeof (int));

    File_info flio = {};

    flio.code = (int*) calloc (100, sizeof (int));
    
        printf ("OK1\n");

    int cnt = read (fd, &flio, sizeof (flio));

    printf ("%s\n", &(flio.ver)); 
    printf("%d", strcmp((char*)(&flio.ver), "1.0"));

    close (fd);

    return 0;
}