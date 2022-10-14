#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_


#include "assembler_config.h"
#include "labels.h"

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

enum Assembler_commands
{
    CMD_HLT     = 0,

    CMD_PUSH    = 1,
    CMD_IN      = 2,

    CMD_ADD     = 3,
    CMD_SUB     = 4, 
    CMD_MUT     = 5,
    CMD_DIV     = 6,
    
    CMD_LABLE   = 7,

    CMD_JUMP    = 8,

    CMD_JA      = 9, 
    CMD_JAE     = 10,
    CMD_JB      = 11,
    CMD_JBE     = 12,
    CMD_JL      = 13,
    CMD_JM      = 14,

    CMD_POP    = 16,

    CMD_OUT     = 17,
};


struct Asm_struct
{
    int cnt_bytes = 0;
    unsigned char *code = nullptr;

    int cnt_labels = 0;
    Label* labels = nullptr;
};


const int Maxbuf = 20;


int Convert_operations (int fdin);

#endif  //#endif _ASSEMBLER_H_