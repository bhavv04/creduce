#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/mapreduce.h"

static void wordcount_map(const char *key, const char *value, KVList *output) {
    (void)key;

    char line[MAX_VAL_LEN];
    strncpy(line, value, MAX_VAL_LEN - 1);
    line[MAX_VAL_LEN - 1] = '\0';

    char *token = strtok(line, " \t\r\n.,!?;:\"'()-[]{}");
    while (token != NULL) {
        kvlist_append(output, token, "1");
        token = strtok(NULL, " \t\r\n.,!?;:\"'()-[]{}");
    }
}

static void wordcount_reduce(const char *key, KVList *values, KVList *output) {
    int count = 0;
    for (size_t i = 0; i < values->count; i++) {
        count += atoi(values->pairs[i].value);
    }

    char result[32];
    snprintf(result, sizeof(result), "%d", count);
    kvlist_append(output, key, result);
}

int main(void) {
    Job job = {
        .map         = wordcount_map,
        .reduce      = wordcount_reduce,
        .input_file  = "data/input.txt",
        .output_file = "data/output.txt",
        .num_threads = 1,
    };

    return run_job(&job);
}