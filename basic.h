#ifndef BASIC_H
#define BASIC_H

#include <stddef.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    size_t length;
    size_t capacity;
    char* data;
} StringBuilder;

void sb_resize(StringBuilder* sb, size_t new_capacity);

#define sb_push(sb, val) _Generic((val), \
                 char*: sb_push_str, \
	         int: sb_push_int, \
                 char: sb_push_char, \
	         float: sb_push_float \
               ) (sb, val)
	         

void sb_push_str(StringBuilder* sb, char* str);
void sb_push_char(StringBuilder* sb, char ch);
void sb_push_int(StringBuilder* sb, int i);
void sb_push_float(StringBuilder* sb, float f);
StringBuilder sb_clone(StringBuilder* sb);


typedef struct {
    size_t length;
    char* data;
} StringView;

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int) (sv).length, (sv).data

StringView sv_from_parts(char* str, size_t len);
StringView sv_from_cstr(char* str);
StringView sv_from_sb(StringBuilder* sb);
bool sv_equal(StringView s1, StringView s2);
void sv_trim_left(StringView* sv);
void sv_trim_right(StringView* sv);
void sv_trim(StringView* sv);
StringView sv_chop_delim(StringView* sv, char delim);
StringView sv_chop_str(StringView* sv, char* str);

#endif // BASIC_H

