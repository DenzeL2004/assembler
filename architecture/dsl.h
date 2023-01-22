#ifndef _DSL_H_
#define _DSL_H_

#define CPU                                     \
    cpu

#define CPU_STACK                               \
    &cpu->stack

#define CPU_CODE                                \
    cpu->code

#define NEXT_CMD                                \
    cpu->code += sizeof (elem)

#define GET_IMM_VAL                             \
    *(elem*) cpu->code

#define MOVE_IP_CODE(jump)                      \
    cpu->code = (ptr_beg_code + (int) jump)

#define ASM_CODE                                \
    asmst->code

#define GET_VAL_FROM_STACK(val)                 \
    Stack_pop (CPU_STACK, val)

#define SET_VAL_TO_STACK(val)                   \
    Stack_push (CPU_STACK, val)


#endif  //#endif _DSL_H_