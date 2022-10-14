#ifndef _ASM_CONFIG_H_
#define _ASM_CONFIG_H_



static const int Ver = 1;

static const int Sig = 'D' * 256 + 'K';

static const char *name_output_file = "out.bin";

static const int Cnt_reg = 4;

enum Arg_type
{
    ARG_NUM = 1 << 5,
    ARG_REG = 1 << 6,
    ARG_RAM = 1 << 7,
};

enum Regs
{
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3,
};



#endif //#endif _ASM_CONFIG_H_