#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "labels.h"
#include "src/Generals_func/generals.h"
#include "src/log_info/log_errors.h"


int Label_init (Label *label, const int ip_cmd, const char *name, const int bypass) 
{
    assert (label != nullptr && "pointer to nullptr label");

    label->string_code = (unsigned int) Get_str_hash (name);
    
    label->ptr_jump = ip_cmd;

    label->bypass = bypass;

    return 0;
}

int Check_reserved_name (const char *name_new_label, const unsigned int *cmd_code)
{
    assert (name_new_label != nullptr && "name_new_label is nullptr");

    unsigned int str_hash = (unsigned int) Get_str_hash (name_new_label);

    for (int ip_com = 0; ip_com < Max_cnt_cmd; ip_com++)
        if (cmd_code[ip_com] == str_hash)
            return 1;

    return 0;
}

int Check_cnt_labels (Label_table *label_table)
{
    return label_table->cnt_labels == label_table->label_capacity;
}

int Recalloc_cnt_labels (Label_table *label_table)
{
    assert (label_table != nullptr && "label_tablel is nullptr");
    
    label_table->label_capacity += Additional_labels;

    realloc (label_table->labels, label_table->label_capacity * sizeof (Label));

    if (Check_nullptr (label_table->labels))
    { 
        Log_report ("Error allocating memory for labels\n");
        Err_report ();

        return RECALLOC_MEMORY_ERR;
    }

    return 0;
}

int Find_label (Label_table *label_table, const char *name_new_label) 
{
    assert (label_table != nullptr && "label_tablel is nullptr");
    assert (name_new_label != nullptr && "name_new_label is nullptr");

    unsigned int new_string_hash = (unsigned int) Get_str_hash (name_new_label);

    for (int ip_label = 0; ip_label < label_table->cnt_labels; ip_label++) 
    {
        if ((label_table->labels + ip_label)->string_code == new_string_hash) 
        {
            return ip_label;
        }
    }

    return Not_init_label;
}

unsigned int Get_str_hash (const char *str)
{
    assert (str != nullptr && "str is nullptr");
    
    char *str_= strdup (str);
    int len = 0;
    while (*(str_+ len) != '\0')
    {
        str_[len] = (char) tolower (str_[len]);
        len++;
    }

    unsigned int hash = (unsigned int) Get_hash (str_, sizeof (str_));
    
    free (str_);

    return hash;
}

int Ctor_label_tabel (Label_table *label_table)
{
    assert (label_table != nullptr && "str is nullptr");

    label_table->cnt_labels      = 0;
    label_table->label_capacity  = Init_number_labels;
    label_table->labels = (Label*) calloc (Init_number_labels, sizeof(Label));

    if (label_table->labels == nullptr)
    {
        Log_report ("Memory was not allocated for the array of labels\n");
        Err_report ();

        return ERR_MEMORY_ALLOC;
    }

    return 0;
}

int Dtor_label_tabel (Label_table *label_table)
{
    assert (label_table != nullptr && "str is nullptr");

    label_table->cnt_labels      = -1;
    label_table->label_capacity  = -1;

    if (Check_nullptr (label_table->labels))
    {
        Log_report ("An error occurred in Label_tabel deconstruction\n");
        Err_report ();
    }
    else
        free (label_table->labels);

    return 0;
}