
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

DEF_CMD (MUL, 5, 0, {
    
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

DEF_CMD (SQR, 7, 0, {
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

DEF_CMD (DUP, 9, 0, {
    elem val = 0;

    GET_VAL_FROM_STACK (&val);

    SET_VAL_TO_STACK (val);
    SET_VAL_TO_STACK (val);   
})



DEF_CMD (OUT, 10, 0, {
    elem val = 0;
    GET_VAL_FROM_STACK (&val);

    printf ("%.6" USE_TYPE "\n", val); 
})

DEF_CMD_JUMP (JUMP, 11, {
    MOVE_IP_CODE (GET_IMM_VAL); 
})

DEF_CMD_JUMP (JA,  12,  {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (!Equality_double(val1, val2) && val1 > val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);                          
    else                                                
       NEXT_CMD;
})

DEF_CMD_JUMP (JAE, 13, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (Equality_double(val1, val2) || val1 > val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JB, 14, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (!Equality_double(val1, val2) && val1 < val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JBE, 8, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (Equality_double(val1, val2) || val1 < val2)                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JE, 16, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (Equality_double(val1, val2))                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (JM, 17, {

    elem val1 = 0, val2 = 0;                           
    GET_VAL_FROM_STACK (&val1);                        
    GET_VAL_FROM_STACK (&val2);                        

    if (!Equality_double(val1, val2))                                 
        MOVE_IP_CODE (GET_IMM_VAL);                                 
    else                                                
        NEXT_CMD;
})

DEF_CMD_JUMP (CALL, 18, {
    
    SET_VAL_TO_STACK ((elem) (CPU_CODE - ptr_beg_code));
    
    MOVE_IP_CODE (GET_IMM_VAL);

})

DEF_CMD (RET, 19, 0, {

    elem ip_jump = 0;
    GET_VAL_FROM_STACK (&ip_jump);
    
    MOVE_IP_CODE (ip_jump);
    NEXT_CMD;
})

DEF_CMD (SHOWRAM, 20, 0, {
       Show_ram (CPU);
})

DEF_CMD (AND, 21, 0, {

    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) && ((int)val2));
})

DEF_CMD (OR, 22, 0, {
    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) || ((int)val2));
})

DEF_CMD (NOT, 23, 0, {

    elem val = 0;
    GET_VAL_FROM_STACK (&val);

    SET_VAL_TO_STACK (!((int)val));
})

DEF_CMD (GT, 24, 0, {
    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) < ((int)val2));
})

DEF_CMD (LT, 25, 0, {
    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) > ((int)val2));
})


DEF_CMD (NLT, 26, 0, {

    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) <= ((int)val2));
})

DEF_CMD (NGT, 27, 0, {

    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) >= ((int)val2));
})

DEF_CMD (EQ, 28, 0, {

    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) == ((int)val2));
})

DEF_CMD (NEQ, 29, 0, {

    elem val1 = 0, val2 = 0;
    GET_VAL_FROM_STACK (&val1);
    GET_VAL_FROM_STACK (&val2);

    SET_VAL_TO_STACK (((int)val1) != ((int)val2));
})

DEF_CMD (NEG, 30, 0, {

    elem val = 0;
    GET_VAL_FROM_STACK (&val);

    SET_VAL_TO_STACK (val * -1);
})

