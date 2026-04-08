#ifndef MAPREDUCE_H
#define MAPREDUCE_H

#include <stddef.h>

#define MAX_KEY_LEN   256
#define MAX_VAL_LEN   1024
#define MAX_KV_COUNT  1000000

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VAL_LEN];
} KeyValue;

typedef struct {
    KeyValue *pairs;
    size_t    count;
    size_t    capacity;
} KVList;

typedef void (*MapFunc)(const char *key, const char *value, KVList *output);
typedef void (*ReduceFunc)(const char *key, KVList *values, KVList *output);

typedef struct {
    MapFunc    map;
    ReduceFunc reduce;
    const char *input_file;
    const char *output_file;
    int         num_threads;
} Job;

KVList *kvlist_create(size_t initial_capacity);
void    kvlist_append(KVList *list, const char *key, const char *value);
void    kvlist_free(KVList *list);

int run_job(Job *job);

#endif