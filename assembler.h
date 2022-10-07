#ifndef _ASSEMBLER_
#define _ASSEMBLER_

//#define BIN_REPRESENT

enum Assembler_errors
{
    CREAT_CONVERT_FILE_ERR = -1,
    CLOSE_CONVERT_FILE_ERR = -2,

    READ_TO_BUFFER_ERR     = -3,
    
    EMPTY_FILE_ERR         = -4,

    UNKNOWN_COM_ERR        = -5
};

enum Assembler_commands
{
    CMD_HLT     = 0,

    CMD_ADD     = 1,

    CMD_SUM     = 2,
    CMD_SUB     = 3, 
    CMD_MULT    = 4,
    CMD_DIV     = 5,
    
    CMD_OUT     = 6,
};

struct File_info
{
    char* sig = nullptr;
    char* ver = nullptr;
    int cnt_com = 0;
};

int Convert_operations (int fdin);

#endif