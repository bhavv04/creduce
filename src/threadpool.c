#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "mapreduce.h"
#include "threadpool.h"

typedef struct {
    Job    *job;
    KVList *input;
    size_t  start;
    size_t  end;
    KVList *output;
} MapWorkerArgs;

static void *map_worker(void *arg) {
    MapWorkerArgs *args = (MapWorkerArgs *)arg;

    for (size_t i = args->start; i < args->end; i++) {
        args->job->map(
            args->input->pairs[i].key,
            args->input->pairs[i].value,
            args->output
        );
    }

    return NULL;
}

KVList *parallel_map(Job *job, KVList *input) {
    int num_threads = job->num_threads;
    if (num_threads <= 0) num_threads = 1;

    pthread_t      *threads   = malloc(sizeof(pthread_t) * num_threads);
    MapWorkerArgs  *args      = malloc(sizeof(MapWorkerArgs) * num_threads);
    KVList        **partials  = malloc(sizeof(KVList *) * num_threads);

    size_t chunk = input->count / num_threads;

    for (int t = 0; t < num_threads; t++) {
        partials[t]         = kvlist_create(chunk * 4 + 64);
        args[t].job         = job;
        args[t].input       = input;
        args[t].start       = t * chunk;
        args[t].end         = (t == num_threads - 1) ? input->count : (t + 1) * chunk;
        args[t].output      = partials[t];

        pthread_create(&threads[t], NULL, map_worker, &args[t]);
    }

    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }

    KVList *merged = kvlist_create(input->count * 4);
    for (int t = 0; t < num_threads; t++) {
        for (size_t i = 0; i < partials[t]->count; i++) {
            kvlist_append(merged, partials[t]->pairs[i].key, partials[t]->pairs[i].value);
        }
        kvlist_free(partials[t]);
    }

    free(threads);
    free(args);
    free(partials);

    return merged;
}