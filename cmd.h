DEF_CMD (HLT, 0, 0, {
    code = (ptr_beg_code + cpu->cnt_bytes);
})

DEF_CMD (PUSH, 1, 1, {

    elem arg = 0;

    code = Get_push_arg (code, cmd, &arg, cpu);

    if (code == nullptr)
    {
        Log_report ("Error in getting arguments in the push function\n");
        return PROCESS_COM_ERR; 
    }

    Stack_push (&cpu->stack, arg);
})

DEF_CMD (IN , 2, 0, {

    elem val = 0;
    if (scanf ("%" USE_TYPE, &val) != 1)
    {
        Log_report ("Invalid number of arguments read\n");
        return PROCESS_COM_ERR;
    }
    
    Stack_push (&cpu->stack, val);
})

DEF_CMD (ADD , 3, 0, {

    elem val1 = 0, val2 = 0;
    Stack_pop (&cpu->stack, &val1);
    Stack_pop (&cpu->stack, &val2);

    Stack_push (&cpu->stack, val1 + val2);
})

DEF_CMD (SUB, 4, 0, {
    
    elem val1 = 0, val2 = 0;
    Stack_pop (&cpu->stack, &val1);
    Stack_pop (&cpu->stack, &val2);

    Stack_push (&cpu->stack, val1 - val2);
})

DEF_CMD (MULT, 5, 0, {
    
    elem val1 = 0, val2 = 0;
    Stack_pop (&cpu->stack, &val1);
    Stack_pop (&cpu->stack, &val2);

    Stack_push (&cpu->stack, val1 * val2);
})

DEF_CMD (DIV , 6, 0, {

    elem val1 = 0, val2 = 0;
    Stack_pop (&cpu->stack, &val1);
    Stack_pop (&cpu->stack, &val2);

    Stack_push (&cpu->stack, val1 / val2);
})

DEF_CMD (POP , 15, 1, {
    elem val = 0;
    Stack_pop (&cpu->stack, &val);
    
    code = Set_pop_arg (code, cmd, val, cpu);

    if (code == nullptr)
    {
        Log_report ("Error in getting arguments in the pop function\n");
        return PROCESS_COM_ERR; 
    }
})



DEF_CMD (OUT , 16, 0, {

    elem val = 0;
    Stack_pop (&cpu->stack, &val);

    printf ("last val stack %" USE_TYPE "\n", val);
})

DEF_CMD_JUMP (JUMP, 8, *, {
    code = (ptr_beg_code + *code);
})

DEF_CMD_JUMP (JA,  9,  >)

DEF_CMD_JUMP (JAE, 10, >=)

DEF_CMD_JUMP (JB, 11,  <)

DEF_CMD_JUMP (JBE, 12, <=)

DEF_CMD_JUMP (JL, 13, ==)

DEF_CMD_JUMP (JM, 14, !=)

DEF_CMD_JUMP (CALL, 17, *, {
    
    Stack_push (&cpu->stack, (code - ptr_beg_code));
    code = (ptr_beg_code + *code);

})

DEF_CMD (RET , 18, 0, {

    int ip_jump = 0;
    Stack_pop (&cpu->stack, &ip_jump);
    
    code = (ptr_beg_code + ip_jump);

    code += sizeof (int);
})

