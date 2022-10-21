#ifndef _ASM_CONFIG_H_
#define _ASM_CONFIG_H_

typedef int elem;

static const char *Name_output_file = "out.bin";

static const int Ver = 1;

static const int Sig = 'D' * 256 + 'K';

static const int Cnt_reg = 16;

const int Max_cnt_cmd = (1 << 5);

enum Arg_type
{
    ARG_IMM = 1 << 5,
    ARG_REG = 1 << 6,
    ARG_RAM = 1 << 7,
};

#define DEF_REG(let, num)   \
    R##let##X = num,        


enum Regs
{
    #include "regs.h"
};

#undef DEF_REG



#endif //#endif _ASM_CONFIG_H_