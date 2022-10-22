#ifndef _ARCHITECTURE_H_
#define _ARCHITECTURE_H_

typedef int elem;

static const char *Default_name_output_file = "out.bin";

static const int Ver = 1;

static const int Sig = 'D' * 256 + 'K';

const int Cnt_reg = 16;

const int Max_cnt_cmd = (1 << 5);

const char Cmd_mask = (1 << 5) - 1;

enum Arg_type
{
    ARG_IMM = 1 << 5,
    ARG_REG = 1 << 6,
    ARG_RAM = 1 << 7,
};

#define DEF_REG(name, num)   \
    name = num,        


enum Regs
{
    #include "regs.h"
};


#undef DEF_REG



#define DEF_CMD(name, num, ...)      \              
    CMD_##name = (num),

#define DEF_CMD_JUMP(name, num, ...) \
    CMD_##name = (num),


enum Assembler_commands
{
    #include "cmd.h"
};

#undef DEF_CMD
#undef DEF_CMD_JUMP



#define USE_TYPE "d"               //<- specifier character to print/scan elem


#endif //#endif _ARCHITECTURE_H_