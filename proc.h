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

struct Bin_file 
{
    int  signature   = 0;
    int  asm_version = 0;
};

const int Min_stack_size = 5;

int Run_proc (int fdin);

#endif //#endif _PROC_H_