#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



#include "../src/Generals_func/generals.h"
#include "../src/log_info/log_errors.h"


#include "../architecture/dsl.h"

#include "proc.h"
#include "../architecture/architecture.h"

static int Cpu_struct_ctor (Cpu_struct *cpu);

static int Cpu_struct_dtor (Cpu_struct *cpu);

static int Init_cpu        (Cpu_struct *cpu, FILE *fpin);

static int Comands_exe (Cpu_struct *cpu);

static int Check_header (unsigned char **buffer);



static elem* Get_addr (const unsigned char cmd, Cpu_struct *cpu);


static int Show_ram (Cpu_struct *cpu);


static int Proc_dump (const Cpu_struct *cpu, const unsigned char *beg_code);

static int Write_cpu_code (FILE* fpout, const Cpu_struct *cpu, const unsigned char *beg_code);

static int Write_cpu_regs (FILE* fpout, const Cpu_struct *cpu);

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
        return PROC_CTOR_ERR;
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
        return PROC_DTOR_ERR;
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

static int Init_cpu (Cpu_struct *cpu, FILE *fpin)
{
    assert (cpu != nullptr && "cpu is nullptr");
    assert (fpin != nullptr && "fpin si nullptr");

    fseek (fpin, 0, SEEK_END);
    int file_size = ftell (fpin) + 1;
    
    rewind (fpin);

    unsigned char* buffer = (unsigned char*) calloc (file_size, sizeof (unsigned char));
    if (Check_nullptr (buffer))
    {
        Log_report ("An error occurred while allocating memory for the buffer\n");
        Err_report ();

        return ERR_MEMORY_ALLOC;
    }

    int read_byte = fread (buffer, sizeof (unsigned char), file_size, fpin);

    //for (int id = 0; id < file_stat.st_size; id++)
      //  printf ("%d. %d\n", id, *(buffer + id));

   // printf ("read = %d, byte = %d\n", read_byte, file_stat.st_size);
    if (read_byte <= 0)
    {
        Log_report ("function read outputs a negative number bin file\n"
                    "read_byte = %d\n", read_byte);
        Err_report ();

        return READ_FROM_BIN_ERR;
    }

    if (Check_header (&buffer))
    {
        Log_report ("File headers did not match constants\n");
        Err_report ();

        return READ_FROM_BIN_ERR;
    }

    cpu->cnt_bytes = *(int*) buffer;
    buffer += sizeof (int);

    cpu->code = buffer;

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

static int Check_header (unsigned char **buffer)
{
    assert (buffer != nullptr && "buffer is is nullptr");

    Bin_file bin_file = {};

    bin_file.signature = *(int*) *buffer;
    *buffer += sizeof (int);

    bin_file.asm_version = *(int*) *buffer;
    *buffer += sizeof (int);


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

int Run_proc (FILE* fpin)
{
    assert (fpin != nullptr && "fpin is nullptr");

    Cpu_struct cpu = {};
    Cpu_struct_ctor (&cpu);

    if (Init_cpu (&cpu, fpin))
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


    cpu.code -= sizeof (int) * 3;
    if (Cpu_struct_dtor (&cpu))
    {
        Log_report ("File destructor ended with an error\n");
        Err_report ();

        return PROCESS_ERR;
    }

    return 0;
}

//======================================================================================

#define DEF_CMD_JUMP(name, num, ...)                                \                                       
        case num:                                                   \
        {                                                           \
            __VA_ARGS__                                             \            
            break;                                                  \   
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

    unsigned char *ptr_beg_code = cpu->code;

    while (cpu->code - ptr_beg_code < cpu->cnt_bytes)
    {
        cpu->cur_cmd = cpu->code - ptr_beg_code;

        unsigned char cmd = *cpu->code;

        cpu->code++;

        switch (cmd & Cmd_mask)
        {   
            #include "../architecture/cmd.h"

            default:
                Log_report ("Unknown command: %d\n", cmd);
                Err_report ();

                return PROCESS_ERR;
        }

        Proc_dump (cpu, ptr_beg_code);
    }

    cpu->code = ptr_beg_code;

    return 0;
}

#undef DEF_CMD 
#undef DEF_CMD_JUMP

//======================================================================================

static elem* Get_addr (const unsigned char cmd, Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");

    if (cmd & ARG_RAM)
    {
        int ip = 0;

        if (cmd & ARG_IMM)
        {
            ip  += (int) *(elem*) cpu->code;
            cpu->code += sizeof (elem); 
        }    
        
        if (cmd & ARG_REG)
        {
            ip  += (int) cpu->regs[*cpu->code];
            cpu->code += sizeof (char);
        }

        if (ip < 0 || ip > Ram_size)
        {
            Log_report ("Accessing unallocated RAM memory, ip = %d", ip);
            Err_report ();

            return nullptr;
        }

        return &cpu->ram[ip];
    }

    else
    {
        if ((cmd & ARG_IMM) && (cmd & ARG_REG))
        {
            Log_report ("Incorrect entry arguments\n");
            Err_report ();

            return nullptr; 
        }

        if (cmd & ARG_IMM)
        {
            elem *memory_ptr = (elem*) cpu->code;
            cpu->code += sizeof (elem);
            
            return memory_ptr;
        }    
        
        if (cmd & ARG_REG)
        {
            elem *memory_ptr = &cpu->regs[*cpu->code];
            cpu->code += sizeof (char);

            return memory_ptr;
        }
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
            int adress = ln * Cl_ram + cl + Offset;

            if (cpu->ram[adress])
            { 
                Print_colour (GREEN, "* ");
            }

            else
            {
                Print_colour (RED,   "* ");
            }
            
        }

        printf ("\n");
    }

    return 0;
}

//======================================================================================

static int Proc_dump (const Cpu_struct *cpu, const unsigned char *beg_code)
{
    assert (cpu != nullptr && "cpu is nullptr");

    Stack_dump ((Stack*) &cpu->stack);

    FILE *fp_log = Get_log_file_ptr ();

    fprintf (fp_log, "============================================\n\n");

    fprintf (fp_log, "DUMP PROC\n");

    #ifdef USE_CPU_CODE_DUMP
        Write_cpu_code (fp_log, cpu, beg_code);
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

static int Write_cpu_code (FILE* fpout, const Cpu_struct *cpu, const unsigned char *beg_code)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cpu != nullptr && "cpu is nullptr");
    assert (beg_code != nullptr && "beg_code is nullptr");

    
    for (int ip_com = 0; ip_com < cpu->cnt_bytes; ip_com++)
    {
        fprintf (fpout, "%03d ", ip_com);     
    }

    fprintf (fpout, "\n");

    for (int cur_byte = 0; cur_byte < cpu->cnt_bytes; cur_byte++)
    {
        fprintf (fpout, "%03d ", beg_code[cur_byte]);
    }

    fprintf (fpout, "\n");

    fprintf (fpout," %*s", (cpu->cur_cmd)*4, "^");
    fprintf (fpout, "cur_cmd = %d", beg_code[cpu->cur_cmd]);

    return 0;
}

//======================================================================================

#define DEF_REG(name, num)   \
    fprintf (fpout, "%s: %"  USE_TYPE "\n", #name, cpu->regs[num]);   

static int Write_cpu_regs (FILE* fpout, const Cpu_struct *cpu)
{
    assert (fpout != nullptr && "fpout is nullptr");
    assert (cpu != nullptr && "cpu is nullptr");

    #include "../architecture/regs.h"

    return 0;
}

#undef DEF_REG

//======================================================================================
