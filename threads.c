/**
 * @file threads.c
 * @author Matthew Bolding; Griffin McPherson
 * @brief Contains implemented functions pertaining to threads.
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "output.h"
#include "threads.h"
#include "definitions.h"

/**
 * @brief Helper function to generate a random number for a type of thread.
 * 
 * @param type the type of thread
 * @return int random number on a dependent domain
 */
int generate_random(int type) {
    // The seed is set to the nanosecond of the system clock.
    clock_gettime(0, &now_t);
    srand48(now_t.tv_nsec);

    double num = drand48();
    int min, max, range, output;

    if(type == FNCTNL_PROD) {
        min = FNCTNL_PROD_MIN;
        max = FNCTNL_PROD_MAX;
        range = max - min + 1;

        output = (int) ((num * range) + min + 1e-9);
    } else if(type == FAULTY_PROD) {
        // Note that the minimum and maximum values for this type covers exactly half the range.
        // Since the number must be even, simply multiply by 2.
        min = FAULTY_PROD_MIN;
        max = FAULTY_PROD_MAX;
        range = max - min + 1;

        output = ((int) ((num * range) + min + 1e-9)) * 2;
    }

    return output;
}

/**
 * @brief Entrance function for threads of type faulty producer.
 * 
 * @param data a struct pthread_arg
 * @return void* not in use
 */
void *faulty_producer(void *data) {
    int i, number;
    struct pthread_arg *args = (struct pthread_arg *) data;

    // Get the TID.
    int tid = syscall(SYS_gettid);

    // Generate an amount of random even numbers, as specified by the pthread arguments.
    for(i = 0; i < args->prog_arg->items; i++) {
        number = generate_random(FAULTY_PROD);
        
        sem_wait(&args->empty);
        sem_wait(&args->mutex);

        // Put the number in the buffer, and update in.
        args->buffer[args->in] = number;
        args->in = (args->in + 1) % args->prog_arg->length;

        // Print an update.
        print_update(FAULTY_PROD, number, tid, args);

        sem_post(&args->mutex);
        sem_post(&args->full);
    }

    // Exit, after all numbers have been generated.
    pthread_exit(0);
}

/**
 * @brief Entrance function for threads of type functional producer.
 * 
 * @param data a struct pthread_arg
 * @return void* not in use
 */
void *functional_producer(void *data) {
    int i, number;
    struct pthread_arg *args = (struct pthread_arg *) data;

    // Get the TID.
    int tid = syscall(SYS_gettid);
    
    // Generate an amount of primee numbers, as specified by the pthread arguments.
    for(i = 0; i < args->prog_arg->items; i++) {
        // Generate random numbers until number is prime.
        do {
            number = generate_random(FNCTNL_PROD);
        } while (!is_prime(number));

        sem_wait(&args->empty);
        sem_wait(&args->mutex);

        // Put the number in the buffer, and update in.
        args->buffer[args->in] = number;
        args->in = (args->in + 1) % args->prog_arg->length;

        // Print an update.
        print_update(FNCTNL_PROD, number, tid, args);

        sem_post(&args->mutex);
        sem_post(&args->full);
    }

    // Exit, after all numbers have been generated.
    pthread_exit(0);
}

/**
 * @brief Entrance function for threads of type consumer.
 * 
 * @param data a struct pthread_arg
 * @return void* not in use
 */
void *consumer(void *data) {
    int number;
    
    struct pthread_arg *args = (struct pthread_arg *) data;
    int total_produced = args->prog_arg->items * (args->prog_arg->producer + args->prog_arg->faulty);

    // Get the TID
    int tid = syscall(SYS_gettid);

    while(total_consumed < total_produced) {
        sem_wait(&args->full);
        sem_wait(&args->mutex);

        if(total_consumed == total_produced) {
            pthread_exit(0);
        }

        // Remove an item from the buffer, set that index to 0, and update out.
        number = args->buffer[args->out];
        args->buffer[args->out] = 0;
        args->out = (args->out + 1) % args->prog_arg->length;
        
        // Print an update.
        print_update(CONSUMER, number, tid, args);
        
        // Keep track of the number of items consumed.
        total_consumed++;

        sem_post(&args->mutex);
        sem_post(&args->empty);
        
        // At this point, only the thread executing this statement will
        // not be in a waiting state from the semaphore. All the pthreads
        // must be cancelled to make them all return.
        if(total_consumed == total_produced) {
            for(int i = 0; i < num_consumer; i++) {
                pthread_cancel(consumer_arr[i]);
            }
        }
    }

    pthread_exit(0);
}

/**
 * @brief Returns true if n is prime and false otherwise.
 * 
 * @param n the number
 * @return true if n is prime
 * @return false if n is not prime
 */
bool is_prime(int n) {
    int i;

    if(n < 2) return false;

    for(i = 2; i <= sqrt(n); i++) {
        if(n % i == 0) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Helper function to call pthread_create on all pthreads in a given pthread_t array.
 * 
 * @param list the array
 * @param size size of the array
 * @param type the type of pthread, i.e., CONSUMER, FNCTNL_PROD, or FAULTY_PROD
 * @param thread_arg the thread's argument
 */
void create_pthread(pthread_t *list, int size, int type, struct pthread_arg *thread_arg) {
    int i;
    void *function;

    // The function depends on the type of thread.
    if(type == CONSUMER) function = consumer;
    else if(type == FNCTNL_PROD) function = functional_producer;
    else if(type == FAULTY_PROD) function = faulty_producer;

    for(i = 0; i < size; i++) {
        pthread_create(&list[i], NULL, function, (void *) thread_arg);
    }
}

/**
 * @brief Helper function to call pthread_join on all pthreads of an array.
 * 
 * @param list the pthread_t array
 * @param size size of the array
 */
void join_pthreads(pthread_t *list, int size) {
    int i;

    for(i = 0; i < size; i++) {
        pthread_join(list[i], NULL);
    }
}

/**
 * @brief Helper function to free all data structures allocated with calloc().
 * 
 * @param array the buffer array
 */
void free_structures(int *array) {
    free(producer_arr); free(faulty_arr); free(consumer_arr);
    free((int*) tid_functional); free((int*) tid_faulty); free((int*) tid_consumer);
    free((int*) functional_produced); free((int*) faulty_produced); free((int*) consumed_thread);
    free(array);
}