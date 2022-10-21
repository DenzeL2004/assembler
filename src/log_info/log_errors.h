#ifndef _LOG_ERRORS_H_
#define _LOG_ERRORS_H_

#include <stdio.h>

#include "log_def.h"

enum Log_errors
{
    OPEN_FILE_LOG_ERR  = -1, 
    CLOSE_FILE_LOG_ERR = -2    
};

#define USE_LOG

#ifdef USE_LOG

    #define USE_LOG_REPORT

#endif

#ifdef USE_LOG

    #ifdef USE_LOG_REPORT
        
        #define Log_report(...)                            \
                Log_report_ (LOG_ARGS, fp_logs, __VA_ARGS__)

    #else

        #define Log_report(...)                            \
            Err_report_ (LOG_ARGS, fp_logs)
    
    #endif

#else

    #define Log_report(...)           

#endif

FILE* Open_logs_file  ();

int Close_logs_file (FILE* fp_logs);

int Log_report_ (LOG_PARAMETS, FILE *fp_logs, const char *format, ...);

int Err_report_ (const char* file_name, const char* func_name, int line, FILE *fp_logs);

#endif