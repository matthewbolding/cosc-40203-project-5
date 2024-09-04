/**
 * @file input.c
 * @author Matthew Bolding; Griffin McPherson
 * @brief Implements functions relating to the program's input.
 * @version 0.1
 * @date 2022-04-13
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdlib.h>
#include <argp.h>
#include <stdbool.h>

#include "definitions.h"
#include "input.h"

// Global Variables
bool verbose = false;

const char *argp_program_version = "producer-consumer 1.0";
const char *argp_program_bug_address = "<matthew.bolding@tcu.edu; g.mcpherson@tcu.edu>";

static char doc[] = "Producer Consumer Problem for COSC 40203 Operating Systems";

static char args_doc[] = "";

static struct argp_option options[] = {
    {0, 0, 0, 0, "The below five options are mandatory." },
    {"items",    'n', "NUM", 0,                   "The number of items to produce per producer thread"}, 
    {"length",   'l', "NUM", 0,                   "The length of the buffer"}, 
    {"producer", 'p', "NUM", 0,                   "The number of producer threads"}, 
    {"faulty",   'f', "NUM", 0,                   "The number of faulty producer threads"}, 
    {"consumer", 'c', "NUM", 0,                   "The number of consumer threads"}, 
    {0, 0, 0, 0, "Debug is optional." },
    {"debug",    'd', 0, OPTION_ARG_OPTIONAL, "Optional debug flag"}, 
    {0}
};

/**
 * @brief This function interprets the input.
 * 
 */
static error_t parse_opt (int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'n':
            arguments->items = atoi(arg);
            break;
        case 'l':
            arguments->length = atoi(arg);
            break;
        case 'p':
            arguments->producer = atoi(arg);
            break;
        case 'f':
            arguments->faulty = atoi(arg);
            break;
        case 'c':
            arguments->consumer = atoi(arg);
            break;
        case 'd':
            arguments->debug = true;
            break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

/**
 * @brief Helper function to obtain the command arguments provided by the user.
 * 
 * @param argc
 * @param argv
 * @return arguments
 */
struct arguments get_arguments(int argc, char **argv) {
    if(!(argc == 2 || argc == 11 || argc == 12)) {
        fprintf(stderr, "Usage: pc [OPTION...]\n");
        printf("\nSee `./pc --help' for more details.\n");
        exit(1);
    }

    struct arguments arguments;
    arguments.debug = false;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    if(verbose) {
        printf("-n: %d\n", arguments.items);
        printf("-l: %d\n", arguments.length);
        printf("-p: %d\n", arguments.producer);
        printf("-f: %d\n", arguments.faulty);
        printf("-c: %d\n", arguments.consumer);
        printf("-d: %s\n\n", arguments.debug ? "true" : "false");
    }

    return arguments;
}
