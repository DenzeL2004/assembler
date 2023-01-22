#ifndef _PROCESS_TEXT_H_
#define _PROCESS_TEXT_H_

#include <stdio.h>

struct Line 
{
    char *str = nullptr;
    int len_str = 0;
    int id = 0;
};

struct Text_info {
    
    size_t text_size = 0; 
    char *text_buf = nullptr;
    
    size_t cnt_lines = 0;
    Line *lines = nullptr;
};

int Text_read    (int fdin, Text_info *text); 

int Text_write   (FILE *fpout, int cnt_lines, Line *lines);

int Free_buffer  (Text_info *text);

#endif