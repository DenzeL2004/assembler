#include <stdio.h>
#include <fcntl.h>

#include "assembler.h"

int main ()
{
    int fd = open ("input.txt", O_RDONLY, 0);
    
    Convert_operations (fd); 

    return 0;
}