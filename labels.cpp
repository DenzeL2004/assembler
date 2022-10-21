#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "labels.h"


int Label_init (Label *label, const int ip_cmd, const char *name, const int bypass) 
{
    assert (label != nullptr && "pointer to nullptr label");

    label->string_code = Get_str_code (name);
    
    label->ptr_jump = ip_cmd;

    label->bypass = bypass;

    return 0;
}

int Check_reserved_name (const char *name_new_label, const unsigned int *cmd_code)
{
    assert (name_new_label != nullptr && "name_new_label is nullptr");

    unsigned int code_str = Get_str_code (name_new_label);

    for (int ip_com = 0; ip_com < Max_cnt_cmd; ip_com++)
        if (cmd_code[ip_com] == code_str)
            return 1;

    return 0;
}

int Check_cnt_labels (const int cur_cnt_labels, const int label_capacity)
{
    return cur_cnt_labels == label_capacity;
}

int Recalloc_cnt_labels (Label *labels, int *label_capacity)
{
    assert (labels != nullptr && "labels is nullptr");
    
    *label_capacity += Additional_labels;
    realloc (labels, *label_capacity * sizeof (Label));

    if (labels == nullptr) return RECALLOC_MEMORY_ERR;

    return 0;
}

int Find_label (const Label *labels, const char *name_new_label, const int cnt_labels) 
{
    assert (labels != nullptr && "pointer to nullptr label");
    assert (name_new_label != nullptr && "name_new_label is nullptr");

    unsigned int new_string_code = Get_str_code (name_new_label);

    for (int ip_label = 0; ip_label < cnt_labels; ip_label++) 
    {
        if ((labels + ip_label)->string_code == new_string_code) 
        {
            return ip_label;
        }
    }

    return Not_init_label;
}

unsigned int Get_str_code (const char *str)
{
    assert (str != nullptr && "str is nullptr");

    unsigned int code = 0;
    int pw = 1;

    while (*str != '\0')
    {
        code += (tolower(*str) - '\0') * pw;
        pw *= 2;

        str++;
    }

    return code;
}