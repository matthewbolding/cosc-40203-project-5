/**
 * @file threads.h
 * @author Matthew Bolding; Griffin McPherson
 * @brief Contains function prototypes for threads.c.
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdbool.h>

#include "definitions.h"

int generate_random(int type);

void create_pthread(pthread_t *list, int size, int type, struct pthread_arg *pthread_arg);
void join_pthreads(pthread_t *list, int size);

void *faulty_producer(void *data);
void *functional_producer(void *data);
void *consumer(void *data);

bool is_prime(int n);
void free_structures(int *array);