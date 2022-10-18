#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_


#include "assembler_config.h"
#include "labels.h"

static const char *Name_input_file = "input.txt";

static const char *Name_output_file = "out.bin";

enum Assembler_errors
{
    CREAT_CONVERT_FILE_ERR = -1,
    CLOSE_CONVERT_FILE_ERR = -2,

    READ_TO_BUFFER_ERR     = -3,
    
    EMPTY_FILE_ERR         = -4,

    UNKNOWN_COM_ERR        = -5,

    CONVERT_COMMAND_ERR    = -6, 

    ST_ASM_DTOR_ERR        = -9,
    ST_ASM_CTOR_ERR        = -10,

    DEF_ARGS_ERR           = -11,

    FILE_INFO_DTOR_ERR     = -7,

    FREE_BUF_ERR           = -8, 
};

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


struct Asm_struct
{
    int cnt_bytes = 0;
    unsigned char *code = nullptr;

    int cnt_labels = 0;
    Label* labels = nullptr;
};


const int Maxbuf = 20;


int Convert_operations (int fdin, const char *output_file);

#endif  //#endif _ASSEMBLER_H_