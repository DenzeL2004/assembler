#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "src/log_info/log_errors.h"


static FILE *fp_logs = stderr;


int main (int argc, char *argv[])
{
    #ifdef USE_LOG
        
        fp_logs = Open_logs_file ();
        
        if (fp_logs == stderr)
            return OPEN_FILE_LOG_ERR;

    #endif 

    char *input_file  = "";
    char *output_file = "";

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

    char *input_assembler_argv = strdup ("assembler.exe ");
    strcat(input_assembler_argv, input_file);
    strcat(input_assembler_argv, output_file);

    //printf ("%s\n", input_assembler_argv);

    if (!input_assembler_argv)
    {
        Log_report ("Error copying command line arguments in assembler\n");
        return -1;
    }
    
   // int d = system (input_assembler_argv);
    //printf ("%d\n", d);
    if (system (input_assembler_argv))
    {
        Log_report ("Assembler execution failed\n");
        return -1;
    }

    
    char *input_cpu_argv = strdup ("proc.exe ");
    strcat("", output_file);

    if (!input_cpu_argv)
    {
        Log_report ("Error copying command line arguments in proc\n");
        return -1;
    }

    printf ("%s\n", input_cpu_argv);

    if (system (input_cpu_argv))
    {
        Log_report ("CPU execution failed\n");
        return -1;
    }

    free (input_assembler_argv);
    free (input_cpu_argv);

    #ifdef USE_LOG
        
        if (Close_logs_file (fp_logs))
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}