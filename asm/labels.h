#ifndef _LABELS_H_
#undef  _LABELS_H_

#include "../architecture/architecture.h"

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

struct Label_table
{
    int cnt_labels = 0;
    int label_capacity = 0;
    
    Label* labels = nullptr;
};


int Label_init (Label *label, const int ip_cmd, const char *name, const int bypass);

int Check_reserved_name (const char *name_new_label, const unsigned int *cmd_code);

int Check_cnt_labels (Label_table *label_table);

int Recalloc_cnt_labels (Label_table *label_table);

int Find_label (Label_table *label_table, const char *name_new_label);

unsigned int Get_str_hash (const char *str);

int Ctor_label_tabel (Label_table *label_table);

int Dtor_label_tabel (Label_table *label_table);

#endif //#endif _LABELS_H_