#ifndef _LABELS_H_
#undef  _LABELS_H_

#include <string.h>
#include "architecture.h"

static const int Init_number_labels = 20;

static const int Additional_labels  = 20;

static const int Not_init_label = -1;


enum Bypass_number
{
    FIRST   = 1, 
    SECOND  = 2,
};


enum Label_err
{
    REDEF_LABEL_ERR     = -1,

    RECALLOC_MEMORY_ERR = -2,
};


struct Label 
{
    unsigned int string_code = 0;
    elem ptr_jump    = 0;
    
    int bypass = 0; 
};

int Check_reserved_name (const char *name_new_label, const unsigned int *cmd_code);

int Check_cnt_labels (int cur_cnt_labels, int capacity_labels);

int Recalloc_cnt_labels (Label *labels, int *label_capacity);

int Label_init (Label *label, const int ip_cmd, const char *name, const int bypass);

int Find_label (const Label *labels, const char *name_new_label, const int cnt_labels);

unsigned int Get_str_code (const char *str);

#endif //#endif _LABELS_H_