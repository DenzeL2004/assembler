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
#include "assembler_config.h"


FILE *fp_logs = stderr;


static int Cpu_struct_ctor (Cpu_struct *cpu);

static int Cpu_struct_dtor (Cpu_struct *cpu);

static int Proc_comands (Cpu_struct *cpu);

static int Init_cpu (Cpu_struct *cpu, int fdin);

static int Check_header (Bin_file *Bin_file);

static int Proc_dump (Cpu_struct *cpu, Stack *stack);


static int Cpu_struct_ctor (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "fcpu is nullptr");

    cpu->code = nullptr;

    cpu->cnt_bytes = 0;

    cpu->cur_cmd = -1;

    return 0;
}

static int Cpu_struct_dtor (Cpu_struct *cpu)
{
    if (cpu->code == nullptr)
    {
        Log_report ("Memory has not been allocated yet\n");
        return 0;
    }

    if (cpu->code == (unsigned char*) POISON)
    {
        Log_report ("Memory has been freed\n");
        return 0;
    }

    free (cpu->code);

    cpu->code = (unsigned char*) POISON;

    cpu->cnt_bytes = -1;

    cpu->cur_cmd = -1;

    return 0;
}

static int Init_cpu (Cpu_struct *cpu, int fdin)
{
    assert (cpu != nullptr && "cpu is nullptr");
    assert (fdin >= 0 && "file descriptor is negative number");

    struct stat file_stat = {};
    if (fstat (fdin, &file_stat))
    {
        Log_report ("Unsuccessful memory extraction\n");
        return INIT_FILE_INFO_ERR;
    }

    Bin_file bin_file = {};

    int read_byte = read (fdin, &bin_file, sizeof (Bin_file));

    if (read_byte < 0)
    {
        Log_report ("function read outputs a negative number\n");
        return INIT_FILE_INFO_ERR;
    }

    if (Check_header (&bin_file))
    {
        Log_report ("File headers did not match constants\n");
        return INIT_FILE_INFO_ERR;
    }

    read (fdin, cpu, sizeof (int));

    cpu->code = (unsigned char*) calloc (cpu->cnt_bytes, sizeof (char));

    read_byte = read (fdin, cpu->code, file_stat.st_size - sizeof (Bin_file) + sizeof (int));

    if (read_byte < 0)
    {
        Log_report ("function read outputs a negative number\n");
        return INIT_FILE_INFO_ERR;
    }

    return 0;
}

static int Check_header (Bin_file *bin_file)
{
    assert (bin_file != nullptr && "File info is nullptr");

    if (bin_file->signature != Sig)
    {
        Log_report ("Assemblers signature did't match the processor\n");
        return SIGNATURE_MISMATCH_ERR;
    }

    if (bin_file->asm_version != Ver)
    {
        Log_report ("Assemblers version did't match the processor\n");
        return VERSION_MISMATCH_ERR;
    }

    return 0;
}

int Run_proc (int fdin)
{
    assert (fdin >= 0 && "file descriptor is negative number");

    #ifdef USE_LOG
        
        fp_logs = Open_logs_file ();
        
        if (fp_logs == stderr)
            return OPEN_FILE_LOG_ERR;

    #endif

    Cpu_struct cpu = {};
    Cpu_struct_ctor (&cpu);

    if (Init_cpu (&cpu, fdin))
    {
        Log_report ("File initialization error\n");
        return PROCESS_ERR;
    }

    if (Proc_comands (&cpu))
    {
        Log_report ("command processing failed\n");
        return PROCESS_ERR;
    }

    if (Cpu_struct_dtor (&cpu))
    {
        Log_report ("File destructor ended with an error\n");
        return PROCESS_ERR;
    }

    #ifdef USE_LOG
        
        if (Close_logs_file (fp_logs))
            return CLOSE_FILE_LOG_ERR;

    #endif

    return 0;
}

static int Proc_comands (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");    

    unsigned char *code = cpu->code;
    unsigned char *ptr_beg_code = code;

    Stack stack = {};
    Stack_ctor (&stack, Min_stack_size);

    while (code - ptr_beg_code < cpu->cnt_bytes)
    {
        cpu->cur_cmd = code - ptr_beg_code;

        char cmd = *code;
        code++;

        switch (cmd)
        {   
            case CMD_JUMP:
                code = (ptr_beg_code + *code);
                break;

            case CMD_PUSH:
            {
                Stack_push (&stack, *(elem*) code);

                code += sizeof (elem);
            }
                break;

            case CMD_IN:
            {
                elem val = 0;
                
                scanf ("%" USE_TYPE, &val);
                
                Stack_push (&stack, val);
                
                code++;
            }
                break;
            

            case CMD_ADD:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 + val2);
            }
                break;
            

            case CMD_MUT:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 * val2);
            }    
                break;
            

            case CMD_SUB:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 - val2);
            }
                break;

            case CMD_DIV:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&stack, &val1);
                Stack_pop (&stack, &val2);

                Stack_push (&stack, val1 / val2);
            }
                break;

            case CMD_OUT:
            {
                elem val = 0;
                Stack_pop (&stack, &val);

                printf ("last val stack %" USE_TYPE "\n", val);
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

        Proc_dump (cpu, &stack);
    }

    Stack_dtor (&stack);

    return 0;
}

static int Proc_dump (Cpu_struct *cpu, Stack *stack)
{
    assert (cpu != nullptr && "cpu is nullptr");
    assert (stack != nullptr && "stack is nullptr");

    Stack_dump (stack);

    printf ("============================================\n");

    //printf ("cur_byte: ");

    for (int ip_com = 0; ip_com < cpu->cnt_bytes; ip_com++)
    {
        printf ("%03d ", ip_com);     
    }

    printf ("\n");

  //  printf ("     com: ");

    for (int cur_bte = 0; cur_bte < cpu->cnt_bytes; cur_bte++)
    {
        printf ("%03d ", cpu->code[cur_bte]);
    }

    printf ("\n");

    printf (" %*s", (cpu->cur_cmd)*4, "^");
    printf ("cur_cmd = %d", cpu->code[cpu->cur_cmd]);
    getc (stdin);

    printf ("\n");
    printf ("============================================\n\n");

    return 0;
}