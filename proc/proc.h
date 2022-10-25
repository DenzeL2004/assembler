#ifndef _PROC_H_
#define _PROC_H_

#include "../architecture/architecture.h"
#include "../src/stack/stack.h"


const int Ln_ram = 20;

const int Cl_ram = 20;

const int Ram_size = 400;

const int Min_stack_size = 5;

const int Program_delay = 300;

const int Window_height = 400;

const int Window_width  = 100;

enum Processor_err
{
    PROC_CTOR_ERR           = -1,
    PROC_DTOR_ERR           = -2,
    
    VERSION_MISMATCH_ERR    = -3,
    SIGNATURE_MISMATCH_ERR  = -4,

    PROCESS_ERR             = -5,
    PROCESS_COM_ERR         = -6,

    READ_FROM_BIN_ERR       = -7,
};

struct Bin_file 
{
    int  signature   = 0;
    int  asm_version = 0;
};

struct  Cpu_struct
{
    int cnt_bytes = 0;
    unsigned char *code = nullptr;

    int cur_cmd = 0;
    elem regs[Cnt_reg] = {0};

    elem *ram = nullptr;

    Stack stack = {};    
};


#define USE_CPU_CODE_DUMP   //<- We write to the log file information by cpu code

#define USE_CPU_REG_DUMP    //<- We write to the log file information by cpu regs


int Run_proc (int fdin);


#endif //#endif _PROC_H_