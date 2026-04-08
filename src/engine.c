#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mapreduce.h"

KVList *kvlist_create(size_t initial_capacity) {
    KVList *list = malloc(sizeof(KVList));
    if (!list) return NULL;

    list->pairs = malloc(sizeof(KeyValue) * initial_capacity);
    if (!list->pairs) {
        free(list);
        return NULL;
    }

    list->count    = 0;
    list->capacity = initial_capacity;
    return list;
}

void kvlist_append(KVList *list, const char *key, const char *value) {
    if (list->count >= list->capacity) {
        list->capacity *= 2;
        list->pairs = realloc(list->pairs, sizeof(KeyValue) * list->capacity);
    }

    strncpy(list->pairs[list->count].key,   key,   MAX_KEY_LEN - 1);
    strncpy(list->pairs[list->count].value, value, MAX_VAL_LEN - 1);
    list->pairs[list->count].key[MAX_KEY_LEN - 1]   = '\0';
    list->pairs[list->count].value[MAX_VAL_LEN - 1] = '\0';
    list->count++;
}

void kvlist_free(KVList *list) {
    if (!list) return;
    free(list->pairs);
    free(list);
}

static int kv_compare(const void *a, const void *b) {
    return strcmp(((KeyValue *)a)->key, ((KeyValue *)b)->key);
}

static KVList *read_input(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "error: cannot open input file %s\n", filename);
        return NULL;
    }

    KVList *input = kvlist_create(1024);
    char line[MAX_VAL_LEN];
    size_t line_num = 0;
    char key[MAX_KEY_LEN];

    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        snprintf(key, sizeof(key), "%zu", line_num++);
        kvlist_append(input, key, line);
    }

    fclose(f);
    return input;
}

static KVList *map_phase(Job *job, KVList *input) {
    KVList *mapped = kvlist_create(input->count * 4);

    for (size_t i = 0; i < input->count; i++) {
        job->map(input->pairs[i].key, input->pairs[i].value, mapped);
    }

    return mapped;
}

static void shuffle_phase(KVList *mapped) {
    qsort(mapped->pairs, mapped->count, sizeof(KeyValue), kv_compare);
}

static KVList *reduce_phase(Job *job, KVList *shuffled) {
    KVList *output = kvlist_create(shuffled->count);
    if (shuffled->count == 0) return output;

    size_t i = 0;
    while (i < shuffled->count) {
        const char *current_key = shuffled->pairs[i].key;

        size_t j = i;
        while (j < shuffled->count && strcmp(shuffled->pairs[j].key, current_key) == 0) {
            j++;
        }

        KVList group;
        group.pairs    = &shuffled->pairs[i];
        group.count    = j - i;
        group.capacity = j - i;

        job->reduce(current_key, &group, output);
        i = j;
    }

    return output;
}

static void write_output(const char *filename, KVList *output) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "error: cannot open output file %s\n", filename);
        return;
    }

    for (size_t i = 0; i < output->count; i++) {
        fprintf(f, "%s\t%s\n", output->pairs[i].key, output->pairs[i].value);
    }

    fclose(f);
}

int run_job(Job *job) {
    printf("[creduce] reading input: %s\n", job->input_file);
    KVList *input = read_input(job->input_file);
    if (!input) return -1;

    printf("[creduce] map phase — %zu records\n", input->count);
    KVList *mapped = map_phase(job, input);
    kvlist_free(input);

    printf("[creduce] shuffle phase — %zu pairs\n", mapped->count);
    shuffle_phase(mapped);

    printf("[creduce] reduce phase\n");
    KVList *output = reduce_phase(job, mapped);
    kvlist_free(mapped);

    printf("[creduce] writing output: %s\n", job->output_file);
    write_output(job->output_file, output);
    kvlist_free(output);

    printf("[creduce] done\n");
    return 0;
}