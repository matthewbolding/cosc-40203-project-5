/**
 * @file definitions.h
 * @author Matthew Bolding; Griffin McPherson
 * @brief Contains global definitions of nearly all variables.
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

#define FNCTNL_PROD 1
#define FAULTY_PROD 2
#define CONSUMER    3

#define FNCTNL_PROD_MIN 2
#define FAULTY_PROD_MIN 2
#define FNCTNL_PROD_MAX 999999
#define FAULTY_PROD_MAX 499999

#ifndef ARGUMENTS_TYPEDEF
#define ARGUMENTS_TYPEDEF

struct arguments {
    int items, length, producer, faulty, consumer;
    bool debug;
};

#endif

#ifndef PTHREAD_ARG_TYPEDEF
#define PTHREAD_ARG_TYPEDEF

struct pthread_arg {
    struct arguments *prog_arg;
    sem_t mutex;
    sem_t empty;
    sem_t full;
    int *buffer;
    int in;
    int out;
};

#endif

pthread_t *producer_arr;
pthread_t *faulty_arr;
pthread_t *consumer_arr;

int num_items_per_producer, buffer_size, num_producers, num_faulty;
int volatile num_consumer, num_full, num_empty, num_nonprimes, total_consumed;

int volatile *tid_functional,      *tid_faulty,      *tid_consumer, 
             *functional_produced, *faulty_produced, *consumed_thread;

struct timespec now_t;
struct timeval time_start, time_end, time_elapsed;