/**
 * @file output.c
 * @author Matthew Bolding; Griffin McPherson
 * @brief Implements functions relating to the program's output.
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "threads.h"
#include "output.h"
#include "definitions.h"

/**
 * @brief Helper function to initalize the program's counters, arrays, and general information.
 * 
 * @param items the number of items each producer thread produces
 * @param length the length of the buffer
 * @param producers the number of producer threads
 * @param faulties the number of faulty producer threads
 * @param consumers the number of consumer threads
 */
void initialize_stats(int items, int length, int producers, int faulties, int consumers) {
    num_items_per_producer = items;
    buffer_size = length;
    num_producers = producers;
    num_faulty = faulties;
    num_consumer = consumers;

    tid_consumer = calloc(sizeof(int), consumers);
    tid_faulty = calloc(sizeof(int), faulties);
    tid_functional = calloc(sizeof(int), producers);

    functional_produced = calloc(sizeof(int), producers);
    faulty_produced = calloc(sizeof(int), faulties);
    consumed_thread = calloc(sizeof(int), consumers);
}

/**
 * @brief Helper function to print the program's final statistics.
 * 
 */
void display_stats() {
    printf("\nPRODUCER / CONSUMER SIMULATION COMPLETE\n");
    printf("=======================================\n");
    printf("Number of Items Per Producer Thread: %d\n", num_items_per_producer);
    printf("Size of Buffer: %d\n", buffer_size);
    printf("Number of Producer Threads: %d\n", num_producers);
    printf("Number of Faulty Producer Threads: %d\n", num_faulty);
    printf("Number of Consumer Threads: %d\n", num_consumer);

    printf("\nNumber of Times Buffer Became Full %d\n", num_full);
    printf("Number of Times Buffer Became Empty %d\n", num_empty);

    printf("\nNumber of Non-primes Detected %d\n", num_nonprimes);
    printf("Total Number of Items Consumed: %d\n", total_consumed);
    
    int i;
    for(i = 0; i < num_consumer; i++) {
        printf("  Thread %d: %d\n", i + 1, consumed_thread[i]); // Will need an array here, likely.
    }
    timersub(&time_end, &time_start, &time_elapsed);
    printf("\nTotal Simulation Time: %ld.%06ld seconds\n", (long int) time_elapsed.tv_sec, (long int) time_elapsed.tv_usec);
}

/**
 * @brief Helper function to print out an update after each production or consumption.
 * 
 * @param type the type of pthread
 * @param number the number either generated or consumed
 * @param tid the TID of the calling pthread
 * @param pthread_arg the thread's argument
 */
void print_update(int type, int number, int tid, struct pthread_arg *pthread_arg) {
    int buffer_items, index;

    // If the caller is of type functional producer,
    if(type == FNCTNL_PROD) {
        // get the index of the action array,
        index = update_arrays(type, tid);
        if(pthread_arg->prog_arg->debug) printf("(PRODUCER %3d writes %3d/%d %4d): ", index + 1, functional_produced[index], num_items_per_producer, number);

        // print the current array,
        buffer_items = print_array(pthread_arg->buffer, pthread_arg->prog_arg->length, pthread_arg->prog_arg->debug);

        // and determine if the buffer is full.
        if(buffer_items == buffer_size) {
            if(pthread_arg->prog_arg->debug)  printf("*BUFFER NOW FULL* ");
            num_full++;
        }
    
    // else if the caller is of type faulty producer,
    } else if(type == FAULTY_PROD) {
        // get the index of the action array,
        index = update_arrays(type, tid);
        if(pthread_arg->prog_arg->debug) printf("(PR*D*C*R %3d writes %3d/%d %4d): ", index + 1, faulty_produced[index], num_items_per_producer, number);

        // print the current array,
        buffer_items = print_array(pthread_arg->buffer, pthread_arg->prog_arg->length, pthread_arg->prog_arg->debug);

        // and determine if the buffer is full.
        if(buffer_items == buffer_size) {
            if(pthread_arg->prog_arg->debug) printf("*BUFFER NOW FULL* ");
            num_full++;
        }

    // else if the caller is of type consumer.
    } else if(type == CONSUMER) {
        // get the index of the action array,
        index = update_arrays(type, tid);
        if(pthread_arg->prog_arg->debug) printf("(CONSUMER %3d reads %4d %9d): ", index + 1, consumed_thread[index], number);

        // print the current array,
        buffer_items = print_array(pthread_arg->buffer, pthread_arg->prog_arg->length, pthread_arg->prog_arg->debug);

        // and determine if the number is not prime,
        if(!(is_prime(number))) {
            if(pthread_arg->prog_arg->debug) printf("*NOT PRIME* ");
            num_nonprimes++;
        }
        
        // and if the buffer is empty.
        if(buffer_items == 0) {
            if(pthread_arg->prog_arg->debug) printf("*BUFFER NOW EMPTY* ");
            num_empty++;
        }
    }

    if(pthread_arg->prog_arg->debug) printf("\n");
}

/**
 * @brief Helper function to update arrays, tracking the tid-to-index and tid-to-actions.
 * 
 * @param type the type of pthread
 * @param tid the pthread's tid
 * @return int the corresponding index of the pthread
 */
int update_arrays(int type, int tid) {
    int i, bound;
    int volatile *tid_array, *action_array;

    // Based on the type of pthread calling, set the variables appropriately.
    if(type == FNCTNL_PROD) { bound = num_producers; tid_array = tid_functional; action_array = functional_produced; }
    else if(type == FAULTY_PROD) { bound = num_faulty; tid_array = tid_faulty; action_array = faulty_produced; }
    else if(type == CONSUMER) { bound = num_consumer; tid_array = tid_consumer; action_array = consumed_thread; }

    // Iterate up to the bounds, searching through the tid_arr to find either the first zero entry
    // or an entry that matches the calling pthread's TID.
    for(i = 0; i < bound; i++) {
        // If a match is found,
        if(tid_array[i] == tid) {
            // increment the action by one for that index.
            action_array[i]++;
            break;
        // Otherwise, if a zero is found first,
        } else if(tid_array[i] == 0) {
            // then load the TID into the tid_arr at that index,
            // and increment the action array, again, at that index.
            tid_array[i] = tid;
            action_array[i]++;
            break;
        }
    }
    return i;
}

/**
 * @brief Helper function to print the nnumber of items in the buffer and the buffer itself.
 * 
 * @param array the array
 * @param size the size fo the array
 * @param debug flag to indicate whether to print the array
 * @return int the count of non-zero entries
 */
int print_array(int *array, int size, int debug) {
    int count = 0;
    for(int i = 0; i < size; i++) {
        if(array[i] != 0) {
            count++;
        }
    }
    
    if(debug) {
        // Print the number of non-zero items in the array.
        printf("(%d): ", count);

        printf("[ ");
        for(int i = 0; i < size; i++) {
            if(array[i] != 0) {
                printf("%6d%2s", array[i], "");
            }
        }
        printf("] ");
    }

    return count;
}