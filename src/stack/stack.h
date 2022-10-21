#ifndef _STACK_H_
#define _STACK_H_

#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <assert.h>

#include "config.h"
#include "log_def.h"

struct  Stack_info
{
    char *origin_file = nullptr;
    char *origin_func = nullptr;
    int origin_line = 0;
};

struct Stack 
{
    #ifdef CANARY_PROTECT
        uint64_t canary_val_begin = CANARY_VAL;
    #endif

    elem *data = nullptr;

    long size_data = 0;
    long capacity  = 0;

    Stack_info stack_info = {};

    #ifdef HASH
        uint64_t hash_struct = 0;
        uint64_t hash_data   = 0;
    #endif

    #ifdef CANARY_PROTECT
        uint64_t canary_val_end  = CANARY_VAL;
    #endif
};

enum Change_stack_capacity
{
    DECREASE  = -1,
    NO_CHANGE =  0,
    INCREASE  =  1,
    CHANGE    =  2
};

enum Stack_err
{
    BAD_DATA_PTR         = 1,

    SIZE_LOWER_ZERO      = 2,
    CAPACITY_LOWER_ZERO  = 4,
    CAPACITY_LOWER_SIZE  = 8,
    
    CANARY_CURUPTED      = 16,
    CANARY_PTR_CURUPTED  = 32,
    
    HASH_DATA_CURUPTED   = 64,
    HASH_STRUCT_CURUPTED = 128
};

enum Stack_func_err{
    STACK_CTOR_ERR       = -1,
    SET_STACK_VALLS_ERR  = -2,
    
    RECALLOC_STACK_ERR   = -3,
    RESIZE_CHECK_ERR     = -4,
    STACK_RESIZE_ERR     = -5,
    
    STACK_PUSH_ERR       = -6,
    STACK_POP_ERR        = -7,
    
    STACK_DTOR_ERR       = -8,

    STACK_SAVE_HASH_ERR  = -9,

    STACK_INFO_CTOR_ERR  = -10,
    STACK_INFO_DTOR_ERR  = -11,
};

#define Stack_ctor(stack, capacity)                 \
        Stack_ctor_ (stack, capacity, LOG_ARGS, fp_logs)

int Stack_ctor_ (Stack *stack, long capacity, LOG_PARAMETS, FILE *fp_logs);

#define Stack_dtor(stack)                           \
        Stack_dtor_ (stack, fp_logs)

int Stack_dtor_ (Stack *stack, FILE *fp_logs);

#define Stack_push(stack, val)                      \
        Stack_push_ (stack, val, fp_logs)

int Stack_push_ (Stack *stack, elem  val, FILE *fp_logs);

#define Stack_pop(stack, val)                       \
        Stack_pop_ (stack, val, fp_logs)

int Stack_pop_  (Stack *stack, elem *val, FILE *fp_logs);

#ifdef USE_LOG

    #define Stack_dump(stack)                       \
            Stack_dump_ (stack, LOG_ARGS, fp_logs)

#else  
    
    #define Stack_dump(stack)                   

#endif

uint64_t Stack_dump_ (Stack *stack, LOG_PARAMETS, FILE *fp_logs);

#endif