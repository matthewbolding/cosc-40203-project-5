/**
 * @file prod-com.c
 * @author Matthew Bolding; Griffin McPherson
 * @brief Contains the main function.
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

#include <time.h>
#include <sys/time.h>

#include <unistd.h>
#include <signal.h>
#include <sys/syscall.h>

#include "input.h"
#include "threads.h"
#include "output.h"

/**
 * @brief Main function and entrance into the program.
 * 
 */
int main (int argc, char **argv) {
    // Get the command line arguments.
    struct arguments arguments = get_arguments(argc, argv);

    // Create the argument that gets passed to all pthreads.
    struct pthread_arg thread_arg;

    sem_init(&thread_arg.mutex, 0, 1);
    sem_init(&thread_arg.empty, 0, arguments.length);
    sem_init(&thread_arg.full, 0, 0);

    thread_arg.in = 0;
    thread_arg.out = 0;
    thread_arg.prog_arg = &arguments;
    thread_arg.buffer = calloc(sizeof(int), arguments.length);

    // Initialize buffer and simulation statistics.
    initialize_stats(arguments.items, arguments.length, arguments.producer, arguments.faulty, arguments.consumer);

    // Start timer.
    gettimeofday(&time_start, NULL);

    // Create the threads.
    producer_arr = calloc(sizeof(pthread_t), arguments.producer);
    faulty_arr = calloc(sizeof(pthread_t), arguments.faulty);
    consumer_arr = calloc(sizeof(pthread_t), arguments.consumer);

    printf("Starting Threads...\n\n");
    create_pthread(producer_arr, arguments.producer, FNCTNL_PROD, &thread_arg);
    create_pthread(faulty_arr, arguments.faulty, FAULTY_PROD, &thread_arg);
    create_pthread(consumer_arr, arguments.consumer, CONSUMER, &thread_arg);

    // Join the threads.
    join_pthreads(producer_arr, arguments.producer);
    join_pthreads(faulty_arr, arguments.faulty);
    join_pthreads(consumer_arr, arguments.consumer);

    // Get time after all threads have exited.
    gettimeofday(&time_end, NULL);

    display_stats();

    free_structures(thread_arg.buffer);
}