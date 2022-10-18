#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "proc.h"
#include "../Generals_func/generals.h"
#include "../Logs/log_errors.h"
#include "../My_stack/stack.h"

#include "assembler.h"
#include "assembler_config.h"


FILE *fp_logs = stderr;


static int Cpu_struct_ctor (Cpu_struct *cpu);

static int Cpu_struct_dtor (Cpu_struct *cpu);

static int Init_cpu        (Cpu_struct *cpu, int fdin);

static int Comands_exe (Cpu_struct *cpu);

static int Check_header (Bin_file *Bin_file);

static int Proc_dump (Cpu_struct *cpu);

static unsigned char *Get_push_arg 
(unsigned char *code, const int cmd, elem *arg, const Cpu_struct *cpu);

static unsigned char *Set_pop_arg 
(unsigned char *code, const int cmd, elem val, Cpu_struct *cpu);

static int Show_ram (Cpu_struct *cpu);

//======================================================================================

static int Cpu_struct_ctor (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "fcpu is nullptr");

    cpu->code = nullptr;

    cpu->cnt_bytes = 0;

    cpu->cur_cmd = -1;

    for (int ip_reg = 0; ip_reg < Cnt_reg; ip_reg++)
    {
        cpu->regs[ip_reg] = 0;
    }

    cpu->ram = nullptr;

    if (Stack_ctor (&cpu->stack, Min_stack_size))
    {
        Log_report ("An error occurred in the stack construction\n");
        return PROC_CTOR_ERR;
    }

    return 0;
}

//======================================================================================

static int Cpu_struct_dtor (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");

    if (Check_nullptr (cpu->code))
        Log_report ("Memory has not been allocated for CODE yet\n");
    else
        free (cpu->code);


    cpu->cnt_bytes = -1;

    cpu->cur_cmd = -1;

    if (Check_nullptr (cpu->ram))
        Log_report ("Memory has not been allocated for RAM yet\n");
    else
        free (cpu->ram);


    if (Stack_dtor (&cpu->stack))
    {
        Log_report ("An error occurred in stack deconstruction\n");
        return PROC_DTOR_ERR;
    }

    return 0;
}

//======================================================================================

static int Init_cpu (Cpu_struct *cpu, int fdin)
{
    assert (cpu != nullptr && "cpu is nullptr");
    assert (fdin >= 0 && "file descriptor is negative number");

    struct stat file_stat = {};
    if (fstat (fdin, &file_stat))
    {
        Log_report ("Unsuccessful memory extraction\n");
        return PROCESS_ERR;
    }

    Bin_file bin_file = {};

    int read_byte = read (fdin, &bin_file, sizeof (Bin_file));

    if (read_byte <= 0)
    {
        Log_report ("function read outputs a negative number\n");
        return PROCESS_ERR;
    }

    if (Check_header (&bin_file))
    {
        Log_report ("File headers did not match constants\n");
        return PROCESS_ERR;
    }

    read (fdin, &cpu->cnt_bytes, sizeof (int));

    cpu->code = (unsigned char*) calloc (cpu->cnt_bytes, sizeof (unsigned char));
    
    read_byte = read (fdin, cpu->code, file_stat.st_size);

    if (read_byte <= 0)
    {
        Log_report ("function read outputs a negative number\n");
        return PROCESS_ERR;
    }

    cpu->ram = (elem*) calloc (Ram_size, sizeof (elem));

    if (Check_nullptr (cpu->ram))
    {
        Log_report ("An error occurred while allocating memory for the processor\n");
        return NOT_ALLOC_PTR;
    }

    return 0;
}

//======================================================================================

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

    if (Comands_exe (&cpu))
    {
        Log_report ("command processing failed\n");
        return PROCESS_ERR;
    }

    Show_ram (&cpu);

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

//======================================================================================

#define DEF_CMD_JUMP(name, num, oper, ...)                      \                                       
        case num:                                               \
        {                                                       \
            if (num == CMD_JUMP || num == CMD_CALL){            \
                __VA_ARGS__                                     \
            } else {                                            \
                elem val1 = 0, val2 = 0;                        \
                Stack_pop  (&cpu->stack, &val1);                \
                Stack_pop  (&cpu->stack, &val2);                \
                                                                \
                if (val1 oper val2)                             \
                    code = (ptr_beg_code + *code);              \                              
                else                                            \
                    code += sizeof (int);                       \
            }                                                   \
            break;                                              \
        }                                                       

#define DEF_CMD(name, num, arg, ...)                            \                                       
        case num:                                               \
        {                                                       \
            __VA_ARGS__                                         \
            break;                                              \
        }                                                       

static int Comands_exe (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");    

    unsigned char *code = cpu->code;
    unsigned char *ptr_beg_code = code;

    while (code - ptr_beg_code < cpu->cnt_bytes)
    {
        cpu->cur_cmd = code - ptr_beg_code;

        unsigned char cmd = *code;
        
        code++;
 
        switch (cmd & Cmd_mask)
        {   
            #include "cmd.h"

            default:
                Log_report ("Unknown command\n");
                return PROCESS_ERR;
        }

       // Proc_dump (cpu);
    }

    return 0;
}

#undef DEF_CMD 
#undef DEF_CMD_JUMP
//======================================================================================

static unsigned char *Get_push_arg 
        (unsigned char *code, const int cmd, elem *arg, const Cpu_struct *cpu)
{
    assert (code != nullptr && "code is nullptr");
    assert (cpu  != nullptr && "cpu is nullptr");
    
    if (cmd & ARG_NUM)
    {
        *arg  += *(int*) code;
        code += sizeof (int); 
    }    

    if (cmd & ARG_REG)
    {
        if (*code < Cnt_reg)
            *arg  += cpu->regs[*code];
        else
        {
            Log_report ("Accessing unallocated memory\n");
            return nullptr;
        }

        code += sizeof (char); 
    }

    if (cmd & ARG_RAM)
    {
        if (*arg < Ram_size && *arg >= 0)
        {
            *arg = cpu->ram[*arg];
            sleep (Program_delay);
        }
        else
        {
            Log_report ("Accessing unallocated memory\n");
            return nullptr;
        }
    }

    return code;
}

//======================================================================================

static unsigned char *Set_pop_arg 
        (unsigned char *code, const int cmd, elem val, Cpu_struct *cpu)
{
    assert (code != nullptr && "code is nullptr");
    assert (cpu  != nullptr && "cpu is nullptr");

    int arg = 0;

    if (cmd & ARG_RAM)
    {
        if (cmd & ARG_NUM)
        {
            arg  += *(int*) code;
            code += sizeof (int); 
        }  

        if (cmd & ARG_REG)
        {
            arg  += cpu->regs[*code];
            code += sizeof (char); 
        }

        if (arg < Ram_size)
        {
            cpu->ram[arg] = val;
            sleep (Program_delay);
        }

        else
        {
            Log_report ("Accessing unallocated RAM memory\n");
            return nullptr;
        }
        
        return code;
    }

    if (cmd & ARG_REG)
    {
        arg  += *code;
        code += sizeof (char);

        if (cmd & ARG_NUM)
        {
            Log_report ("Incorrect command entry\n");
            return nullptr;
        }
       
        if (arg < Cnt_reg)
            cpu->regs[arg] = val;
        else
        {
            Log_report ("Accessing unallocated REG memory\n");
            return nullptr;
        }

        return code;;
    }

    if (cmd & ARG_NUM)
    {
        Log_report ("Incorrect command entry\n");
        return nullptr;
    }

    return nullptr;
}

//======================================================================================

static int Proc_dump (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");

    Stack_dump (&cpu->stack);

    printf ("============================================\n\n");

    for (int ip_com = 0; ip_com < cpu->cnt_bytes; ip_com++)
    {
        printf ("%03d ", ip_com);     
    }

    printf ("\n");

    for (int cur_bte = 0; cur_bte < cpu->cnt_bytes; cur_bte++)
    {
        printf ("%03d ", cpu->code[cur_bte]);
    }

    printf ("\n");

    printf (" %*s", (cpu->cur_cmd)*4, "^");
    printf ("cur_cmd = %d", cpu->cur_cmd);
    getc (stdin);

    printf ("\n");
    printf ("============================================\n\n");

    return 0;
}

//======================================================================================

static int Show_ram (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");

    for (int ln = 0; ln < Ln_ram; ln++)
    {
        for (int cl = 0; cl < Cl_ram; cl++)
        {
            int adress = ln * Cl_ram + cl;

            if (cpu->ram[adress]) Print_colour (GREEN, "*");
            else                  Print_colour (RED, "*");
        }

        printf ("\n");
    }

    return 0;
}

//======================================================================================