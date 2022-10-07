#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "process_text.h"
#include "../Generals_func/generals.h"
#include "../Logs/log_errors.h"
#include "assembler.h"

#define Get_convert_command(commands_line, code)                    \
            Get_convert_command_ (commands_line, code, fp_logs)

static int Get_convert_command_ (Text_info *commands_line, int *code, FILE *fp_logs);


#define Write_convert_file(code, file_info)                    \
            Write_convert_file_ (code, file_info, fp_logs)

static int Write_convert_file_ (int *code, File_info *file_info, FILE *fp_logs);

int Convert_operations (int fdin)
{
    #ifdef USE_LOG
        
        FILE *fp_logs = Open_logs_file ();
        
        if (fp_logs == nullptr)
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

    int *code = (int*) calloc (commands_line.cnt_lines, sizeof (int));
    
    if (code == nullptr)
    {
        Log_report ("Memory was not allocated for the array of commands\n");
        return ERR_MEMORY_ALLOC;
    }

    printf ("OFF\n");

    int cnt_com = Get_convert_command (&commands_line, code);

    if (cnt_com <= 0)
    {
        Log_report ("The file was not converted because an unknown"
                    "command was encountered in the source file\n");
        return UNKNOWN_COM_ERR;
    }

    File_info file_info = {};
    file_info.cnt_com = cnt_com;
    file_info.sig     = "DK";
    file_info.ver     = "1.0"; 
    
    Write_convert_file (code, &file_info);

    Free_buffer (&commands_line);
    
    #ifdef USE_LOG
        
        if (Close_logs_file (fp_logs))
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}

static int Get_convert_command_ (Text_info *commands_line, int *code, FILE *fp_logs)
{
    assert (commands_line != nullptr && "commands line is nullptr");
    assert (code != nullptr && "code is nullptr");

    int ip_com = 0;
    for (int id_line = 0; id_line < commands_line->cnt_lines; id_line++)
    {
        char *cur_line = commands_line->lines[id_line].str;

        char cmd[1000];
        int read_ch = 0;

        sscanf (cur_line, "%s %n", cmd, &read_ch);

        if (cmd[0] == '\n')
        {
            continue;
        }
        
        else if (strcmpi (cmd, "add") == 0)
        {
            code[ip_com++] = CMD_ADD;

            int val = 0;
            sscanf (cur_line + read_ch, "%d", &val);

            code[ip_com++] = val;
        }
        
        else if (strcmpi (cmd, "sum") == 0)
        {
            code[ip_com++] = CMD_SUM;
        }
        
        else if (strcmpi (cmd, "sub") == 0)
        {
            code[ip_com++] = CMD_SUB;
        }
        
        else if (strcmpi (cmd, "mult") == 0)
        {
            code[ip_com++] = CMD_MULT;
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
    }

    return ip_com;
}

static int Write_convert_file_ (int *code, File_info *file_info, FILE *fp_logs)
{
    int fdout = creat ("convert_input.txt", O_WRONLY);

    #ifdef BIN_REPRESENT

        if (fdout < 0)
        {
            Log_report ("Converted file did't create");
            return CREAT_CONVERT_FILE_ERR;
        }

        size_t size_file_info = sizeof (file_info);
        write (fdout, file_info, size_file_info);

        write (fdout, code, sizeof (int) * file_info->cnt_com);

    #else

        FILE *fpout = fdopen (fdout, "wt");

        if (fpout == nullptr)
        {
            Log_report ("Converted file did't create");
            return CREAT_CONVERT_FILE_ERR;
        }

        fprintf (fpout, "%s\n", file_info->sig);
        fprintf (fpout, "%s\n", file_info->ver);
        fprintf (fpout, "%d\n", file_info->cnt_com);

        for (int id = 0; id < file_info->cnt_com; id++)
            fprintf (fpout, "%d ", code[id]);
    
    #endif
    
    if (close (fdout))
    {
        Log_report ("Converted file did't close");
        return CREAT_CONVERT_FILE_ERR;
    }
    
    return 0;
}