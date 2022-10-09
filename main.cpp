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
    
    Convert_operations (fd); 

    close (fd);

    fd = open ("convert_input.bin", O_RDONLY, 0);

    Processing (fd);

    close (fd);

    for (int id_com = 0; id_com < 6; id_com++)
    {
        printf ("%02d ",id_com);     
    }

    return 0;
}