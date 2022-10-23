#include <stdio.h>


#include "assembler.h"
#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"


static FILE *fp_logs = stderr;


int main (int argc, char *argv[])
{
    #ifdef USE_LOG
        
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif 

    char *input_file  = nullptr;
    char *output_file = (char*) Default_name_output_file;

    switch (argc)
    {
        case 1:
            Log_report ("Input file not specified\n");
            return -1;
        
        case 2:
            input_file = argv[1];
            break;

        case 3:
            input_file  = argv[1];
            output_file = argv[2];
            break;
        
        default:
            Log_report ("Too many input parameters\n");
            return -1;
    }

    int fdin = open (input_file, O_RDONLY, 0);
    if (fdin < 0)
    {
        Log_report ("Input file not opened\n");
        return -1;
    }

    if (Convert_operations (fdin, output_file))
    {
        Log_report ("Assembler error\n");
        return -1; 
    }

    close (fdin);


    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}