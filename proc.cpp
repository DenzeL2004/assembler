#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#include "src/Generals_func/generals.h"
#include "src/log_info/log_errors.h"


#include "dsl.h"

#include "proc.h"
#include "architecture.h"
#include "assembler.h"


static int Cpu_struct_ctor (Cpu_struct *cpu);

static int Cpu_struct_dtor (Cpu_struct *cpu);

static int Init_cpu        (Cpu_struct *cpu, int fdin);

static int Comands_exe (Cpu_struct *cpu);

static int Check_header (Cpu_struct *cpu);


static unsigned char *Get_push_arg 
(unsigned char *code, const int cmd, elem *arg, const Cpu_struct *cpu);

static unsigned char *Set_pop_arg 
(unsigned char *code, const int cmd, elem val, Cpu_struct *cpu);


static int Show_ram (Cpu_struct *cpu);


static int Proc_dump (Cpu_struct *cpu);

static int Write_cpu_code (FILE* fpout, Cpu_struct *cpu);

static int Write_cpu_regs (FILE* fpout, Cpu_struct *cpu);

//======================================================================================

static int Cpu_struct_ctor (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "fcpu is nullptr");

    cpu->code = nullptr;

    cpu->cnt_bytes = 0;

    cpu->cur_cmd = -1;

    if (Clear_data ((unsigned char*) cpu->regs,
                             sizeof (cpu->regs)))
    {
        Log_report ("Command table cleanup error\n");
        return ST_ASM_CTOR_ERR;
    }

    cpu->ram = nullptr;

    if (Stack_ctor (&cpu->stack, Min_stack_size))
    {
        Log_report ("An error occurred in the stack construction\n");
        Err_report ();

        return PROC_CTOR_ERR;
    }

    return 0;
}

//======================================================================================

static int Cpu_struct_dtor (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");

    if (Check_nullptr (cpu->code))
    {
        Log_report ("Memory has not been allocated for CODE yet\n");
        Err_report ();
    }
    else
        free (cpu->code);


    cpu->cnt_bytes = -1;

    cpu->cur_cmd = -1;

    if (Check_nullptr (cpu->ram))
    {
        Log_report ("Memory has not been allocated for RAM yet\n");
        Err_report ();
    }
    else
        free (cpu->ram);

    if (Clear_data ((unsigned char*) cpu->regs,
                             sizeof (cpu->regs)))
    {
        Log_report ("Command table cleanup error\n");
        return ST_ASM_CTOR_ERR;
    }

    if (Stack_dtor (&cpu->stack))
    {
        Log_report ("An error occurred in stack deconstruction\n");
        Err_report ();

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
        Err_report ();

        return READ_FROM_BIN_ERR;
    }

    cpu->code = (unsigned char*) calloc (file_stat.st_size, sizeof (unsigned char));

    int read_byte = read (fdin, cpu->code, file_stat.st_size);

    if (read_byte != read_byte)
    {
        Log_report ("function read outputs a negative number bin file\n");
        Err_report ();

        return READ_FROM_BIN_ERR;
    }

    
    if (Check_header (cpu))
    {
        Log_report ("File headers did not match constants\n");
        Err_report ();

        return READ_FROM_BIN_ERR;
    }


    cpu->cnt_bytes = *(int*) cpu->code;
    cpu->code += sizeof (int);


    cpu->ram = (elem*) calloc (Ram_size, sizeof (elem));
    if (Check_nullptr (cpu->ram))
    {
        Log_report ("An error occurred while allocating memory for the processor\n");
        Err_report ();

        return NOT_ALLOC_PTR;
    }

    return 0;
}

//======================================================================================

static int Check_header (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is is nullptr");

    Bin_file bin_file = {};

    bin_file.signature = *(int*) cpu->code;
    cpu->code += sizeof (int);

    bin_file.asm_version = *(int*) cpu->code;
    cpu->code += sizeof (int);


    if (bin_file.signature != Sig)
    {
        Log_report ("Assemblers signature did't match the processor\n");
        Err_report ();

        return SIGNATURE_MISMATCH_ERR;
    }

    if (bin_file.asm_version != Ver)
    {
        Log_report ("Assemblers version did't match the processor\n");
        Err_report ();

        return VERSION_MISMATCH_ERR;
    }

    return 0;
}

int Run_proc (int fdin)
{
    assert (fdin >= 0 && "file descriptor is negative number");

    Cpu_struct cpu = {};
    Cpu_struct_ctor (&cpu);

    if (Init_cpu (&cpu, fdin))
    {
        Log_report ("File initialization error\n");
        Err_report ();

        return PROCESS_ERR;
    }

    if (Comands_exe (&cpu))
    {
        Log_report ("command processing failed\n");
        Err_report ();

        return PROCESS_ERR;
    }


    if (Cpu_struct_dtor (&cpu))
    {
        Log_report ("File destructor ended with an error\n");
        Err_report ();

        return PROCESS_ERR;
    }

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
                GET_VAL_FROM_STACK (val1);                      \
                GET_VAL_FROM_STACK (val2);                      \
                                                                \
                if (val1 oper val2)                             \
                    code = (ptr_beg_code + *(int*) code);       \                              
                else                                            \
                    code += sizeof (elem);                      \
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

        //printf ("*cmd = %d\n", cmd);

        switch (cmd & Cmd_mask)
        {   
            #include "cmd.h"

            default:
                Log_report ("Unknown command: %d\n", cmd);
                Err_report ();

                return PROCESS_ERR;
        }

        Proc_dump (cpu);
        
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
    
    if (cmd & ARG_IMM)
    {
        *arg  += *(elem*) code;
        code += sizeof (elem); 
    }    

    if (cmd & ARG_REG)
    {
        if (*code < Cnt_reg)
            *arg  += cpu->regs[*code];
        else
        {
            Log_report ("Accessing unallocated memory\n");
            Err_report ();

            return nullptr;
        }

        code += sizeof (char); 
    }

    if (cmd & ARG_RAM)
    {
        if (*arg < Ram_size && *arg >= 0)
        {
            *arg = cpu->ram[(int) *arg];
            usleep (Program_delay);
        }
        else
        {
            Log_report ("Accessing unallocated memory\n");
            Err_report ();

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
        if (cmd & ARG_IMM)
        {
            arg  += *(int*) code;
            code += sizeof (elem); 
        }  

        if (cmd & ARG_REG)
        {
            arg  += cpu->regs[*(int*) code];
            code += sizeof (char); 
        }


        if (arg < Ram_size)
        {
            cpu->ram[arg] = val;
            usleep (Program_delay);
        }

        else
        {
            Log_report ("Accessing unallocated RAM memory\n");
            Err_report ();

            return nullptr;
        }
        
        return code;
    }

    if (cmd & ARG_REG)
    {
        arg  += *code;
        code += sizeof (char);

        if (cmd & ARG_IMM)
        {
            Log_report ("Incorrect command entry\n");
            Err_report ();

            return nullptr;
        }
       
        if (arg < Cnt_reg)
            cpu->regs[arg] = val;
        else
        {
            Log_report ("Accessing unallocated REG memory\n");
            Err_report ();

            return nullptr;
        }

        return code;
    }

    if (cmd & ARG_IMM)
    {
        Log_report ("Incorrect command entry\n");
        Err_report ();

        return nullptr;
    }

    return nullptr;
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

static int Proc_dump (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");

    Stack_dump (&cpu->stack);

    FILE *fp_log = Get_log_file_ptr ();

    fprintf (fp_log, "============================================\n\n");

    #ifdef USE_CPU_CODE_DUMP
        Write_cpu_code (fp_log, cpu);
    #endif

    fprintf (fp_log, "\n\n");

    #ifdef USE_CPU_CODE_DUMP
        Write_cpu_regs (fp_log, cpu);
    #endif

    fprintf (fp_log, "\n");
    fprintf (fp_log, "============================================\n\n");

    return 0;
}

//======================================================================================

static int Write_cpu_code (FILE* fpout, Cpu_struct *cpu)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cpu != nullptr && "cpu is nullptr");

    for (int ip_com = 0; ip_com < cpu->cnt_bytes; ip_com++)
    {
        fprintf (fpout, "%03d ", ip_com);     
    }

    fprintf (fpout, "\n");

    for (int cur_byte = 0; cur_byte < cpu->cnt_bytes; cur_byte++)
    {
        fprintf (fpout, "%03d ", cpu->code[cur_byte]);
    }

    fprintf (fpout, "\n");

    fprintf (fpout," %*s", (cpu->cur_cmd)*4, "^");
    fprintf (fpout, "cur_cmd = %d", cpu->code[cpu->cur_cmd]);

    return 0;
}

//======================================================================================

#define DEF_REG(name, num)   \
    fprintf (fpout, "%s: %"  USE_TYPE "\n", #name, cpu->regs[num]);   

static int Write_cpu_regs (FILE* fpout, Cpu_struct *cpu)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cpu != nullptr && "cpu is nullptr");

    #include "regs.h"

    return 0;
}

#undef DEF_REG

//======================================================================================
