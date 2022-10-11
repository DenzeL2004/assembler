#include <assert.h>
#include <stdio.h>

#include "labels.h"


int Label_ctor (Label *label, const int ip_cmd, const char *name) 
{
    assert (label != nullptr && "pointer to nullptr label");

    label->string_code = Get_str_code (name);
    
    label->ptr_jump = ip_cmd;

    label->bypass = 0;

    return 0;
}

int Label_dtor (Label *label) 
{
    assert (label != nullptr && "pointer to nullptr label");

    label->string_code = 0;

    label->ptr_jump = -1;

    label->bypass = 0;

    return 0;
}

int Find_label (Label *labels, const char *name_new_label, const int cnt_labels) 
{
    assert (labels != nullptr && "pointer to nullptr label");

    int new_string_code = Get_str_code (name_new_label);

    for (int ip_label = 0; ip_label < cnt_labels; ip_label++) 
    {
        
        if ((labels + ip_label)->string_code == new_string_code) 
        {
            return ip_label;
        }
    }

    return -1;
}

int Get_str_code (const char *str)
{
    int code = 0;
    int pw = 1;

    while (*str != '\0')
    {
        code += (*str - '\0') * pw;
        pw *= 2;

        str++;
    }

    return code;
}