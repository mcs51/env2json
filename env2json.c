#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "json.h"


extern char **environ;

enum Action { DUMP_ENV, DUMP_ARRAY };

void usage(const char* progname)
{
  puts("Usage: ");
  printf("%s\n", progname);
  printf("%s -a KEYS_PREFIXVAR VALUES_PREFIXVAR\n", progname);
}

void dump_env()
{
  JsonNode* env = json_mkobject();

  for (char** e = environ; *e; e++) {
    char* name = *e;
    char* value = *e;
    while (*value++ != '=');
    value[-1] = '\0';
    json_append_member(env, name, json_mkstring(value));
  }

  char* env_str = json_stringify(env, "  ");

  puts(env_str);
}

char* env_find(const char* key)
{
  for (char** e = environ; *e; e++) {
    char* name = *e;
    char* value = *e;
    while (*value++ != '=');
    size_t name_len = value-name-1;

    if (name_len != strlen(key))
      continue;

    if (strncmp(key, name, name_len) == 0) return value;
  }

  return NULL;
}

void dump_array(const char* akeys, const char* avalues)
{
  JsonNode* a = json_mkobject();
  size_t akeys_len = strlen(akeys);
  size_t avals_len = strlen(avalues);

  for (int n = 0; ; n++) {
    char* key = malloc(akeys_len+16);
    sprintf(key, "%s%d", akeys, n);
    char* key_str = env_find(key);

    if (key_str == NULL) break;

    char* val = malloc(avals_len+16);
    sprintf(val, "%s%d", avalues, n);
    char* val_str = env_find(val);

    if (val_str == NULL) break;

    json_append_member(a, key_str, json_mkstring(val_str));
  }

  char* a_str = json_stringify(a, "  ");

  puts(a_str);
}

int main(int argc, char* argv[])
{
  enum Action action = DUMP_ENV;
  char *akeys, *avalues;

  for (char** arg = &argv[1]; *arg; arg++) {
    if (strcmp(*arg, "-a") == 0) {
      action = DUMP_ARRAY;

      if(*(++arg) == NULL) {
        fprintf(stderr, "Error: -a requires value\n");
        usage(argv[0]);
        return 1;
      }

      akeys = *arg;

      if(*(++arg) == NULL) {
        fprintf(stderr, "Error: -a requires value\n");
        usage(argv[0]);
        return 1;
      }

      avalues = *arg;
    }
  }

  switch (action) {
  case DUMP_ENV: dump_env(); break;
  case DUMP_ARRAY: dump_array(akeys, avalues); break;
  }

  return 0;
}

