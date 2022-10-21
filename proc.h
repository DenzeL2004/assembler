#ifndef _PROC_H_
#define _PROC_H_

#include "src/stack/stack.h"
#include "architecture.h"

const int Ln_ram = 20;

const int Cl_ram = 20;

const int Ram_size = 400;

const int Min_stack_size = 5;

const char Cmd_mask = (1 << 5) - 1;

const int Program_delay = 1;

enum Processor_err
{
    PROC_CTOR_ERR           = -1,
    PROC_DTOR_ERR           = -2,
    
    VERSION_MISMATCH_ERR    = -3,
    SIGNATURE_MISMATCH_ERR  = -4,

    PROCESS_ERR             = -5,
    PROCESS_COM_ERR         = -6,

    READ_FROM_BIN_ERR       =-7,
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

int Run_proc (int fdin);

#endif //#endif _PROC_H_