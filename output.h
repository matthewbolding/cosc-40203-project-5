/**
 * @file output.h
 * @author Matthew Bolding; Griffin McPherson
 * @brief Contains function prototypes for output.c and other files.
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "definitions.h"

void initialize_stats(int items, int length, int producers, int faulties, int consumers);
void display_stats();
void print_update(int type, int number, int tid, struct pthread_arg *pthread_arg);
int update_arrays(int type, int tid);
int print_array(int *array, int size, int debug);