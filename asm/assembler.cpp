#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../architecture/dsl.h"

#include "../src/process_text/process_text.h"
#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"

#include "assembler.h"

static int Asm_struct_ctor (Asm_struct *asmst, int code_size);

static int Asm_struct_dtor (Asm_struct *asmst);

static int Get_convert_commands (Text_info *commands_line, Asm_struct *asmst);

static int Def_args (const char *str_args, const int len_str, 
                     Asm_struct *asmst, unsigned char cmd);

static int Def_jump_argument (Asm_struct *asmst, char *name_label);

static int Write_convert_file (const Asm_struct *asmst, const char *output_file);



#define SET_NUM_CMD(code, cmd, shift)       \
    do{                                     \
        *code  = cmd;                       \
         code += shift;                     \                              
    }while (0)

#define SET_ARGS(code, args, shift)         \
    do{                                     \
        *(elem*) code  = args;              \
                 code += shift;             \         
    }while (0)


//======================================================================================

#define DEF_CMD(name, num, arg, ...)                                                            \
                                                                                                \
        asmst->cmd_hash_tabel[num] = Get_str_hash (#name);                                      \        


#define DEF_CMD_JUMP(name, num, ...)                                                            \ 
                                                                                                \
        asmst->cmd_hash_tabel[num] = Get_str_hash (#name);                                      \


static int Asm_struct_ctor (Asm_struct *asmst, int code_size)
{
    assert (asmst != nullptr && "asmst is nullptr");

    ASM_CODE = (unsigned char*) calloc (code_size, sizeof (elem));

    if (Check_nullptr (ASM_CODE))
    {
        Log_report ("Memory was not allocated for the array of commands\n");
        Err_report ();
        return ST_ASM_CTOR_ERR;
    }

    asmst->cnt_bytes = 0;

    if (Ctor_label_tabel (&asmst->label_table))
    {
        Log_report ("Error ctor Label_tabel structure\n");
        Err_report ();
        return ST_ASM_CTOR_ERR;
    }

    asmst->cur_bypass = 0;

    if (Clear_data ((unsigned char*) asmst->cmd_hash_tabel, 
                             sizeof (asmst->cmd_hash_tabel)))
    {
        Log_report ("Command table cleanup error\n");
        return ST_ASM_CTOR_ERR;
    }
    
    #include "../architecture/cmd.h"

    return 0;
}

#undef DEF_CMD
#undef DEF_CMD_JUMP

//======================================================================================


static int Asm_struct_dtor (Asm_struct *asmst)
{
    assert (asmst != nullptr && "asmst is nullptr");

    asmst->cnt_bytes = -1;

    if (Check_nullptr (ASM_CODE)){
        Log_report ("Memory has not been allocated yet\n");
        Err_report ();
    }
    else
        free (ASM_CODE);

    if (Dtor_label_tabel (&asmst->label_table))
    {
        Log_report ("Error dtor Label_tabel structure\n");
        Err_report ();

        return ST_ASM_CTOR_ERR;
    }

    asmst->cur_bypass = -1;

    if (Clear_data ((unsigned char*) asmst->cmd_hash_tabel, 
                             sizeof (asmst->cmd_hash_tabel)))
    {
        Log_report ("Command table cleanup error\n");
        return ST_ASM_CTOR_ERR;
    }

    return 0;
}

//======================================================================================

int Convert_operations (int fdin, const char *output_file)
{
    assert (fdin >= 0 && "descriptor on file is negative number");

    Text_info commands_line = {};

    if (Text_read (fdin, &commands_line))
    {
        Log_report ("An error occurred while reading into the buffer\n");
        Err_report ();

        return READ_TO_BUFFER_ERR;
    }

    if (commands_line.cnt_lines == 0)
    {
        Log_report ("There are no commands in the input file\n");
        Err_report ();

        return EMPTY_FILE_ERR;
    }

    Asm_struct asmst = {};

    if (Asm_struct_ctor (&asmst, commands_line.cnt_lines))
    {
        Log_report ("Structure creation error Asm_struct\n");
        Err_report ();

        return ST_ASM_CTOR_ERR;
    }

    asmst.cur_bypass = FIRST; 
    asmst.cnt_bytes = Get_convert_commands (&commands_line, &asmst);

    if (asmst.cnt_bytes <= 0) 
    {
        Log_report ("Command convert error on FIRST command traversal\n");
        Err_report ();

        return CONVERT_COMMAND_ERR;
    }

    asmst.cur_bypass = SECOND; 
    asmst.cnt_bytes = Get_convert_commands (&commands_line, &asmst);

    if (asmst.cnt_bytes <= 0)
    {
        Log_report ("Command convert error on SECOND command traversal\n");
        Err_report ();

        return CONVERT_COMMAND_ERR;
    }


    if (Write_convert_file (&asmst, output_file))
    {
        Log_report ("An error occurred while writing to the output file\n");
        Err_report ();

        return CREAT_CONVERT_FILE_ERR;
    }

    if (Asm_struct_dtor (&asmst))
    {
        Log_report ("Error in file destructor\n");
        Err_report ();

        return ST_ASM_DTOR_ERR;
    }

    if (Free_buffer (&commands_line))
    {
        Log_report ("The buffer has not been cleared\n");
        Err_report ();

        return FREE_BUF_ERR;
    }

    return 0;
}

//======================================================================================

#define DEF_CMD(name, num, arg, ...)                                                            \
                                                                                                \
        if  (cur_line_hash == asmst->cmd_hash_tabel[num]) {                                     \
                                                                                                \
            if (arg == 1){                                                                      \
                ip_line++;                                                                      \
                                                                                                \                                                      
                if (Def_args (commands_line->lines[ip_line].str,                                \
                          commands_line->lines[ip_line].len_str, asmst, (unsigned char)num))    \
                {                                                                               \
                    Log_report ("Argument definition error\n");                                 \
                    Err_report ();                                                              \
                    return CONVERT_COMMAND_ERR;                                                 \
                }                                                                               \
                                                                                                \
            } else{                                                                             \
                SET_NUM_CMD (ASM_CODE, num, sizeof (char));                                     \
            }                                                                                   \
        }                                                                                       \  
        else                                                                                  


#define DEF_CMD_JUMP(name, num, ...)                                                                    \ 
                                                                                                        \                                      
        if (cur_line_hash == asmst->cmd_hash_tabel[num])                                                \
        {                                                                                               \
            SET_NUM_CMD (ASM_CODE, num, sizeof (char));                                                 \
                                                                                                        \
            ip_line++;                                                                                  \                                                                      
            char *name_label = commands_line->lines[ip_line].str;                                       \
                                                                                                        \
            if (Def_jump_argument (asmst, name_label))                                                  \
            {                                                                                           \
                Log_report ("Argument definition error\n");                                             \
                Err_report ();                                                                          \
                return CONVERT_COMMAND_ERR;                                                             \
            }                                                                                           \
        }                                                                                               \                                                                                       
        else              

static int Get_convert_commands (Text_info *commands_line, Asm_struct *asmst)
{
    assert (commands_line != nullptr && "commands line is nullptr");
    assert (asmst != nullptr && "asmst is nullptr");

    unsigned char *ptr_beg_code = asmst->code;

    int ip_line = 0;

    while (ip_line < commands_line->cnt_lines)
    {
        char    *cur_line      = commands_line->lines[ip_line].str;
        int      cur_len       = commands_line->lines[ip_line].len_str;
    
        if (cur_len == 0 || cur_line[0] == '\n')
        {
            ip_line++;
            continue;
        }

        if (cur_len >= 2 && cur_line [0] == '/' && cur_line [1] == '/')
        {
            ip_line++;
            continue;
        }

        int64_t cur_line_hash = Get_str_hash (cur_line);

        if (cur_line[cur_len - 1] == ':')
        {
            char *name_label = commands_line->lines[ip_line].str;
            name_label [cur_len - 1] = '\0';
    
            if (Check_reserved_name (name_label, asmst->cmd_hash_tabel))
            {
                Log_report ("The label is trying to name a reserved command\n");
                Err_report ();
                return CONVERT_COMMAND_ERR;
            }

            int ip_jump = Find_label (&asmst->label_table, name_label);

            if (ip_jump == Not_init_label)
            {
                if (Check_cnt_labels (&asmst->label_table))
                    if (Recalloc_cnt_labels (&asmst->label_table))
                    {
                        Log_report ("Increasing the number of possible"
                                    "labels encountered an error\n");
                        Err_report ();

                        return CONVERT_COMMAND_ERR;
                    }

                Label_init (asmst->label_table.labels + asmst->label_table.cnt_labels, 
                            asmst->code - ptr_beg_code, name_label, asmst->cur_bypass);
                asmst->label_table.cnt_labels++;
            }

            else
            {
                if ((asmst->label_table.labels + ip_jump)->bypass == asmst->cur_bypass)
                {
                    Log_report ("Redefining the label: %s\n", name_label);
                    Err_report ();

                    return REDEF_LABEL_ERR;
                }
            }

            name_label [cur_len - 1] = ':';

            ip_line++;
            continue;
        }

        #include "../architecture/cmd.h"
        
        /*else*/
        {
            Log_report ("Unknown program entered: |%s|\n", cur_line);
            Err_report ();

            return UNKNOWN_COM_ERR;
        }

        ip_line++;
    }

    asmst->cnt_bytes = asmst->code - ptr_beg_code;

    asmst->code = ptr_beg_code;

    return asmst->cnt_bytes;
}

#undef DEF_CMD
#undef DEF_CMD_JUMP

//======================================================================================

static int Def_args 
        (const char *str_args, const int len_str, Asm_struct *asmst, unsigned char cmd)
{
    assert (str_args  != nullptr && "string is nullptr");
    assert (asmst     != nullptr && "asmst  is nullptr");

    char str[len_str + 1] = {};
    if (!strncpy (str, str_args, len_str))
    {
        Log_report ("string copy failure\n");
        Err_report ();

        return DEF_ARGS_ERR;
    }

    if (strchr (str, '[') || strchr (str, ']'))
    {
        if (str[0] != '[' || str[len_str - 1] != ']')
        {
            Log_report ("Incorrect entry command: %s\n", str);
            Err_report ();

            return DEF_ARGS_ERR;
        }

        for (int ch = 1; ch < len_str - 1; ch++)
        {
            if (str[ch] == '[' || str[ch] == ']')
            {
                Log_report ("Incorrect entry command: %s\n", str);
                Err_report ();

                return DEF_ARGS_ERR;
            }
        }

        cmd |= ARG_RAM;
    }

    unsigned char arg_reg = 0;
    elem          arg_num = 0;

    char *cur_lex = strtok (str, "[]+ "); 
    while (cur_lex != nullptr)
    {
        if (cur_lex[0] == 'r' && cur_lex[2] == 'x' && cur_lex[1] - 'a' <= Cnt_reg)
        {
            arg_reg = (unsigned char) (cur_lex[1] - 'a');
            cmd |= ARG_REG;
        }

        else if (!strcmp (cur_lex, "rbp"))
        {
            arg_reg = (unsigned char) RBP;
            cmd |= ARG_REG;
        }
        
        else 
        {
            if (Check_num (cur_lex))    
            {
                arg_num = (elem) atof(cur_lex);
                cmd |= ARG_IMM;
            }

            else
            {
                Log_report ("Incorrect entry command: %s\n", cur_lex);
                Err_report ();

                return DEF_ARGS_ERR; 
            }
            
        }

        cur_lex = strtok(nullptr, "]+ ");
    }

    SET_NUM_CMD (ASM_CODE, cmd, sizeof (char));

    if (cmd & ARG_RAM)
    {
        if (cmd & ARG_IMM)
            SET_ARGS (ASM_CODE, arg_num, sizeof (elem));

        if (cmd & ARG_REG)
            SET_NUM_CMD (ASM_CODE, arg_reg, sizeof (char));
    }

    else
    {
        if ((cmd & ARG_IMM) && (cmd & ARG_REG))
        {
            Log_report ("Incorrect entry command\n");
            Err_report ();

            return DEF_ARGS_ERR; 
        }

        if (cmd & ARG_IMM)
            SET_ARGS (ASM_CODE, arg_num, sizeof (elem));

        if (cmd & ARG_REG)
            SET_NUM_CMD (ASM_CODE, arg_reg, sizeof (char));
    }
    
    return 0;
}

//======================================================================================

static int Def_jump_argument (Asm_struct *asmst, char *name_label)
{
    assert (asmst     != nullptr && "asmst is nullptr");
    assert (name_label != nullptr && "name_label is nullptr");

    if (Check_reserved_name (name_label, asmst->cmd_hash_tabel))                                
    {                                                                                           
        Log_report ("The label is trying to name a reserved command:"                           
                                                    "%s\n", name_label);                          
        Err_report ();                                                                                                                                       
        return CONVERT_COMMAND_ERR;                                                             
    }                                                                                           
                                                                  
    int ip_jump = Find_label (&asmst->label_table, name_label);                                                                                                                                
    if (ip_jump == Not_init_label && asmst->cur_bypass == SECOND)                                      
    {                                                                                           
        Log_report ("Undefined label: %s\n", name_label);                                                       
        Err_report ();                                                                          
                                                                                                
        return CONVERT_COMMAND_ERR;                                                             
    }                                                                                           
                                                                                                        
    if (ip_jump == Not_init_label)                                                              
        SET_ARGS (asmst->code, (elem) ip_jump, sizeof (elem));                                            
    else                                                                                        
        SET_ARGS (asmst->code, (asmst->label_table.labels + ip_jump)->ptr_jump, sizeof (elem)); 
    return 0;  
}

//======================================================================================

static int Write_convert_file (const Asm_struct *asmst, const char *output_file)
{
    assert (asmst != nullptr && "asmst is nullptr");

    FILE *fpout = Open_file_ptr (output_file, "wb");

    if (Check_nullptr (fpout))
    {
        Log_report ("Descriptor converted file did't create\n");
        Err_report ();

        return CREAT_CONVERT_FILE_ERR;
    }

    fwrite (&Sig, sizeof (int), 1, fpout);

    fwrite (&Ver, sizeof (int), 1, fpout);

    fwrite (&(asmst->cnt_bytes), sizeof(int), 1, fpout);

    fwrite (ASM_CODE, sizeof (unsigned char), asmst->cnt_bytes, fpout);
    

    if (Close_file_ptr (fpout))
    {
        Log_report ("Converted file did't close");
        Err_report ();

        return CREAT_CONVERT_FILE_ERR;
    }

    return 0;
}

//======================================================================================