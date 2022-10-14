#include <stdio.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
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

static int Init_cpu_ram    (Cpu_struct *cpu);

static int Init_cpu        (Cpu_struct *cpu, int fdin);


static int Comands_exe (Cpu_struct *cpu);

static int Check_header (Bin_file *Bin_file);

static int Proc_dump (Cpu_struct *cpu);



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
    if (Check_nullptr (cpu->code))
        Log_report ("Memory has not been allocated yet\n");
    else
        free (cpu->code);


    cpu->cnt_bytes = -1;

    cpu->cur_cmd = -1;

    if (Check_nullptr (cpu->ram))
        Log_report ("Memory has not been allocated yet\n");
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

    if (read_byte < 0)
    {
        Log_report ("function read outputs a negative number\n");
        return PROCESS_ERR;
    }

    if (Check_header (&bin_file))
    {
        Log_report ("File headers did not match constants\n");
        return PROCESS_ERR;
    }

    read (fdin, cpu, sizeof (int));

    cpu->code = (unsigned char*) calloc (cpu->cnt_bytes, sizeof (char));

    read_byte = read (fdin, cpu->code, file_stat.st_size - sizeof (Bin_file) + sizeof (int));

    if (read_byte < 0)
    {
        Log_report ("function read outputs a negative number\n");
        return PROCESS_ERR;
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

static int Comands_exe (Cpu_struct *cpu)
{
    assert (cpu != nullptr && "cpu is nullptr");    

    unsigned char *code = cpu->code;
    unsigned char *ptr_beg_code = code;


    while (code - ptr_beg_code < cpu->cnt_bytes)
    {
        cpu->cur_cmd = code - ptr_beg_code;

        char cmd = *code;
        code++;

        switch (cmd & Cmd_mask)
        {   
            case CMD_JUMP:
                code = (ptr_beg_code + *code);
                break;

            case CMD_PUSH:
            {
                elem arg = 0;

                if (cmd & ARG_NUM)
                {
                    arg  += *(int*) code;
                    code += sizeof (int); 
                }    

                if (cmd & ARG_REG)
                {
                    if (*code < Cnt_reg)
                        arg  += cpu->regs[*code];
                    else
                    {
                        Log_report ("Accessing unallocated memory\n");
                        return PROCESS_COM_ERR;
                    }

                    code += sizeof (char); 
                }

                if (cmd & ARG_RAM)
                {
                    if (arg < Ram_size && arg >= 0)
                        arg = cpu->ram[arg];
                    else
                    {
                        Log_report ("accessing unallocated memory\n");
                        return PROCESS_COM_ERR;
                    }
                }
                
                Stack_push (&cpu->stack, arg);
            }
                break;

            /*case CMD_POP:
            {
                elem val = 0;
                Stack_pop (&cpu->stack, &val);

                elem arg = 0; 

                if (cmd & ARG_NUM)
                {
                    arg  += *(int*) code;
                    code += sizeof (int); 
                }  

                if (cmd & ARG_REG)
                {
                    arg  += cpu->regs[*code];
                    code += sizeof (char); 

                    if (cmd & ARG_RAM)
                    {
                        if (arg < Cnt_reg)
                            cpu->ram[arg] = val;
                        else
                            Log_report ("Accessing unallocated memory\n");
                    }

                }
                
            }
                break;*/

            case CMD_IN:
            {
                elem val = 0;
                
                scanf ("%" USE_TYPE, &val);
                
                Stack_push (&cpu->stack, val);
                
                code++;
            }
                break;
            

            case CMD_ADD:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&cpu->stack, &val1);
                Stack_pop (&cpu->stack, &val2);

                Stack_push (&cpu->stack, val1 + val2);
            }
                break;
            

            case CMD_MUT:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&cpu->stack, &val1);
                Stack_pop (&cpu->stack, &val2);

                Stack_push (&cpu->stack, val1 * val2);
            }    
                break;
            

            case CMD_SUB:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&cpu->stack, &val1);
                Stack_pop (&cpu->stack, &val2);

                Stack_push (&cpu->stack, val1 - val2);
            }
                break;

            case CMD_DIV:
            {
                elem val1 = 0, val2 = 0;
                Stack_pop (&cpu->stack, &val1);
                Stack_pop (&cpu->stack, &val2);

                Stack_push (&cpu->stack, val1 / val2);
            }
                break;

            case CMD_OUT:
            {
                elem val = 0;
                Stack_pop (&cpu->stack, &val);

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

        Proc_dump (cpu);
    }

    return 0;
}

//======================================================================================

/*static int Get_arg (char cmd, char *code)
{
    assert (cpu == nullptr && "cpu is nullptr");

    int arg = 0;

    if (cmd & ARG_NUM)
    {
        arg  += *(int*) code;
        code += sizeof (int); 
    }    

    if (cmd & ARG_REG)
    {
        arg  += *code;
        code += sizeof (char); 
    }

    return 0;
}*/

//======================================================================================

static int Init_cpu_ram (Cpu_struct *cpu)
{
    assert (cpu == nullptr && "cpu is nullptr");

    cpu->ram = (elem*) calloc (Ram_size, sizeof (elem));

    if (Check_nullptr (cpu->ram))
    {
        Log_report ("An error occurred while allocating memory for the processor\n");
        return NOT_ALLOC_PTR;
    }

    return 0;
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
    printf ("cur_cmd = %d", cpu->code[cpu->cur_cmd]);
    getc (stdin);

    printf ("\n");
    printf ("============================================\n\n");

    return 0;
}

//======================================================================================