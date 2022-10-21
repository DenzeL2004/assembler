DEF_CMD (HLT, 0, 0, {
    CPU_CODE = (ptr_beg_code + cpu->cnt_bytes);
})

DEF_CMD (PUSH, 1, 1, {

    elem arg = 0;

    CPU_CODE = Get_push_arg (CPU_CODE, cmd, &arg, CPU);

    if (Check_nullptr (CPU_CODE))
    {
        Log_report ("Error in getting arguments in the push function\n");
        return PROCESS_COM_ERR; 
    }

    SET_VAL (arg);
})

DEF_CMD (IN , 2, 0, {

    elem val = 0;
    if (scanf ("%" USE_TYPE, &val) != 1)
    {
        Log_report ("Invalid number of arguments read\n");
        return PROCESS_COM_ERR;
    }
    
    SET_VAL (val);
})

DEF_CMD (ADD , 3, 0, {

    elem val1 = 0, val2 = 0;
    GET_LAST_VAL (val1);
    GET_LAST_VAL (val2);

    SET_VAL (val1 + val2);
})

DEF_CMD (SUB, 4, 0, {
    
    elem val1 = 0, val2 = 0;
    GET_LAST_VAL (val1);
    GET_LAST_VAL (val2);

    SET_VAL (val1 - val2);
})

DEF_CMD (MULT, 5, 0, {
    
    elem val1 = 0, val2 = 0;
    GET_LAST_VAL (val1);
    GET_LAST_VAL (val2);

    SET_VAL (val1 * val2);
})

DEF_CMD (DIV , 6, 0, {

    elem val1 = 0, val2 = 0;
    GET_LAST_VAL (val1);
    GET_LAST_VAL (val2);

    SET_VAL (val1 / val2);
})

DEF_CMD (SQRT , 7, 1, {
    elem val = 0;
    GET_LAST_VAL (val);
    
    
    val = (elem) sqrt (val);
    SET_VAL (val);
})

DEF_CMD (POP , 15, 1, {
    elem val = 0;
    GET_LAST_VAL(val);
    
    CPU_CODE = Set_pop_arg (CPU_CODE, cmd, val, CPU);

    if (Check_nullptr (CPU_CODE))
    {
        Log_report ("Error in getting arguments in the pop function\n");
        return PROCESS_COM_ERR; 
    }
})



DEF_CMD (OUT , 16, 0, {

    elem val = 0;
    GET_LAST_VAL (val);

    printf ("last val stack %" USE_TYPE "\n", val);
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
    
    SET_VAL ((elem) (CPU_CODE - ptr_beg_code));
    CPU_CODE = (ptr_beg_code + *(int*) CPU_CODE);

})

DEF_CMD (RET , 18, 0, {

    elem ip_jump = 0;
    GET_LAST_VAL (ip_jump);
    
    CPU_CODE = (ptr_beg_code + (int) ip_jump);

    CPU_CODE += sizeof (elem);
})

DEF_CMD (DUP , 20, 0, {
    elem val = 0;

    GET_LAST_VAL (val);
    
    SET_VAL (val);
    SET_VAL (val);   
})


DEF_CMD (SHOWRAM , 19, 0, {
       Show_ram (CPU);
})


