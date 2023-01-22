#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>	
#include <stdint.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>


#include "generals.h"

//======================================================================================

bool Check_num (const char *str)
{
	assert (str != nullptr && "str is nullptr");

	if (str[0] == '-') str++;
	while (*str != '\0')
	{
		if (isdigit (*str) || *str == '.')
			str++;
		else
			return 0;
	}

	return 1;
}

//======================================================================================

void My_flush ()
{
	char ch = 0;
	while ((ch = (char)getchar()) != EOF && ch != '\n')
			continue;
}

//======================================================================================

bool Equality_double (double num1, double num2){
    is_error (isfinite (num1));
    is_error (isfinite (num2));

    return fabs (num1-num2) < Eps;
}

bool Is_zero (double num){
    is_error (isfinite (num));

    return Equality_double (num, 0);
}

//======================================================================================

double Fix_zero (double num){
    is_error (isfinite (num));

    if (Is_zero (num))
        return 0.0;
    return num;
}

//======================================================================================

int Clear_data (unsigned char *cmd_hash_tabel, size_t size_data)
{
    assert (cmd_hash_tabel != nullptr && "cmd_hash_tabel is nullptr");

    for (int ip = 0; ip < size_data; ip++)
        cmd_hash_tabel[ip] = 0;

    return 0;
}

//======================================================================================

int Bin_represent(FILE *fpout, size_t elem, uint64_t size_elem)
{
	assert (fpout != nullptr && "fpot is nullptr");
	
    for (uint64_t num_bit = size_elem * 4; num_bit > 0; num_bit--) {
        fprintf(fpout, "%d", (elem & (1 << (num_bit-1))) ? 1 : 0);
    }

    return 0;
}

//======================================================================================

uint64_t Get_hash (const char *data, uint64_t len) 
{
	assert (data != nullptr && "data is nullptr");

    uint64_t hash = 0;

    for (uint64_t num_bit = 0; num_bit < len; num_bit++) 
    {
        hash += (unsigned char) data[num_bit];
        hash += (hash << 10);
        hash ^= (hash >> 6);
	}

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
	
	return hash;
}

//======================================================================================

void Print_colour (char const colour[], char const *str, ...){
    printf ("%s", colour);

    va_list arg_ptr;

    va_start (arg_ptr, str);
    vprintf (str, arg_ptr);
    va_end (arg_ptr);

    printf ("%s", RESET);
}

//======================================================================================

FILE *Open_file_ptr (const char *name_file, const char *mode)
{
	assert (name_file != nullptr && "name open file is nullptr");
	assert (mode != nullptr && "specifier mod open file is nullptr");

    FILE *fp = fopen (name_file, mode);
    if (!fp){
		errno = ENOENT;
        fprintf (stderr, "Could't open file %s with mode: %s\n", name_file, mode);
		perror ("\n");

		return nullptr;
    }

    return fp;
}

//======================================================================================

char Close_file_ptr (FILE *fp){
	assert (fp != nullptr && "FILE is nullptr");

	if (fclose(fp)){
		fprintf (stderr, "FILE does not close %p\n", fp);
		return ERR_FILE_CLOSE;
	}

    return 0;
}

//======================================================================================

int Open_file_discriptor (const char *name_file, const int mode){
	assert (name_file != nullptr && "name open file is nullptr");

	int fd = open (name_file, mode);
    if (fd < 0){
		errno = ENOENT;
        fprintf (stderr, "Could't get handel of file %s with mode: %d\n", name_file, mode);
		perror ("\n");

		return ERR_FILE_OPEN;
    }

    return fd;
}

//======================================================================================

char Close_file_discriptor (int fd){
	assert (fd  >= 0 && "discriptor is a negative number");

	if (close(fd)){
		fprintf (stderr, "FILE does not close %d\n", fd);
		return ERR_FILE_CLOSE;
	}

    return 0;
}

//======================================================================================

int My_swap (void *obj1, void *obj2, size_t size_type){                         
    assert (obj1 != NULL && "obj1 is NULL");
	assert (obj2 != NULL && "obj2 is NULL");

	char* _obj1  = (char*) obj1;
	char* _obj2  = (char*) obj2;

	while (size_type >= sizeof (int64_t)) {
		int64_t temp       = *(int64_t*) _obj1;
		*(int64_t*) _obj1  = *(int64_t*) _obj2;
		*(int64_t*) _obj2  = temp;

		_obj1 += sizeof (int64_t);
		_obj2 += sizeof (int64_t);

		size_type -= sizeof (int64_t);
	}

	while (size_type >= sizeof (int32_t)) {
		int32_t temp       = *(int32_t*) _obj1;
		*(int32_t*) _obj1  = *(int32_t*) _obj2;
		*(int32_t*) _obj2  = temp;

		_obj1 += sizeof (int32_t);
		_obj2 += sizeof (int32_t);

		size_type -= sizeof (int32_t);
	}

	while (size_type >= sizeof (int16_t)) {
		int16_t temp       = *(int16_t*) _obj1;
		*(int16_t*) _obj1  = *(int16_t*) _obj2;
		*(int16_t*) _obj2  = temp;

		_obj1 += sizeof (int16_t);
		_obj2 += sizeof (int16_t);

		size_type -= sizeof (int16_t);
	}

	while (size_type >= sizeof (int8_t)) {
		int8_t temp       = *(int8_t*) _obj1;
		*(int8_t*) _obj1  = *(int8_t*) _obj2;
		*(int8_t*) _obj2  = temp;

		_obj1 += sizeof (int8_t);
		_obj2 += sizeof (int8_t);

		size_type -= sizeof (int8_t);
	}

	return 0;
}


//======================================================================================

char* My_strndup (const char *str, const int len)
{
	assert (str != nullptr && "str is nullptr");
	assert (len > 0 && "len is not positive number");

	char *dup = (char*) calloc (len + 1, sizeof (char));

	if (!Check_nullptr (dup))
		dup = strncpy (dup, str, len);

	return dup;
}

//======================================================================================

int Factorial (int ord)
{
	if (ord < 0)
		return Inf;

	if (ord == 0)
		return 1;
	
	return ord * Factorial (ord - 1);
}

//======================================================================================