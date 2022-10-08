#ifndef _PROC_H_
#define _PROC_H_

enum Processor_err
{
    INIT_FILE_INFO_ERR      = -1,
    
    VERSION_MISMATCH_ERR    = -2,
    SIGNATURE_MISMATCH_ERR  = -3,

    PROCESS_ERR             = -4,
    PROCESS_COM_ERR         = -5,
};

int Processing (int fdin);

#endif