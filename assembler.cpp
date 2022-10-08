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

static int Get_convert_command (Text_info *commands_line, int *code);

static int Write_convert_file (File_info *file_info);

int File_info_ctor (File_info *file_info)
{
    assert (file_info != nullptr && "file info is nullptr");

    file_info->code = nullptr;

    file_info->sig     = SIG;
    file_info->ver     = VER;

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

    if (file_info->code == (int*) POISON)
    {
        Log_report ("Memory has been freed\n");
        return 0;
    }

    free (file_info->code);

    file_info->code = (int*) POISON;

    file_info->sig     = POISON;
    file_info->ver     = POISON;

    file_info->cnt_com = -1;

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

    File_info output_file_info = {};
    File_info_ctor (&output_file_info);

    output_file_info.code = (int*) calloc (commands_line.cnt_lines, sizeof (int));
    
    if (output_file_info.code == nullptr)
    {
        Log_report ("Memory was not allocated for the array of commands\n");
        return ERR_MEMORY_ALLOC;
    }

    output_file_info.cnt_com = Get_convert_command (&commands_line, output_file_info.code);

    if (output_file_info.cnt_com <= 0)
    {
        Log_report ("The file was not converted because an unknown"
                    "command was encountered in the source file\n");
        return UNKNOWN_COM_ERR;
    }
    
    Write_convert_file (&output_file_info);

    File_info_dtor (&output_file_info);

    Free_buffer (&commands_line);
    
    #ifdef USE_LOG
        
        if (Close_logs_file (fp_logs))
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}

static int Get_convert_command (Text_info *commands_line, int *code)
{
    assert (commands_line != nullptr && "commands line is nullptr");
    assert (code != nullptr && "code is nullptr");

    int ip_com = 0;
    for (int id_line = 0; id_line < commands_line->cnt_lines; id_line++)
    {
        char *cur_line = commands_line->lines[id_line].str;

        char cmd[MAXBUF];
        int read_ch = 0;

        sscanf (cur_line, "%s %n", cmd, &read_ch);

        if (cmd[0] == '\0')
        {
            continue;
        }
        
        else if (strcmpi (cmd, "push") == 0)
        {
            code[ip_com++] = CMD_PUSH;

            int val = 0;
            sscanf (cur_line + read_ch, "%d", &val);

            code[ip_com++] = val;
        }
        
        else if (strcmpi (cmd, "add") == 0)
        {
            code[ip_com++] = CMD_ADD;
        }
        
        else if (strcmpi (cmd, "sub") == 0)
        {
            code[ip_com++] = CMD_SUB;
        }
        
        else if (strcmpi (cmd, "mult") == 0)
        {
            code[ip_com++] = CMD_MUT;
        }
        
        else if (strcmpi (cmd, "div") == 0)
        {
            code[ip_com++] = CMD_DIV;
        }
        
        else if (strcmpi (cmd, "out") == 0)
        {
            code[ip_com++] = CMD_OUT;
        }
        
        else if (strcmpi (cmd, "hlt") == 0)
        {
            code[ip_com++] = CMD_HLT;
        }
        
        else
        {
            Log_report ("Unknown program entered\n");
            return UNKNOWN_COM_ERR;
        }
    
        cmd[0] = '\0';
    }

    return ip_com;
}

static int Write_convert_file (File_info *file_info)
{
    assert (file_info != nullptr && "File info is nullptr");

    int fdout = creat ("convert_input.bin", S_IRWXU);

    if (fdout < 0)
    {
        Log_report ("Descriptor converted file did't create\n");
        return CREAT_CONVERT_FILE_ERR;
    }

    write (fdout, file_info->sig, sizeof (file_info->sig));
    write (fdout, file_info->ver, sizeof (file_info->ver));  

    write (fdout, &file_info->cnt_com, sizeof(int));

    write (fdout, file_info->code, sizeof (int) * file_info->cnt_com);
    
    if (close (fdout))
    {
        Log_report ("Converted file did't close");
        return CREAT_CONVERT_FILE_ERR;
    }
    
    return 0;
}