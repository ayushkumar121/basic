#include "./basic.h"

#define CRLF "\r\n"

bool int_key_eq(void* a, void* b) {
  int* key1=a;
  int* key2=b;

  return *key1 == *key2;
}

size_t int_key_hash(void* a, void *b) {
  HashTable* table = a;
  int* key = b;

  return (*key)%table->capacity;
}

int main(void) {
  char *http_res =
      ""
      "HTTP/2 200" CRLF "access-control-allow-credentials: true" CRLF
      "access-control-allow-headers: Origin, X-Requested-With, Content-Type, "
      "Accept" CRLF "access-control-allow-methods: GET, POST, PUT, PATCH, "
                    "DELETE, OPTIONS, HEAD" CRLF
      "access-control-allow-origin: *" CRLF "age: 1" CRLF
      "cache-control: no-cache" CRLF "content-type: application/json" CRLF
      "date: Mon, 15 Jan 2024 17:56:22 GMT" CRLF "netlify-vary: query" CRLF
      "server: Netlify" CRLF "strict-transport-security: max-age=31536000" CRLF
      "x-nf-request-id: 01HM748V8STNF5ZM0CM13FZVBB" CRLF "" CRLF
      "{\"message\":\"Hello World\"}";

  StringView sv = sv_from_cstr(http_res);

  StringView status_line = sv_chop_str(&sv, CRLF);
  StringView version = sv_chop_delim(&status_line, ' ');
  StringView status_code = sv_chop_delim(&status_line, ' ');

  printf("Version:%.*s\n", SV_Arg(version));
  printf("Code:%.*s\n", SV_Arg(status_code));

  printf("Decoding Headers: \n");

  StringView header_line = sv_chop_str(&sv, CRLF);
  while (header_line.length != 0) {
    StringView header_key = sv_chop_delim(&header_line, ':');

    header_key = sv_trim(header_key);
    header_line = sv_trim(header_line);

    printf("Header Key:%.*s\n", SV_Arg(header_key));
    printf("Header Value:%.*s\n", SV_Arg(header_line));

    header_line = sv_chop_str(&sv, CRLF);
  }

  printf("Body: %.*s\n", SV_Arg(sv));

  printf("\nString Builder Test:\n");

  StringBuilder sb = {0};
  sb_push(&sb, "Hello");
  sb_push(&sb, -4.5);
  sb_push(&sb, "World\n");

  printf("String Builder Test:%.*s\n", SV_Arg(sb));
  sb_free(&sb);

  printf("\nArray Test:\n");

  typedef struct {
    size_t length;
    size_t capacity;
    int *items;
  } IntArray;

  IntArray arr = {0};

  array_append(&arr, 22);
  array_append(&arr, 33);
  array_append(&arr, 44);

  for (size_t i = 0; i < arr.length; i++) {
    printf("%d\n", arr.items[i]);
  }

  printf("\nHashTable Test:\n");
  HashTable table = hash_table_init(10, int_key_eq, int_key_hash);

  int x = 1, y = 2, z = 3;

  hash_table_insert(&table, &x, "X");
  hash_table_insert(&table, &y, "Y");
  hash_table_insert(&table, &z, "Z");

  for (size_t i = 0; i < table.capacity; ++i) {
    TableEntry entry = table.entries[i];
    if (entry.key != NULL) {
      printf("\t%d => %s\n", *(int*)entry.key, (char*)entry.value);
    } else {
      printf("\t___\n");  
    }
  }

  Error err = NullError;

  err = hash_table_get(&table, &x, NULL);
  if (!has_error(err)) {
    printf("Found x\n");
  }

  err = hash_table_get(&table, &y, NULL);
  if (!has_error(err)) {
    printf("Found y\n");
  }
  
  err = hash_table_get(&table, &z, NULL);
  if (!has_error(err)) {
    printf("Found z\n");
  }


  array_free(&arr);
}
