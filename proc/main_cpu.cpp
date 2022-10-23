#include <stdio.h>
#include <string>
#include <windows.h>

#include "proc.h"
#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"


int main (int argc, char *argv[])
{
    #ifdef USE_LOG
        if (Open_logs_file ())
            return OPEN_FILE_LOG_ERR;

    #endif 

    char *Input_file  = nullptr;

    switch (argc)
    {
        case 1:
            Log_report ("Input file not specified\n");
            return -1;
        
        case 2:
            Input_file = argv[1];
            break;
        
        default:
            Log_report ("Too many input parameters\n");
            return -1;
    }


    system(("chcp " + std::to_string(CP_UTF8)).c_str());

    int fdin = open (Input_file, O_RDONLY, 0);
    if (fdin < 0)
    {
        Log_report ("File not opened after assembly\n");
        return -1;
    }

    if (Run_proc (fdin))
    {
        Log_report ("Processor error\n");
        return -1;
    }


    close (fdin);

    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}