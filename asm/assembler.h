#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_


#include "../architecture/architecture.h"
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


struct Asm_struct
{
    int cnt_bytes = 0;
    unsigned char *code = nullptr;

    Label_table label_table = {};

    unsigned int cmd_hash_tabel[Max_cnt_cmd];

    char cur_bypass = 0;
};


int Convert_operations (int fdin, const char *output_file);

#endif  //#endif _ASSEMBLER_H_