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

    CMD_OUT     = 9,
};

struct File_info
{
    int cnt_com = 0;
    int *code = nullptr;
};

struct Asm_struct
{
    int cnt_com = 0;
    int *code = nullptr;

    int cnt_labels = 0;
    Label* labels = nullptr;
};



const int Maxbuf = 20;



int Convert_operations (int fdin);

int File_info_ctor (File_info *file_info);

int File_info_dtor (File_info *file_info);

#endif  //#endif _ASSEMBLER_H_