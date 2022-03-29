/***************************************************************************
/ PROGRAM:  multiThread.c
/ AUTHOR:   G'Avonti Patton
/ SYNOPSIS: A simple multithreaded program that creates five threads. creates a global array of 10,000 integers and populates each element with a pseudorandom number
/           between 1 and 100. It then finds the sum of all the ints in the array by splitting the
/           workload up between the five threads.
/***************************************************************************/
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5
#define NUM_COUNT 10000

struct limits
{
  int begin;
  int end;
  int sum;
};

// The array containing NUM_COUNT number of integers to add together
int numbers[NUM_COUNT];

// Function that all threads will execute
// The thread function that adds the numbers in the array starting with the
// begin field of the args struct through the index represented by the end field.
void *thread_add(void *threadid)
{
  int t;
  long tid;
  tid = (long)threadid;
  int first = ((struct limits *)tid)->begin;
  int last = ((struct limits *)tid)->end;

  for (int i = first; i <= last; i++)
    ((struct limits *)tid)->sum += numbers[i];

  //  Each thread will sleep for one second then continue
  sleep(1);
  return NULL;
}

int main(int argc, char **argv)
{
  // Dynamically allocate memory for and initialize fields of a struct that
  // contains the array indices from which to start and end when adding ints.
  struct limits *one = (struct limits *)malloc(sizeof(struct limits));
  one->begin = 0;
  one->end = NUM_COUNT - 1;
  void *result;
  int correctTotal = 0;
  int rc;
  int i;
  pthread_t threads[NUM_THREADS];
  // Seed the pseudorandom number generator with the current system time
  srand(time(0));
  // Populate the numbers array with pseudorandom ints between 1 and 100
  // and tally the correct total so we can check for program correctness
  for (int i = 0; i < NUM_COUNT; i++)
  {
    numbers[i] = (rand() % 100) + 1;
    correctTotal += numbers[i];
  }
  printf("The correct sum is: %d\n", correctTotal);

  // Create five threads to sum all numbers. divide the array
  // indices so each thread sums an equal seperate amount.
  for (i = 0; i < NUM_THREADS; i++)
  {
    printf("Creating thread: %d\n", i + 1);
    switch (i)
    {
    case 0:
      one->begin = 0;
      one->end = 2000 - 1;
      break;
    case 1:
      one->begin = 2000;
      one->end = 4000 - 1;
      break;
    case 2:
      one->begin = 4000;
      one->end = 6000 - 1;
      break;
    case 3:
      one->begin = 6000;
      one->end = 8000 - 1;
      break;
    case 4:
      one->begin = 8000;
      one->end = 10000 - 1;
      break;
    }
    rc = pthread_create(&threads[i], NULL, thread_add, (void *)one);
    pthread_join(threads[i], NULL);
    if (rc)
    {
      printf("Error:unable to create thread, %s\n", strerror(errno));
      exit(-1);
    }
  }
  // print the sum of the 5 different thread totals to verify
  // it's the same as the correct sum.
  printf("Threaded sum is: %d\n", one->sum);
  pthread_exit(NULL);
  return EXIT_SUCCESS;
}
