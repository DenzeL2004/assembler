#include <math.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include "stack.h"

#include "../log_info/log_errors.h"
#include "../Generals_func/generals.h"



#define Stack_info_ctor(stack_info)                       \
        Stack_info_ctor_ (stack_info, LOG_VAR)

static int Stack_info_ctor_  (Stack_info *stack_info, LOG_PARAMETS);

#define Stack_info_dtor(stack_info)                       \
        Stack_info_dtor_ (stack_info)

static int Stack_info_dtor_  (Stack_info *stack_info);

#ifdef HASH

    #define Stack_hash_save(stack)                        \
        Stack_hash_save_ (stack)
#else

    #define Stack_hash_save(stack)                        \

#endif

static int Stack_hash_save_         (Stack *stack);

#define Check_hash_data(stack)                            \
        Check_hash_data_ (stack)

static int Check_hash_data_   (const Stack *stack);

#define Check_hash_struct(stack)                          \
        Check_hash_struct_ (stack)

static int Check_hash_struct_ (const Stack *stack);

#ifdef CANARY_PROTECT

    #define Stack_canaries_set(stack)                        \
        Stack_canaries_set_ (stack)
#else

    #define Stack_canaries_set(stack)                        \

#endif

static int Stack_canaries_set_ (Stack* stack);

#define Stack_vals_poison_set(stack)                      \
        Stack_vals_poison_set_ (stack)

static int Stack_vals_poison_set_   (Stack *stack);

#define Stack_preparing(stack)                      \
        Stack_preparing_ (stack)

static int Stack_preparing_ (Stack *stack);

#define Recalloc_stack(stack)                              \
        Recalloc_stack_ (stack)

static int Recalloc_stack_ (Stack *stack);

#define Check_resize(stack)                              \
        Check_resize_ (stack)

static int Check_resize_ (Stack *stack);

#define Stack_resize(stack, param)                              \
        Stack_resize_ (stack, param)

static int Stack_resize_ (Stack *stack, const int param);

#define Check_stack_data_ptr(stack)                       \
        Check_stack_data_ptr_ (stack)

static int Check_stack_data_ptr_ (Stack *stack);

#define Stack_check(stack)                                \
        Stack_check_ (stack)

static uint64_t Stack_check_     (Stack *stack);

//=======================================================================================================

int Stack_ctor_ (Stack *stack, long capacity, 
                 const char* file_name, const char* func_name, const int line)
{
    assert (stack != nullptr && "stack is nullptr");

    stack->stack_info = {};
    Stack_info_ctor (&stack->stack_info);

    stack->data = (elem*) &NOT_ALLOC_PTR;

    stack->size_data = 0;

    if (capacity <= 0)
    {
        printf ("You can't use number lower zero\n");

        Log_report ("The user tried to assign a negative number to the size\n");
        return STACK_CTOR_ERR;
    }

    stack->capacity  = capacity; 

    #ifdef CANARY_PROTECT
        stack->canary_val_begin = CANARY_VAL;
        stack->canary_val_end   = CANARY_VAL;
    #endif

    #ifdef HASH
        stack->hash_data = 0;
        stack->hash_struct = 0;
    #endif

    return 0;
}

//=======================================================================================================

int Stack_dtor_ (Stack *stack)
{   
    assert (stack != nullptr && "stack is nullptr");

    if (stack->data != &NOT_ALLOC_PTR)
    {
        if (Stack_dump (stack)) return STACK_DTOR_ERR;   
    }

    Stack_vals_poison_set (stack);

    if (stack->data != &NOT_ALLOC_PTR)
    {
        #ifdef CANARY_PROTECT
            free ((uint64_t*) stack->data - 1);
        #else
            free (stack->data);
        #endif
    }

    stack->data = (elem*) &POISON_PTR;

    stack->size_data = POISON_VAL;
    stack->capacity  = POISON_VAL;

    #ifdef CANARY_PROTECT
        stack->canary_val_begin = POISON_VAL;
        stack->canary_val_end   = POISON_VAL;
    #endif

    #ifdef HASH
        stack->hash_data   = POISON_VAL;
        stack->hash_struct = POISON_VAL;
    #endif

    Stack_info_dtor (&stack->stack_info);
    
    return 0;
}

//=======================================================================================================

static int Stack_info_ctor_ (Stack_info *stack_info, 
                             const char* file_name, const char* func_name, int line) 
{
    assert (stack_info != nullptr && "stack_info is nullptr");

    stack_info->origin_line = line;
    stack_info->origin_file = (char*) file_name;
    stack_info->origin_func = (char*) func_name;
    
    return 0;
}

//=======================================================================================================

static int Stack_info_dtor_ (Stack_info *stack_info) 
{
    assert (stack_info != nullptr && "stack_info is nullptr");
    assert (stack_info->origin_file != nullptr && "stack_info->origin_file is nullptr");
    assert (stack_info->origin_func != nullptr && "stack_info->origin_func is nullptr");
    
    stack_info->origin_file = (char*) POISON_PTR;

    stack_info->origin_func = (char*) POISON_PTR;

    stack_info->origin_line = -1;
    
    return 0;
}

//=======================================================================================================

#ifdef HASH

static int Stack_hash_save_ (Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");

    size_t size_struct = sizeof (*stack) - 2 * sizeof (uint64_t);

    #ifdef CANARY_PROTECT
        size_struct -= sizeof (uint64_t);
    #endif

    stack->hash_struct = Get_hash ((const char*) stack, size_struct);

    if (stack->capacity <= 0)
    {
        Log_report ("We are trying to save the memory of a negative size\n");
        return STACK_SAVE_HASH_ERR;
    }

    size_t size_data = stack->capacity * sizeof (elem);

    stack->hash_data = Get_hash ((const char*) stack->data, size_data);

    return 0;
}

#endif

//=======================================================================================================

#ifdef HASH

static int Check_hash_data_ (const Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");

    if (stack->data != nullptr && stack->data != (elem*) POISON_PTR &&
        stack->capacity > 0) 
    {
        int size_data = stack->capacity * sizeof (elem);

        uint64_t hash = Get_hash ((char*) stack->data, (unsigned int) size_data);

        if (hash == stack->hash_data)
            return 0;
        else
            return 1;
    }

    return 1;
}

#endif

//=======================================================================================================

#ifdef HASH

static int Check_hash_struct_ (const Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");

    unsigned int size_struct = sizeof (*stack) - 2 * sizeof (uint64_t);

    #ifdef CANARY_PROTECT
        size_struct -= sizeof (uint64_t);
    #endif

    uint64_t hash = Get_hash ((char*) stack, size_struct);

    if (hash == stack->hash_struct) 
        return 0;   
    else
        return 1;
}

#endif

//=======================================================================================================

static int Stack_vals_poison_set_ (Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");

    if (Check_stack_data_ptr (stack))
    {
        Log_report ("Stack vals don't assigned\n");
        
        Stack_dump (stack);
        
        return SET_STACK_VALLS_ERR;
    }

    if (stack->data == (elem*) &NOT_ALLOC_PTR)
    {
        Log_report ("Memory has not been allocated for subsequent stack filling\n");
        return SET_STACK_VALLS_ERR;
    }

    for (int id_elem = stack->size_data; id_elem < stack->capacity; id_elem++)
        stack->data[id_elem] = POISON_VAL;

    return 0;
}

//=======================================================================================================

int Stack_push_ (Stack *stack, elem val)
{
    assert (stack != nullptr && "stack is nullptr");
    
    if (stack->data == (elem*) &NOT_ALLOC_PTR)
    {
        Stack_preparing (stack);
    }

    if (Check_stack_data_ptr (stack)) return STACK_PUSH_ERR;

    if (Stack_dump (stack)) return STACK_PUSH_ERR;
    

    int ver_status = Stack_resize (stack, Check_resize (stack));
    switch (ver_status)
    {
        case CHANGE:
            Recalloc_stack (stack);
        
        case NO_CHANGE:
            break;

        default:
            Log_report ("In push recalloc didn't work correctly\n");
            
            Stack_dump (stack);
            
            return STACK_PUSH_ERR;
    }

    stack->data[stack->size_data++] = val;

    Stack_hash_save (stack);

    if (Stack_dump (stack)) return STACK_PUSH_ERR;

    return 0;
}

//=======================================================================================================

int Stack_pop_ (Stack *stack, elem *val)
{
    assert (stack != nullptr && "stack is nullptr");

    if (Check_stack_data_ptr (stack)) return STACK_PUSH_ERR;

    if (stack->data == &NOT_ALLOC_PTR)
    {
        Log_report ("Size is zero, you can't use Stack_pop\n");
        return 0;
    }

    if (Stack_dump (stack)) return STACK_POP_ERR;


    if (stack->size_data == 0)
    {
        Log_report ("Size is zero, you can't use Stack_pop\n");
        return 0;
    }

    int ver_status = Stack_resize (stack, Check_resize (stack));
    switch (ver_status)
    {
        case CHANGE:
            Recalloc_stack (stack);
            break;
        
        case NO_CHANGE:
            break;

        default:
            Log_report ("In pop recalloc didn't work correctly\n");

            Stack_dump (stack);
            
            return STACK_PUSH_ERR;
    }

    *val = stack->data[stack->size_data - 1];
    stack->data[stack->size_data - 1] = POISON_VAL;
    stack->size_data--;

    Stack_hash_save (stack);

    if (Stack_dump (stack)) return STACK_POP_ERR;

    return 0;
}

//=======================================================================================================

static int Check_resize_ (Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");

    if (Stack_dump (stack)) return RESIZE_CHECK_ERR;


    if (stack->capacity / 4 <= stack->size_data && 
        stack->size_data + 1 < stack->capacity / 2) return DECREASE;

    if (stack->capacity == stack->size_data)        return INCREASE;

    return NO_CHANGE;
}

//=======================================================================================================

static int Stack_resize_ (Stack *stack, const int param)
{
    assert (stack != nullptr && "stack is nullptr");

    if (Stack_dump (stack)) return STACK_RESIZE_ERR;

    switch (param)
    {
        case INCREASE:
            stack->capacity *= 2;
            break;

        case DECREASE:
            stack->capacity /= 2;
            break;

        case NO_CHANGE:
            return NO_CHANGE;
        
        default:
            Stack_dump (stack);
            return STACK_RESIZE_ERR;
            
    }

    Stack_hash_save (stack);

    return CHANGE;
}

//=======================================================================================================

static int Recalloc_stack_ (Stack *stack) 
{
    assert (stack != nullptr && "stack is nullptr");

    if (Check_stack_data_ptr (stack))
    {
        Log_report ("In recalloc BAD date pointer\n");
        return RECALLOC_STACK_ERR;
    }
    
    #ifdef CANARY_PROTECT

        int size = stack->capacity * sizeof(elem) + 2 * sizeof(uint64_t);

        uint64_t *start_data =  (uint64_t*) stack->data - 1;

        elem *data_ptr = (elem*) realloc(start_data, size * sizeof(char));
    
    #else

        elem *data_ptr = (elem*) realloc (stack->data, sizeof(elem) * stack->capacity);
    
    #endif

    #ifdef CANARY_PROTECT

        stack->data = (elem*) ((char*) data_ptr + sizeof(uint64_t));
        
        Stack_canaries_set (stack);

    #else

        stack->data = data_ptr;
    
    #endif

    Stack_vals_poison_set (stack);

    #ifdef HASH

        Stack_hash_save (stack);
    
    #endif
    
    if (Stack_dump (stack)) return RECALLOC_STACK_ERR;
    
    return 0;
}

//=======================================================================================================

static int Stack_canaries_set_ (Stack* stack) 
 {
    assert (stack != nullptr && "stack is nullptr");

    uint64_t *canary_ptr = (uint64_t*) stack->data - 1;

    *canary_ptr = CANARY_VAL;

    canary_ptr = (uint64_t*) ((char*) stack->data + stack->capacity * sizeof(elem));

    *canary_ptr = CANARY_VAL;

    return 0;
}

//=======================================================================================================

static int Stack_preparing_ (Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");
    
    #ifdef CANARY_PROTECT

        int size = (stack->capacity * sizeof (elem)) + 2 * sizeof (uint64_t);

        elem *data_ptr = (elem*) calloc(size, sizeof (char));

    #else 

        elem *data_ptr = (elem*)calloc(stack->capacity, sizeof (elem));
    
    #endif

    #ifdef CANARY_PROTECT

        char* begin_data_ptr = (char*) data_ptr + sizeof (uint64_t);

        stack->data = (elem*) begin_data_ptr;

        Stack_canaries_set (stack);
    
    #else    
    
        stack->data = data_ptr;
    
    #endif
        
    if (Check_stack_data_ptr (stack)) return STACK_PUSH_ERR;

    Stack_vals_poison_set (stack);

    #ifdef HASH
        Stack_hash_save (stack);
    #endif

    return 0;
}

//=======================================================================================================

uint64_t Stack_dump_ (Stack *stack,
                 const char* file_name, const char* func_name, int line)
{
    assert (stack != nullptr && "stack is nullptr");

    uint64_t err_code = Stack_check (stack);

    #ifdef USE_STACK_DUMP

    FILE *fp_logs = Get_log_file_ptr ();

    fprintf (fp_logs, "=================================================\n\n");

    fprintf (fp_logs, "REFERENCE:\n");

    if (err_code)
    {
        fprintf (fp_logs, "ERROR\nStack variable created in file %s in function %s line %d.\n", 
                stack->stack_info.origin_file,  stack->stack_info.origin_func, stack->stack_info.origin_line);
        
        fprintf (fp_logs, "Caused an error in file %s, function %s, line %d\n\n", LOG_VAR);
    }
    else
    {
        fprintf (fp_logs, "OK\nStack variable created in file %s in function %s line %d.\n", 
                stack->stack_info.origin_file,  stack->stack_info.origin_func, stack->stack_info.origin_line);
    
        fprintf (fp_logs, "last call in file %s, function %s, line %d\n\n", LOG_VAR);
    }
                     
    
    
    if (err_code)
    {
        fprintf (fp_logs, "ERR CODE: ");
        Bin_represent (fp_logs, err_code, sizeof (err_code));
        fprintf (fp_logs, "\n");
    }

    fprintf (fp_logs, "Stack pointer to data is |%p|\n\n", (char*) stack->data);

    fprintf (fp_logs, "Stack size_data = %ld\n", stack->size_data);
    fprintf (fp_logs, "Stack capacity  = %ld\n",  stack->capacity);

    #ifdef CANARY_PROTECT

        fprintf (fp_logs, "\nStack canary val begin = %I64X\n", stack->canary_val_begin);
        fprintf (fp_logs, "Stack canary val end   = %I64X\n",   stack->canary_val_end);

        if (!(err_code & BAD_DATA_PTR))
        {
            uint64_t *canary_ptr_begin = (uint64_t*) stack->data - 1;

            uint64_t *canary_ptr_end   = (uint64_t*) ((char*) stack->data + stack->capacity * sizeof(elem));

            fprintf (fp_logs, "\nCanary data ptr begin = %I64X\n", *canary_ptr_begin);
            fprintf (fp_logs, "Canary data ptr end   = %I64X\n",   *canary_ptr_end); 
        }

    #endif

    #ifdef HASH 

        fprintf (fp_logs, "\nStack hash data     = %I64u\n", stack->hash_data);
        fprintf (fp_logs, "Stack hash struct   = %I64u\n", stack->hash_struct);
    
    #endif

    fprintf (fp_logs, "\n");

    if (err_code & BAD_DATA_PTR) fprintf (fp_logs, "stack pointer data is BAD.\n");

    if (err_code & SIZE_LOWER_ZERO)      fprintf (fp_logs, "stack size_data is a negative number.\n");
    if (err_code & CAPACITY_LOWER_ZERO)  fprintf (fp_logs, "stack capacity is a negative number.\n");
    if (err_code & CAPACITY_LOWER_SIZE)  fprintf (fp_logs, "stack capacity is lower size_data:\n");

    #ifdef CANARY_PROTECT   

        if (err_code & CANARY_CURUPTED)     fprintf (fp_logs, "stack canary is currupted.\n");
        if (err_code & CANARY_PTR_CURUPTED) fprintf (fp_logs, "data canary is currupted.\n");
    
    #endif

    #ifdef HASH

        if (err_code & HASH_DATA_CURUPTED)   fprintf (fp_logs, "Hash value did not match\n");
        if (err_code & HASH_STRUCT_CURUPTED) fprintf (fp_logs, "Hash Struct did not match\n");
    
    #endif

    fprintf (fp_logs, "\n");

    if (!((err_code & BAD_DATA_PTR) || (err_code & HASH_STRUCT_CURUPTED) ||
          (err_code & SIZE_LOWER_ZERO) || (err_code & CAPACITY_LOWER_ZERO)))
    {
        for (int id_elem = 0; id_elem < stack->capacity; id_elem++)
        {
            if (id_elem < stack->size_data)
                fprintf (fp_logs, "%5d. *[%" PRINT_TYPE "]\n", 
                            id_elem, stack->data[id_elem]);
            else    
                fprintf (fp_logs, "%5d.  [%" PRINT_TYPE "] is POISON\n", 
                                    id_elem, stack->data[id_elem]);
        }
    }

    fprintf (fp_logs, "=================================================\n\n");

    #endif

    return err_code;
}

//=======================================================================================================

static int Check_stack_data_ptr_ (Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");

    if (stack->data == nullptr)
    {
        Log_report ("The user tried to use nullptr. Data is nullptr\n");
        return BAD_DATA_PTR;
    }

    if (stack->data == (elem*) POISON_PTR)
    {
        Log_report ("The user tried to use a pointer with a pison value\n");
        return BAD_DATA_PTR;
    }

    if (stack->data == (elem*) &NOT_ALLOC_PTR && 
        stack->capacity == 0 && stack->size_data == 0)
    {
            return 0;
    }

    if (stack->data == (elem*) &NOT_ALLOC_PTR && 
        (stack->size_data != 0))
    {
        Log_report ("The user tried to use a pointer which has't yet been allocated\n");
        return BAD_DATA_PTR;
    }

    return 0;
}

//=======================================================================================================

static uint64_t Stack_check_ (Stack *stack)
{
    assert (stack != nullptr && "stack is nullptr");   

    uint64_t err_code = 0;

    if (Check_stack_data_ptr (stack)) err_code |= BAD_DATA_PTR;

    if (stack->size_data < 0) err_code |= SIZE_LOWER_ZERO;
    if (stack->capacity  < 0) err_code |= CAPACITY_LOWER_ZERO;

    if (stack->capacity < stack->size_data) err_code |= CAPACITY_LOWER_SIZE;
    
    #ifdef CANARY_PROTECT

        if (stack->canary_val_begin != CANARY_VAL || stack->canary_val_end != CANARY_VAL)
            err_code |= CANARY_CURUPTED;

        if (!Check_stack_data_ptr (stack))
        {
            uint64_t *canary_ptr_begin = (uint64_t*) stack->data - 1;

            uint64_t *canary_ptr_end   = (uint64_t*) ((char*) stack->data + stack->capacity * sizeof(elem));

            if (*canary_ptr_begin != CANARY_VAL || 
                *canary_ptr_end   != CANARY_VAL)  err_code |= CANARY_PTR_CURUPTED;  
        }

    #endif

    #ifdef HASH

        if (!(err_code & CAPACITY_LOWER_ZERO))
            if (Check_hash_data (stack))   err_code |= HASH_DATA_CURUPTED;

        if (Check_hash_struct (stack)) err_code |= HASH_STRUCT_CURUPTED;
    
    #endif

    return err_code;    
}

//=======================================================================================================