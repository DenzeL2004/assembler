#ifndef _DSL_H_
#define _DSL_H_

#define CPU                 \
    cpu

#define CPU_STACK           \
    &cpu->stack

#define CPU_CODE            \
    code

#define ASM_CODE            \
    asmst->code

#define GET_LAST_VAL(val)           \
    Stack_pop (CPU_STACK, &val);

#define SET_VAL(val)                \
    Stack_push (CPU_STACK, val);


#endif  //#endif _DSL_H_