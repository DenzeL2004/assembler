
DEF_CMD (HLT, 0, 0, {
    CPU_CODE = (ptr_beg_code + cpu->cnt_bytes);
})


DEF_CMD (PUSH, 1, 1, {

    elem *addr = Get_addr (cmd, CPU);

    if (Check_nullptr (addr))
    {
        Log_report ("Null pointer to memory");
        return ERR_NULLPTR;
    }

    SET_VAL_TO_STACK (*addr);
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

    elem *addr = Get_addr (cmd, CPU);

    if (Check_nullptr (addr))
    {
        Log_report ("Null pointer to memory");
        return ERR_NULLPTR;
    }

    *addr = val;
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

    printf ("%.6" USE_TYPE "\n", val); 
})

DEF_CMD_JUMP (JUMP, 8, {
    MOVE_IP_CODE (GET_IMM_VAL); //1) get_imm_val 2) set_ip
})

DEF_CMD_JUMP (JA,  9,  {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (!Equality_double(val1, val2) && val1 > val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);      // set_ip                           
    else                                                
       NEXT_CMD;
})

DEF_CMD_JUMP (JAE, 10, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (Equality_double(val1, val2) || val1 > val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JB, 11, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (!Equality_double(val1, val2) && val1 < val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JBE, 12, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (Equality_double(val1, val2) || val1 < val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JE, 13, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (Equality_double(val1, val2))                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JM, 14, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (!Equality_double(val1, val2))                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (CALL, 17, {
    
    SET_VAL_TO_STACK ((elem) (CPU_CODE - ptr_beg_code));

    MOVE_IP_CODE (GET_IMM_VAL);

})

DEF_CMD (RET, 18, 0, {

    elem ip_jump = 0;
    GET_VAL_FROM_STACK (&ip_jump);
    
    MOVE_IP_CODE (ip_jump);
    NEXT_CMD;
})

DEF_CMD (SHOWRAM, 19, 0, {
       Show_ram (CPU);
})
