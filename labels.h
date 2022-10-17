#ifndef _LABELS_H_
#undef  _LABELS_H_

#include <string.h>


const int Max_cnt_labels = 50;

const int Uninit_label = -1;


enum Bypass_number
{
    FIRST   = 1, 
    SECOND  = 2,
};


enum Label_err
{
    REDEF_LABEL_ERR = -1,
};


struct Label 
{
    int string_code = 0;
    int ptr_jump    = 0;
    
    int bypass = 0; 
};


int Label_ctor (Label *label, const int ip_cmd, const char *name);

int Label_dtor (Label *label);

int Find_label (Label *labels, const char *name_new_label, const int cnt_labels);

int Get_str_code (const char *str);

#endif //#endif _LABELS_H_