#ifndef _LOG_ERRORS_H_
#define _LOG_ERRORS_H_

#include "log_def.h"
#include "config.h"

#ifdef USE_LOG

    #define Log_report(message)                            \
            Log_report_ (LOG_ARGS, fp_logs, message)

#else

    #define Log_report(message)    

#endif

FILE* Open_logs_file  ();

int Close_logs_file (FILE* fp_logs);

int Log_report_ (LOG_PARAMETS, FILE* fp_logs, const char *format, ...);

#endif