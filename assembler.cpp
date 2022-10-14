#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "process_text.h"
#include "../Generals_func/generals.h"
#include "../Logs/log_errors.h"

#include "assembler.h"



static FILE *fp_logs = stderr;



static int Get_convert_command (Text_info *commands_line, Asm_struct *asmst, 
                                const int cur_bypass);

static int Write_convert_file (Asm_struct *asmst);

static int Asm_struct_ctor (Asm_struct *asmst, int code_size);

static int Asm_struct_dtor (Asm_struct *asmst);


#define Set_num_cmd(code, cmd, shift)       \
    do{                                      \
        *code = cmd;                         \
         code += shift;                    \                              
    }while (0)

#define Set_args(code, args, shift)         \
    do{                                      \
        *(int*) code = args;                 \
                code += shift;               \         
    }while (0)


int File_info_ctor (File_info *file_info)
{
    assert (file_info != nullptr && "file info is nullptr");

    file_info->code = nullptr;

    file_info->cnt_com = 0;

    return 0;
}

int File_info_dtor (File_info *file_info)
{
    assert (file_info != nullptr && "file info is nullptr");

    if (file_info->code == nullptr)
    {
        Log_report ("Memory has not been allocated yet\n");
        return 0;
    }

    if (file_info->code == (unsigned char*) POISON)
    {
        Log_report ("Memory has been freed\n");
        return 0;
    }

    free (file_info->code);

    file_info->code = (unsigned char*) POISON;

    file_info->cnt_com = -1;

    return 0;
}

static int Asm_struct_ctor (Asm_struct *asmst, int code_size)
{
    assert (asmst != nullptr && "asmst is nullptr");

    asmst->code = (unsigned char*) calloc (code_size * 2, sizeof (int));

    if (asmst->code == nullptr)
    {
        Log_report ("Memory was not allocated for the array of commands\n");
        return ERR_MEMORY_ALLOC;
    }

    asmst->cnt_bytes = 0;

    asmst->cnt_labels = 0;
    asmst->labels = (Label*) calloc (Max_cnt_labels, sizeof(Label));

    if (asmst->labels == nullptr)
    {
        Log_report ("Memory was not allocated for the array of labels\n");
        return ERR_MEMORY_ALLOC;
    }

    return 0;
}


static int Asm_struct_dtor (Asm_struct *asmst)
{
    assert (asmst != nullptr && "asmst is nullptr");

    asmst->cnt_bytes = -1;

    asmst->cnt_labels = -1;

    if (asmst->code   != nullptr) free (asmst->code);
    if (asmst->labels != nullptr) free (asmst->labels);

    return 0;
}


int Convert_operations (int fdin)
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

    asmst.cnt_bytes = Get_convert_command (&commands_line, &asmst, FIRST);

    if (asmst.cnt_bytes <= 0) 
    {
        Log_report ("Command convert error on FIRST command traversal\n");
        return CONVERT_COMMAND_ERR;
    }


    asmst.cnt_bytes = Get_convert_command (&commands_line, &asmst, SECOND);

    if (asmst.cnt_bytes <= 0)
    {
        Log_report ("Command convert error on SECOND command traversal\n");
        return CONVERT_COMMAND_ERR;
    }


    if (Write_convert_file (&asmst))
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

static int Get_convert_command (Text_info *commands_line, Asm_struct *asmst, 
                                const int cur_bypass)
{
    assert (commands_line != nullptr && "commands line is nullptr");
    assert (asmst != nullptr && "asmst is nullptr");

    char cmd[Maxbuf] = {'0'};
    int ip_cmd = 0;

    unsigned char *code = asmst->code;
    unsigned char *ptr_beg_code = code;

    for (int ip_line = 0; ip_line < commands_line->cnt_lines; ip_line++)
    {
        char *cur_line = commands_line->lines[ip_line].str;

        int read_ch = 0;

        if (sscanf (cur_line, "%s %n", cmd, &read_ch) !=  1)
        {
            Log_report ("Incorrect number of elements read\n");
            return CONVERT_COMMAND_ERR;
        }
        
        if (cmd[0] == ':')
        {
            int ip_jump = Find_label (asmst->labels, (cmd + 1), asmst->cnt_labels);

            if (ip_jump == -1)
            {
                Label_ctor (asmst->labels + asmst->cnt_labels, 
                            code - ptr_beg_code, (cmd + 1));
                (asmst->labels + asmst->cnt_labels)->bypass = cur_bypass;

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
        }

        else if (strcmpi (cmd, "jump") == 0) 
        {
            Set_num_cmd (code, CMD_JUMP, sizeof (char));

            char name_label[Maxbuf] = {'0'};

            if (sscanf (cur_line + read_ch, "%s", name_label) !=  1)
            {
                Log_report ("Incorrect number of elements read\n");
                return CONVERT_COMMAND_ERR;
            }

            int ip_jump = Find_label (asmst->labels, name_label, asmst->cnt_labels);

            if (ip_jump == -1) Set_args (code, ip_jump, sizeof (int));
            else               Set_args (code, (asmst->labels + ip_jump)->ptr_jump, sizeof (int));

        }
        
        else if (strcmpi (cmd, "push") == 0)
        {
            Set_num_cmd (code, CMD_PUSH, sizeof (char));

            int val = 0;

            if (sscanf (cur_line + read_ch, "%d", &val) != 1)
            {
                Log_report ("Incorrect number of elements read\n");
                return CONVERT_COMMAND_ERR;
            }

            Set_args (code, val, sizeof (int));
        }
        
        else if (strcmpi (cmd, "add") == 0)
        {
            Set_num_cmd (code, CMD_ADD, sizeof (char));
        }
        
        else if (strcmpi (cmd, "sub") == 0)
        {
            Set_num_cmd (code, CMD_SUB, sizeof (char));
        }
        
        else if (strcmpi (cmd, "mult") == 0)
        {
            Set_num_cmd (code, CMD_MUT, sizeof (char));
        }
        
        else if (strcmpi (cmd, "div") == 0)
        {
            Set_num_cmd (code, CMD_DIV, sizeof (char));
        }
        
        else if (strcmpi (cmd, "out") == 0)
        {
            Set_num_cmd (code, CMD_OUT, sizeof (char));
        }
        
        else if (strcmpi (cmd, "hlt") == 0)
        {
            Set_num_cmd (code, CMD_HLT, sizeof (char));
        }

        else if (strcmpi (cmd, "in") == 0)
        {
            Set_num_cmd (code, CMD_IN, sizeof (char));
        }
        
        else
        {
            Log_report ("Unknown program entered\n");
            return UNKNOWN_COM_ERR;
        }
    
        cmd[0] = '\0';
    }

    return (code - ptr_beg_code);
}

static int Write_convert_file (Asm_struct *asmst)
{
    assert (asmst != nullptr && "asmst is nullptr");

    int fdout = creat (name_output_file, S_IRWXU);

    if (fdout < 0)
    {
        Log_report ("Descriptor converted file did't create\n");
        return CREAT_CONVERT_FILE_ERR;
    }


    write (fdout, &Sig, sizeof (Sig));
    write (fdout, &Ver, sizeof (Ver)); 

    write (fdout, &(asmst->cnt_bytes), sizeof(int));

    write (fdout, asmst->code, sizeof (char) * asmst->cnt_bytes);
    
    if (close (fdout))
    {
        Log_report ("Converted file did't close");
        return CREAT_CONVERT_FILE_ERR;
    }
    
    return 0;
}