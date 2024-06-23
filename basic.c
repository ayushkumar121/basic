#include "./basic.h"

/* Error Handling */

Error error(char* message) {
  return (Error){.message = message};
}

bool has_error(Error err) {
  return err.message != NULL;
}

/* String Builder */

void sb_resize(StringBuilder *sb, size_t new_capacity) {
  sb->capacity = new_capacity;
  sb->items = MEM_REALLOC(sb->items, sb->capacity + 1);
}

void sb_free(StringBuilder *sb) { array_free(sb); }

void sb_push_str(StringBuilder *sb, char *str) {
  size_t item_len = strlen(str);
  if (sb->capacity < (sb->length + item_len + 1)) {
    sb_resize(sb, sb->capacity + item_len);
  }

  memcpy(sb->items + sb->length, str, item_len);
  sb->length += item_len;
  sb->items[sb->length] = 0;
}

void sb_push_char(StringBuilder *sb, char ch) {
  if (sb->capacity < (sb->length + 2)) {
    sb_resize(sb, sb->capacity + 1);
  }

  sb->items[sb->length] = ch;
  sb->length += 1;
  sb->items[sb->length] = 0;
}

void sb_push_int(StringBuilder *sb, int i) {
  if (i == 0) {
    sb_push_char(sb, '0');
    return;
  }

  bool is_neg = i < 0;
  if (is_neg)
    i *= -1;

  int k = (is_neg) ? log10(i) + 2 : log10(i) + 1;
  sb_resize(sb, sb->capacity + k);

  int j = k;
  while (i != 0) {
    sb->items[sb->length + j - 1] = i % 10 + '0';
    i = i / 10;
    j -= 1;
  }

  if (is_neg)
    sb->items[sb->length] = '-';
  sb->length += k;
  sb->items[sb->length] = 0;
}

void sb_push_double(StringBuilder *sb, double d) {
  // Handling int part
  int i = (d < 0) ? ceil(d) : floor(d);
  sb_push_int(sb, i);

  sb_push_char(sb, '.');

  // Handling fractional part
  int f = (d - i) * 1000000;
  if (f < 0)
    f *= -1;
  sb_push_int(sb, f);
}

StringBuilder sb_clone(StringBuilder *sb) {
  StringBuilder clone = {0};
  sb_push_str(&clone, sb->items);
  return clone;
}

/* String View */

StringView sv_from_parts(char *str, size_t len) {
  return (StringView){.length = len, .items = str};
}

StringView sv_from_cstr(char *str) {
  StringView sv = {0};
  sv.items = str;
  sv.length = strlen(str);
  return sv;
}

StringView sv_from_sb(StringBuilder *sb) {
  StringView sv = {0};
  sv.items = sb->items;
  sv.length = sb->length;
  return sv;
}

bool sv_equal(StringView s1, StringView s2) {
  if (s1.length != s2.length)
    return false;

  for (size_t i = 0; i < s1.length; i++) {
    if (s1.items[i] != s2.items[i])
      return false;
  }

  return true;
}

StringView sv_trim_left(StringView sv) {
  StringView result = sv;
  while (isspace(*result.items)) {
    result.items++;
    result.length--;
  }
  return result;
}

StringView sv_trim_right(StringView sv) {
  StringView result = sv;
  while (isspace(result.items[result.length - 1]))
    result.length--;
  return result;
}

StringView sv_trim(StringView sv) {
  StringView result = sv_trim_left(sv);
  result = sv_trim_right(sv);

  return result;
}

StringView sv_chop_delim(StringView *sv, char delim) {
  size_t i = 0;
  while (sv->items[i] != delim && i < sv->length)
    i++;

  StringView result = {0};
  result.items = sv->items;
  result.length = i;

  sv->items = sv->items + i + 1;
  sv->length = sv->length - i;

  return result;
}

StringView sv_chop_str(StringView *sv, char *str) {
  size_t n = strlen(str);
  StringView pat = sv_from_parts(str, n);
  StringView result = sv_from_parts(sv->items, 0);

  size_t i = 0;
  for (i = 0; i < sv->length - n; i++) {
    StringView target = sv_from_parts(&(sv->items[i]), n);
    if (sv_equal(pat, target))
      break;
  }

  result.length = i;

  sv->items = sv->items + i + n;
  sv->length = sv->length - i - n;

  return result;
}

/* Hash Table */

HashTable hash_table_init(size_t capacity, KeyEqFunc key_eq, KeyHashFunc key_hash) {
  assert(key_eq != NULL && "key_eq is required");
  assert(key_hash != NULL && "key_hash is required");

  HashTable v = {0};
  
  size_t sz = capacity*sizeof(TableEntry);
  v.entries = (TableEntry*)MEM_REALLOC(NULL, sz);
  v.capacity = capacity;
  v.key_eq = key_eq;
  v.key_hash = key_hash;

  memset(v.entries, 0, sz);
  return v;
}

Error hash_table_insert(HashTable* v, void* key, void* val) {
  assert(v != NULL && "map cannot be null");

  if (key == NULL) return error("key is null");  
  if (val == NULL) return error("value is null");
  if (v->entries == NULL) return error("uninitialized map");

  size_t index = v->key_hash(v, key);
  bool index_found = false;

  for (size_t i = 0; i < v->capacity; ++i) {
    size_t try_index = (i+index)%v->capacity;
    TableEntry entry = v->entries[try_index];

    if (entry.key == NULL || v->key_eq(entry.key, key)) {
      index = try_index;
      index_found = true;
      break;
    }
  }
  if (!index_found) return error("map is full");

  v->entries[index].key = key;
  v->entries[index].value = val;
  v->length++;

  return NullError;
}

Error hash_table_get(HashTable* v, void* key, void** out) {
  assert(v != NULL && "map is null");

  if (key == NULL) return error("key is null");
  if (v->entries == NULL) return error("uninitialized map");

  size_t index = v->key_hash(v, key);
  bool index_found = false;

  for (size_t i = 0; i < v->capacity; ++i) {
    size_t try_index = (i+index)%v->capacity;
    TableEntry entry = v->entries[try_index];

    if (entry.key != NULL && v->key_eq(entry.key, key)) {
      index = try_index;
      index_found = true;
      break;
    }
  }
  if (!index_found) return error("key not found");
  if (out != NULL) {
    *out = v->entries[index].value;
  }
  
  return NullError;
}

Error hash_table_remove(HashTable* v, void* key, void** out) {
  assert(v != NULL && "map is null");

  if (key == NULL) return error("key is null");
  if (v->entries == NULL) return error("uninitialized map");

  size_t index = v->key_hash(v, key);
  bool index_found = false;

  for (size_t i = 0; i < v->capacity; ++i) {
    size_t try_index = (i+index)%v->capacity;
    TableEntry entry = v->entries[try_index];

  if (entry.key != NULL && v->key_eq(entry.key, key)) {
      index = try_index;
      index_found = true;
      break;
    }
  }
  if (!index_found) return error("key not found");

  if (out != NULL) {
    *out = v->entries[index].value;
  }

  v->entries[index].key = NULL;
  v->entries[index].value = NULL;
  v->length--;

  return NullError;
}

void hash_table_free(HashTable* v) {
  assert(v != NULL && "map is null");

  if (v->entries) {
    MEM_FREE(v->entries, v->capacity*sizeof(TableEntry));
    v->entries = NULL;
    v->capacity = 0;
  }
}