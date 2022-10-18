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
#include "../Generals_func/generals.h"
#include "../Logs/log_errors.h"


static FILE *fp_logs = stderr;


int main (int argc, char *argv[])
{
    #ifdef USE_LOG
        
        fp_logs = Open_logs_file ();
        
        if (fp_logs == stderr)
            return OPEN_FILE_LOG_ERR;

    #endif 

    char *Input_file  = (char*) Name_input_file;
    char *Output_file = (char*) Name_output_file;

    switch (argc)
    {
        case 1:
            break;
        
        case 2:
            Input_file = argv[1];
            break;

        case 3:
            Input_file  = argv[1];
            Output_file = argv[2];
            break;
        
        default:
            Log_report ("Too many input parameters\n");
            return -1;
    }

    int fd = open (Input_file, O_RDONLY, 0);
    if (fd < 0)
    {
        Log_report ("Input file not opened\n");
        return -1;
    }

    if (Convert_operations (fd, Output_file))
    {
        Log_report ("Assembler error\n");
        return -1; 
    }

    close (fd);

    fd = open (Output_file, O_RDONLY, 0);
    if (fd < 0)
    {
        Log_report ("File not opened after assembly\n");
        return -1;
    }

    if (Run_proc (fd))
    {
        Log_report ("Processor error\n");
        return -1;
    }

    close (fd);

    #ifdef USE_LOG
        
        if (Close_logs_file (fp_logs))
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}