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

    FILE *fpin = Open_file_ptr (Input_file, "rb");
    if (Check_nullptr (fpin))
    {
        Log_report ("File not opened after assembly\n");
        return -1;
    }

    if (Run_proc (fpin))
    {
        Log_report ("Processor error\n");
        return -1;
    }


    Close_file_ptr (fpin);

    #ifdef USE_LOG
        
        if (Close_logs_file ())
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}