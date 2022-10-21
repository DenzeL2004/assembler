#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "src/process_text/process_text.h"
#include "src/Generals_func/generals.h"
#include "src/log_info/log_errors.h"

#include "assembler.h"

static FILE *fp_logs = stderr;



static int Asm_struct_ctor (Asm_struct *asmst, int code_size);

static int Asm_struct_dtor (Asm_struct *asmst);

static int Get_convert_commands (Text_info *commands_line, Asm_struct *asmst, 
                                const int cur_bypass);

static int Def_args (const char *str_args, const int len_str, unsigned char *code, unsigned char cmd);

static int Write_convert_file (const Asm_struct *asmst, const char *output_file);


#define Set_num_cmd(code, cmd, shift)       \
    do{                                     \
        *code = cmd;                        \
         code += shift;                     \                              
    }while (0)

#define Set_args(code, args, shift)         \
    do{                                     \
        *(elem*) code = args;               \
                 code += shift;             \         
    }while (0)


//======================================================================================

#define DEF_CMD(name, num, arg, ...)                                                            \
                                                                                                \
        asmst->cmd_code_tabel[num] = Get_str_code (#name);                                      \      


#define DEF_CMD_JUMP(name, num, ...)                                                            \ 
                                                                                                \
        asmst->cmd_code_tabel[num] = Get_str_code (#name);                                      \


static int Asm_struct_ctor (Asm_struct *asmst, int code_size)
{
    assert (asmst != nullptr && "asmst is nullptr");

    asmst->code = (unsigned char*) calloc (code_size, sizeof (elem));

    if (Check_nullptr (asmst->code))
    {
        Log_report ("Memory was not allocated for the array of commands\n");
        return ST_ASM_CTOR_ERR;
    }

    asmst->cnt_bytes = 0;

    asmst->cnt_labels      = 0;
    asmst->label_capacity  = Init_number_labels;
    asmst->labels = (Label*) calloc (Init_number_labels, sizeof(Label));

    if (asmst->labels == nullptr)
    {
        Log_report ("Memory was not allocated for the array of labels\n");
        return ST_ASM_CTOR_ERR;
    }

    for (int ip_com = 0; ip_com < Max_cnt_cmd; ip_com++)
        asmst->cmd_code_tabel[ip_com] = 0;
    
    #include "cmd.h"

    return 0;
}

#undef DEF_CMD
#undef DEF_CMD_JUMP
//======================================================================================


static int Asm_struct_dtor (Asm_struct *asmst)
{
    assert (asmst != nullptr && "asmst is nullptr");

    asmst->cnt_bytes = -1;

    if (Check_nullptr (asmst->code))
        Log_report ("Memory has not been allocated yet\n");
    else
        free (asmst->code);

    if (Check_nullptr (asmst->labels))
        Log_report ("An error occurred in stack deconstruction\n");
    else
        free (asmst->labels);

    asmst->cnt_labels       = -1;
    asmst->label_capacity   = -1;

    for (int ip_com = 0; ip_com < Max_cnt_cmd; ip_com++)
        asmst->cmd_code_tabel[ip_com] = 0;

    return 0;
}

//======================================================================================

int Convert_operations (int fdin, const char *output_file)
{
    assert (fdin >= 0 && "descriptor on file is negative number");

    #ifdef USE_LOG
        
        fp_logs = Open_logs_file ();
        
        if (fp_logs == stderr)
            return OPEN_FILE_LOG_ERR;

    #endif 

    Text_info commands_line = {};


    if (Text_read (fdin, &commands_line))
    {
        Log_report ("An error occurred while reading into the buffer\n");
        return READ_TO_BUFFER_ERR;
    }

    if (commands_line.cnt_lines == 0)
    {
        Log_report ("There are no commands in the input file\n");
        return EMPTY_FILE_ERR;
    }

    Asm_struct asmst = {};

    if (Asm_struct_ctor (&asmst, commands_line.cnt_lines))
    {
        Log_report ("Structure creation error Asm_struct\n");
        return ST_ASM_CTOR_ERR;
    }

    asmst.cnt_bytes = Get_convert_commands (&commands_line, &asmst, FIRST);

    if (asmst.cnt_bytes <= 0) 
    {
        Log_report ("Command convert error on FIRST command traversal\n");
        return CONVERT_COMMAND_ERR;
    }


    asmst.cnt_bytes = Get_convert_commands (&commands_line, &asmst, SECOND);

    if (asmst.cnt_bytes <= 0)
    {
        Log_report ("Command convert error on SECOND command traversal\n");
        return CONVERT_COMMAND_ERR;
    }

    if (Write_convert_file (&asmst, output_file))
    {
        Log_report ("An error occurred while writing to the output file\n");
        return CREAT_CONVERT_FILE_ERR;
    }

    if (Asm_struct_dtor (&asmst))
    {
        Log_report ("Error in file destructor\n");
        return ST_ASM_DTOR_ERR;
    }

    if (Free_buffer (&commands_line))
    {
        Log_report ("The buffer has not been cleared\n");
        return FREE_BUF_ERR;
    }

    #ifdef USE_LOG
        
        if (Close_logs_file (fp_logs))
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}

//======================================================================================

#define DEF_CMD(name, num, arg, ...)                                                            \
                                                                                                \
        if  (strcmpi (cur_line, #name) == 0) {                                                  \
                                                                                                \
            if (arg == 1){                                                                      \
                ip_line++;                                                                      \
                                                                                                \                                                      
                int shift = Def_args (commands_line->lines[ip_line].str,                        \
                            commands_line->lines[ip_line].len_str, code, (unsigned char)num);   \
                                                                                                \   
                code += sizeof (char) * shift;                                                  \
                                                                                                \
            } else                                                                              \
                Set_num_cmd (code, num, sizeof (char));                                         \
        }                                                                                       \  
        else                                                                                  


#define DEF_CMD_JUMP(name, num, ...)                                                                    \ 
                                                                                                        \                                      
        if (strcmpi (cur_line, #name) == 0)                                                             \
        {                                                                                               \
            Set_num_cmd (code, num, sizeof (char));                                                     \
                                                                                                        \
            ip_line++;                                                                                  \                                                                      
            char *name_label = commands_line->lines[ip_line].str;                                       \
                                                                                                        \
            if (Check_reserved_name (name_label, asmst->cmd_code_tabel))                                \
            {                                                                                           \
                Log_report ("The label is trying to name a reserved command\n");                        \
                return CONVERT_COMMAND_ERR;                                                             \
            }                                                                                           \
                                                                                                        \
            int ip_jump = Find_label (asmst->labels, name_label, asmst->cnt_labels);                    \
                                                                                                        \
            if (ip_jump == Not_init_label && cur_bypass == SECOND)                                      \
            {                                                                                           \
                Log_report ("Undefined label\n");                                                       \
                return CONVERT_COMMAND_ERR;                                                             \
            }                                                                                           \
                                                                                                        \       
            if (ip_jump == Not_init_label)                                                              \
                Set_args (code, ip_jump, sizeof (elem));                                                 \
            else                                                                                        \
                Set_args (code, (asmst->labels + ip_jump)->ptr_jump, sizeof (elem));                     \
        }                                                                                               \                                                                                       
        else              

static int Get_convert_commands (Text_info *commands_line, Asm_struct *asmst, 
                                const int cur_bypass)
{
    assert (commands_line != nullptr && "commands line is nullptr");
    assert (asmst != nullptr && "asmst is nullptr");

    unsigned char *code = asmst->code;
    unsigned char *ptr_beg_code = code;

    int ip_line = 0;

    while (ip_line < commands_line->cnt_lines)
    {
        char *cur_line = commands_line->lines[ip_line].str;
        int   cur_len  = commands_line->lines[ip_line].len_str;
        
        if (cur_len == 0)
        {
            ip_line++;
            continue;
        }

        if (cur_line[cur_len - 1] == ':')
        {
            char *name_label = commands_line->lines[ip_line].str;
            name_label [cur_len - 1] = '\0';

            if (Check_reserved_name (name_label, asmst->cmd_code_tabel))
            {
                Log_report ("The label is trying to name a reserved command\n");
                return CONVERT_COMMAND_ERR;
            }

            int ip_jump = Find_label (asmst->labels, name_label, asmst->cnt_labels);

            if (ip_jump == Not_init_label)
            {
                if (Check_cnt_labels (asmst->cnt_labels, asmst->label_capacity))
                    if (Recalloc_cnt_labels (asmst->labels, &asmst->label_capacity))
                    {
                        Log_report ("Increasing the number of possible"
                                    "labels encountered an error\n");
                        return CONVERT_COMMAND_ERR;
                    }

                Label_init (asmst->labels + asmst->cnt_labels, 
                            code - ptr_beg_code, name_label, cur_bypass);

                asmst->cnt_labels++;
            }

            else
            {
                if ((asmst->labels + ip_jump)->bypass == cur_bypass)
                {
                    Log_report ("Redefining the label\n");
                    return REDEF_LABEL_ERR;
                }
            }

            name_label [cur_len - 1] = ':';
        }

        else 

        #include "cmd.h"
        
        /*else*/
        {
            Log_report ("Unknown program entered\n");
            return UNKNOWN_COM_ERR;
        }

        ip_line++;
    }
    
    

    return (code - ptr_beg_code);
}

#undef DEF_CMD
#undef DEF_CMD_JUMP
//======================================================================================

static int Def_args (const char *str_args, const int len_str, unsigned char *code, unsigned char cmd)
{
    assert (str_args  != nullptr && "string is nullptr");
    assert (code != nullptr && "code is nullptr");

    char str[len_str + 1] = {};
    if (!strncpy (str, str_args, len_str))
    {
        Log_report ("string copy failure\n");
        return DEF_ARGS_ERR;
    }

    if (strchr (str, '[') || strchr (str, ']'))
    {
        if (str[0] != '[' || str[len_str - 1] != ']')
        {
            Log_report ("Incorrect entry command\n");
            return DEF_ARGS_ERR;
        }

        for (int ch = 1; ch < len_str - 1; ch++)
        {
            if (str[ch] == '[' || str[ch] == ']')
            {
                Log_report ("Incorrect entry command\n");
                return DEF_ARGS_ERR;
            }
        }

        cmd |= ARG_RAM;
    }

    unsigned char arg_reg = 0;
    int           arg_num = 0;

    char *cur_lex = strtok (str, "[]+ "); 
    while (cur_lex != nullptr)
    {
        if (cur_lex[0] == 'r' && cur_lex[2] == 'x' && cur_lex[1] - 'a' <= Cnt_reg)
        {
            arg_reg = cur_lex[1] - 'a';
            cmd |= ARG_REG;
        }
        
        else 
        {
            if (Check_num (cur_lex))    
            {
                arg_num = atoi(cur_lex);
                cmd |= ARG_IMM;
            }

            else
            {
                Log_report ("Incorrect entry command\n");
                return DEF_ARGS_ERR; 
            }
            
        }

        cur_lex = strtok(nullptr, "]+ ");
    }

    int shift = 0;

    Set_num_cmd (code, cmd, sizeof (char));
    shift += sizeof (char);

    if (cmd & ARG_IMM)
    {
        Set_args (code, arg_num, sizeof (elem));
        shift += sizeof (elem);
    }  

    if (cmd & ARG_REG)
    {
        Set_num_cmd (code, arg_reg, sizeof (char));
        shift += sizeof (char);
    }

    if (shift <= 0)                                                                 
    {                                                                               
        Log_report ("Argument definition error\n");                                 
        return CONVERT_COMMAND_ERR;                                                 
    }
    
    return shift;
}

//======================================================================================

static int Write_convert_file (const Asm_struct *asmst, const char *output_file)
{
    assert (asmst != nullptr && "asmst is nullptr");

    int fdout = creat (output_file, S_IRWXU);

    if (fdout < 0)
    {
        Log_report ("Descriptor converted file did't create\n");
        return CREAT_CONVERT_FILE_ERR;
    }


    write (fdout, &Sig, sizeof (int));

    write (fdout, &Ver, sizeof (int)); 

    write (fdout, &(asmst->cnt_bytes), sizeof(int));

    write (fdout, asmst->code, sizeof (unsigned char) * asmst->cnt_bytes);

    if (close (fdout))
    {
        Log_report ("Converted file did't close");
        return CREAT_CONVERT_FILE_ERR;
    }
    
    return 0;
}

//======================================================================================