#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h> 
#include <sys/stat.h>
#include <io.h>

#include "assembler.h"
#include "proc.h"


int main ()
{
    int fd = open ("input.txt", O_RDONLY, 0);
    
    if (Convert_operations (fd))
        return -1; 

    close (fd);

    fd = open (name_output_file, O_RDONLY, 0);

    Run_proc (fd);

    close (fd);


    return 0;
}