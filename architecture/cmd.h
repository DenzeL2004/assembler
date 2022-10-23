
DEF_CMD (HLT, 0, 0, {
    CPU_CODE = (ptr_beg_code + cpu->cnt_bytes);
})

DEF_CMD (PUSH, 1, 1, {

    elem arg = Get_arg (cmd, CPU);

    if (cmd & ARG_RAM)
    {
        if (arg < Ram_size && arg >= 0)
        {
            arg = CPU_RAM;
            usleep (Program_delay);
        }

        else
        {
            Log_report ("Accessing unallocated memory\n");
            Err_report ();

            return PROCESS_COM_ERR;
        }
    }

    SET_VAL_TO_STACK (arg);
})

DEF_CMD (IN, 2, 0, {

    elem val = 0;
    if (scanf ("%" USE_TYPE, &val) != 1)
    {
        Log_report ("Invalid number of arguments read\n");
        return PROCESS_COM_ERR;
    }
    
    SET_VAL_TO_STACK (val);
})

DEF_CMD (ADD, 3, 0, {

    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (val1 + val2);
})

DEF_CMD (SUB, 4, 0, {
    
    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (val2 - val1);
})

DEF_CMD (MULT, 5, 0, {
    
    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (val1 * val2);
})

DEF_CMD (DIV, 6, 0, {

    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (val2 / val1);
})

DEF_CMD (SQRT, 7, 1, {
    elem val = 0;
    GET_VAL_FROM_STACK (&val);
    
    
    val = (elem) sqrt (val);
    SET_VAL_TO_STACK (val);
})

DEF_CMD (POP, 15, 1, {

    elem val = 0;
    GET_VAL_FROM_STACK (&val);

    elem arg = Get_arg (cmd, CPU);

    if (cmd & ARG_RAM)
    {
        if (arg < Ram_size)
        {
            CPU_RAM = val;
            usleep (Program_delay);
        }

        else
        {
            Log_report ("Accessing unallocated RAM memory\n");
            Err_report ();

            return PROCESS_COM_ERR;
        }
    }

    else if (cmd & ARG_REG)
    {
        if (cmd & ARG_IMM)
        {
            Log_report ("Incorrect command entry\n");
            Err_report ();

            return PROCESS_COM_ERR;
        }

        else if (arg < Cnt_reg)
            CPU_REGS = val;

        else
        {
            Log_report ("Accessing unallocated REG memory\n");
            Err_report ();

            return PROCESS_COM_ERR;
        }
    }

    else if (cmd & ARG_IMM)
    {
        Log_report ("Incorrect command entry\n");
        Err_report ();

        return PROCESS_COM_ERR;
    }
})

DEF_CMD (DUP, 20, 0, {
    elem val = 0;

    GET_VAL_FROM_STACK (&val);

    SET_VAL_TO_STACK (val);
    SET_VAL_TO_STACK (val);   
})



DEF_CMD (OUT, 16, 0, {
    elem val = 0;
    GET_VAL_FROM_STACK (&val);

    printf ("OUT VAL: %" USE_TYPE "\n", val);
})

DEF_CMD_JUMP (JUMP, 8, *, {
    CPU_CODE = (ptr_beg_code + *CPU_CODE);
})

DEF_CMD_JUMP (JA,  9,  >)

DEF_CMD_JUMP (JAE, 10, >=)

DEF_CMD_JUMP (JB, 11,  <)

DEF_CMD_JUMP (JBE, 12, <=)

DEF_CMD_JUMP (JL, 13, ==)

DEF_CMD_JUMP (JM, 14, !=)

DEF_CMD_JUMP (CALL, 17, *, {
    
    SET_VAL_TO_STACK ((elem) (CPU_CODE - ptr_beg_code));

    CPU_CODE = (ptr_beg_code + *(int*) CPU_CODE);

})

DEF_CMD (RET, 18, 0, {

    elem ip_jump = 0;
    GET_VAL_FROM_STACK (&ip_jump);
    
    CPU_CODE = (ptr_beg_code + (int) ip_jump);
    CPU_CODE += sizeof (elem);
})

DEF_CMD (SHOWRAM, 19, 0, {
       Show_ram (CPU);
})


