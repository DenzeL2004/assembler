#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>

typedef double elem;


#define USE_LOG  //<- connect when we use logs

//#define USE_STACK_DUMP

#define DEBUG    //<- Use of protection



#ifdef DEBUG

    #define CANARY_PROTECT  //<- use of canary protection

    #ifdef CANARY_PROTECT
        const uint64_t CANARY_VAL = 0xDEADBABEDEADBABE;
    #else
        const uint64_t CANARY_VAL = 0;
    #endif

    #define HASH           //<- use of hash protection

#endif


const elem POISON_VAL = 228;       //<- fill stack with poisonous value

const int POISON_PTR = 4242564;   //<- fill pointer with poison value

const elem NOT_ALLOC_PTR = 666666; //<- fill in the value of the pointer to which we are going to allocate dynamic memory


#define PRINT_TYPE "lf"               //<- specifier character to print elem

#endif