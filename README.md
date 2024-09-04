# Producer-Conusmer Problem with Faulty Threads
## COSC 40203 Operating Systems (Spring 2022)
Due 15:30, April 14<sup>th</sup>, 2022
Matthew Bolding and Griffin McPherson

## Objective

The purpose of this programming project is to explore process synchronization. This will be accomplished by writing a simulation to the Producer / Consumer problem described below. Your simulation will be implemented using pthreads with semaphores and mutex locks. Tutorials on the pthread functions and their usage can be found in our text, in our notes, or online. This project is very similar to the project described in our textbook but has additional requirements.

## The Producer/Consumer Problem

The producer consumer problem is a process synchronization problem that demonstrates cooperating threads. Producer threads will place an item into a common shared buffer. Consumer threads will remove an item from the buffer. In this project the buffer is "bounded"; i.e. it has a finite length to it. This could be implemented as a circular array or you could use one of the STL linear data structures (list, vector, or queue).

A producer thread can place an item into the buffer only if the buffer has a free memory location to store the item. A producer thread cannot add an item to a full buffer. A consumer thread can remove an item from the buffer only if the buffer is not empty. A consumer thread must wait to consume items if the buffer is empty.

The "items" stored in this buffer will be positive integers. Your producer process will "randomly find" a prime number (described below). The consumer process will have to "verify" that the number consumed from the buffer is a prime number.

## Project Specifications

Write a program that simulates the producer/consumer problem using C/C++ and pthreads. The producer thread will alternate between finding a prime number and inserting it into the buffer. Prime numbers will be “randomly guessed”. That is, the producer will use a random number generator to generate a random number [2..999999]. Then the producer will test if the random number is prime. If the number is a prime, the producer will (try to) insert it into the buffer.

Producer threads always insert prime numbers into the buffer. Faulty producers insert any random even number [4..999999] into the buffer.

Consumer threads will remove numbers from the buffer and test if the number is prime. If the number is not prime, a warning message will be displayed.

The main function will initialize the buffer and create the separate producer, faulty producer, and consumer threads. Once it has created the threads, the `main()` function will then wait for the producers to finish producing and the consumers to finish consuming. After joining all threads, the main thread will then display the simulation statistics. The `main()` function will be passed five required parameters on the command line and a sixth optional parameter:

1. The number of items to produce per producer thread (required) (`-n`)
2. The length of the buffer (required) (`-l`)
3. The number of producer threads (required) (`-p`)
4. The number of faulty producer threads (required) (`-f`)
5. The number of consumer threads (required) (`-c`)
6. Optional debug flag (optional) (`-d`)

Perform any and all necessary validation on the command line arguments. There are no default values for the command line arguments. There are C/C++ functions to help parse command line arguments received in any order. See the next section for an example of how the program will be run with command line arguments.

## How to Compile and Run

To compile the program, execute `make`, which compiles and links an executable called `pc`. The make file has an additional target, `clean`, which results in all object files and the executable being deleted.

## Project Deliverables

1. Follow the project submission guidelines.
2. Follow the project documentation standards.
3. Your project must have a `Makefile` and a `README.md` file.
4. Update your project using GitHub Classroom.