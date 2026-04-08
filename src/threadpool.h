#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "mapreduce.h"

KVList *parallel_map(Job *job, KVList *input);

#endif