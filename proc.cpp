#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include "proc.h"
#include "../Logs/log_errors.h"
#include "../My_stack/stack.h"

#include "assembler.h"

FILE *fp_logs = stderr;

static int Proc_comands (File_info *file_info);

static int Init_file_info (File_info *file_info, int fdin);

static int Check_header (File_info *file_info);

static int Proc_dump (File_info *file_info, Stack *stack);

static int Init_file_info (File_info *file_info, int fdin)
{
    assert (file_info != nullptr && "File info is nullptr");

    struct stat file_stat = {};
    fstat (fdin, &file_stat);

    int read_byte = read (fdin, file_info, sizeof (File_info) - sizeof (int));

    if (read_byte < 0)
    {
        Log_report ("function read outputs a negative number");
        return INIT_FILE_INFO_ERR;
    }

    file_info->code = (int*) calloc (file_info->cnt_com, sizeof (int));

    read_byte = read (fdin, file_info->code, file_stat.st_size - sizeof (File_info) + sizeof (int));

    if (read_byte < 0)
    {
        Log_report ("function read outputs a negative number");
        return INIT_FILE_INFO_ERR;
    }

    return 0;
}

static int Check_header (File_info *file_info)
{
    assert (file_info != nullptr && "File info is nullptr");

    /*if (strcmp (file_info->sig, SIG))
    {
        Log_report ("Assemblers signature did't match the processor\n");
        return SIGNATURE_MISMATCH_ERR;
    }

    if (strcmp (file_info->ver, VER))
    {
        Log_report ("Assemblers version did't match the processor\n");
        return VERSION_MISMATCH_ERR;
    }*/

    return 0;
}

int Processing (int fdin)
{
    assert (fdin >= 0 && "file descriptor is negative number");

    #ifdef USE_LOG
        
        fp_logs = Open_logs_file ();
        
        if (fp_logs == stderr)
            return OPEN_FILE_LOG_ERR;

    #endif

    File_info file_info = {};
    File_info_ctor (&file_info);

    Init_file_info (&file_info, fdin);

    if (Check_header (&file_info))
    {
        Log_report ("File headers did not match constants\n");
        return PROCESS_ERR;
    }

    if (Proc_comands (&file_info))
    {
        Log_report ("command processing failed\n");
        return PROCESS_ERR;
    }

    File_info_dtor (&file_info);

    #ifdef USE_LOG
        
        if (Close_logs_file (fp_logs))
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}

static int Proc_comands (File_info *file_info)
{
    assert (file_info != nullptr && "File info is nullptr");    

    int ip_com = 0;
    int *code = file_info->code;

    Stack stack = {};
    Stack_ctor (&stack, 5);

    while (ip_com < file_info->cnt_com)
    {
        switch (code[ip_com++])
        {
            case CMD_PUSH:
            {
                Stack_push (&stack, code[ip_com++]);
                break;
            }

            case CMD_ADD:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 + val2);

                break;
            }

            case CMD_MUT:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 * val2);
                
                break;
            }

            case CMD_SUB:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 - val2);
                
                break;
            }

            case CMD_DIV:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 / val2);
                
                break;
            }

            case CMD_OUT:
            {
                elem val = 0;
                Stack_pop (&stack, &val);

                printf ("last val stack %" PRINT_TYPE "\n", val);
                break;
            }
                
            case CMD_HLT:
            {
                return 0;
            }
            
            default:
                Log_report ("Unknown command\n");
                return PROCESS_ERR;
        }

        Proc_dump (file_info, &stack);
    }

    Stack_dtor (&stack);

    return 0;
}

static int Proc_dump (File_info *file_info, Stack *stack)
{
    assert (file_info != nullptr && "Code is nullptr");
    assert (stack != nullptr && "stack is nullptr");

    Stack_dump (stack);

    printf ("============================================\n");

    printf ("id_com: ");

    for (int id_com = 0; id_com < file_info->cnt_com; id_com++)
    {
        printf ("%02d ", id_com);     
    }

    printf ("\n");

    printf ("   com: ");

    for (int id_com = 0; id_com < file_info->cnt_com; id_com++)
    {
        printf ("%02d ", file_info->code[id_com]);
    }

    printf ("\n");

    printf ("============================================\n\n");

    return 0;
}