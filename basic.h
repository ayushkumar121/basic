#ifndef BASIC_H
#define BASIC_H

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char* message;
} Error;

#define NullError (Error){0};

Error error(char* message);
bool has_error(Error err);

#define MEM_REALLOC realloc
#define MEM_FREE(ptr, size) free(ptr)

#define ARRAY_INIT_CAP 10

#define array_append(array, item)                                              \
  do {                                                                         \
    if ((array)->capacity < (array)->length + 1) {                             \
      (array)->capacity =                                                      \
          ((array)->capacity == 0) ? ARRAY_INIT_CAP : (array)->capacity * 2;   \
      (array)->items = MEM_REALLOC(                                            \
          (array)->items, (array)->capacity * sizeof(*(array)->items));        \
    }                                                                          \
    (array)->items[(array)->length++] = (item);                                \
  } while (0)

#define array_free(array)                                                      \
  MEM_FREE((array)->items, (array)->capacity * sizeof(*((array)->items)))

typedef struct {
  size_t length;
  size_t capacity;
  char *items;
} StringBuilder;

void sb_resize(StringBuilder *sb, size_t new_capacity);
void sb_free(StringBuilder *sb);

#define sb_push(sb, val)                                                       \
  _Generic((val),                                                              \
      char *: sb_push_str,                                                     \
      char: sb_push_char,                                                      \
      int: sb_push_int,                                                        \
      float: sb_push_double,                                                   \
      double: sb_push_double)(sb, val)

void sb_push_str(StringBuilder *sb, char *str);
void sb_push_char(StringBuilder *sb, char ch);
void sb_push_int(StringBuilder *sb, int i);
void sb_push_double(StringBuilder *sb, double d);
StringBuilder sb_clone(StringBuilder *sb);

typedef struct {
  size_t length;
  char *items;
} StringView;

#define SV_Fmt "%.*s"
#define SV_Arg(sv) (int)(sv).length, (sv).items

StringView sv_from_parts(char *str, size_t len);
StringView sv_from_cstr(char *str);
StringView sv_from_sb(StringBuilder *sb);
bool sv_equal(StringView s1, StringView s2);
StringView sv_trim_left(StringView sv);
StringView sv_trim_right(StringView sv);
StringView sv_trim(StringView sv);
StringView sv_chop_delim(StringView *sv, char delim);
StringView sv_chop_str(StringView *sv, char *str);

typedef struct {
  void *key;
  void *value;
} TableEntry;

typedef bool (*KeyEqFunc) (void*, void*);
typedef size_t (*KeyHashFunc) (void*, void*);

typedef struct {
  TableEntry* entries;
  size_t length;
  size_t capacity;

  KeyEqFunc key_eq;
  KeyHashFunc key_hash;
} HashTable;

HashTable hash_table_init(size_t capacity, KeyEqFunc key_eq, KeyHashFunc key_hash);
Error hash_table_insert(HashTable* v, void* key, void* val);
Error hash_table_get(HashTable* v, void* key, void** out);
Error hash_table_remove(HashTable* v, void* key, void** out);
void hash_table_free(HashTable* v);

#endif // BASIC_H
