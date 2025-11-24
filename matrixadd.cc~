/*
matrixadd.cc
Alex Barbee
Oct 8, 2025

Multithreaded program that adds matrices of N size using M threads.
*/ 


// incudes
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
//#include <ctype.h>
#include <math.h>
#include <sys/time.h>
//#include <unistd.h>


// globals
const int MAX_SIZE = 10000;
const int MAX_THREADS = 8;
const int RAND_SEED = 546373;

void* calculateMatrix(void * args);

static double matrixA[MAX_SIZE][MAX_SIZE] = {0};
static double matrixB[MAX_SIZE][MAX_SIZE] = {0};
static double matrixR[MAX_SIZE][MAX_SIZE] = {0};// init to 0

int tCount; // global thread count

struct arg_struct {
  int start;
  int work;
  int length;
  bool isMain;
}; // arg for calculateMatrix


// timing system
timeval startTime;
timeval stopTime;
long mtime, seconds, useconds;


// main
int main(int argc, char *argv[])
{
  
  //Only grab args if they exist
  if ( argc == 3)
  {
    int c1Arg1 = (int)(argv[1][0]);
    int c1Arg2 = (int)(argv[2][0]);
    
    // Validate argument count and type
    if (!(c1Arg1 >= '0' && c1Arg1 <= '9') || !(c1Arg2 >= '0' && c1Arg2 <= '9')) 
    {
      printf("matrixadd requires 2 arguments. Matrix Size (int) & Thread Count (int)\n");
      return 1;
    }
  } else 

  {
      printf("matrixadd requires 2 arguments. Matrix Size (int) & Thread Count (int)\n");
      return 1;
  }
  
  // Grab and store the arguments
  const int mSize = atoi(argv[1]);
  tCount = atoi(argv[2]);
  
  // Validate argument values
  if ( mSize > MAX_SIZE )
  {
    
    printf("The matrix size must be %d or under.\n", MAX_SIZE);
    return 1;
    
  } else if ( tCount > MAX_THREADS )
  {
    
    printf("The thread count must be %d or under.\n", MAX_THREADS);
    return 1;
    
  }
  
  // Arguments passed
  printf("You selected a matrix size of %d and a thread count of %d\n", mSize, tCount);
  
  int newThreads = tCount-1;
  
  pthread_t threads[newThreads];
  
  printf("Filling matrixA and matrixB...\n\n");
   
  // Fill matrices with random numbers
  for (int i = 0; i < mSize; i++)
  {
    
    for (int j = 0; j < mSize; j++)
    {
      
      matrixA[i][j] = random() / double(RAND_MAX);
      matrixB[i][j] = random() / double(RAND_MAX);

    }
  }
  
  // init state variables to be passed through the arg struct
  double workSz;
  double workFraction = ((double)mSize/(double)tCount);
  double leftover = modf(workFraction,&workSz);
  
  printf("Work fraction: %f\n",workFraction);
  printf("Workload per thread: %f\n",workSz);
  printf("Extra workload: %f\n", leftover*tCount);
  
  printf("\nBeginning execution...\n\n");
  
  // make an array of arg_struct's so we can have separate args for each thread
  struct arg_struct myArgs[MAX_THREADS];
  
  // start the execution timer
  gettimeofday(&startTime, 0);
  
  for(int i=0; i<newThreads; i++)
  {
    
    // set arguments for the thread
    myArgs[i].isMain  = false;
    myArgs[i].length  = mSize;
    myArgs[i].work    = (int)workSz;
    myArgs[i].start   = (i*myArgs[i].work);
    
    //start the thread
    pthread_create(&threads[i], NULL, calculateMatrix, (void*)&myArgs[i]);
  }
  

  int extra = (int)(leftover*tCount);
 
  // running the final calculateMatrix on the main thread
  myArgs[newThreads].length = mSize;
  myArgs[newThreads].work   = (int)workSz + extra;
  myArgs[newThreads].isMain = true;
  myArgs[newThreads].start  = (newThreads*(int)workSz);
  
  
  printf("\nFinal (main) Threaded Workload: %d\n",myArgs[newThreads].work);
  printf("Final (main) startPoint: %d\n",myArgs[newThreads].start);
  
  // main thread executing the algorithm
  calculateMatrix((void*)&myArgs[newThreads]);
  
  
  // wait for child threads to finish
  for (int i = 0; i < newThreads; i++)
  {
    // pthread_detach(threads[i]);
    pthread_join(threads[i], NULL);
  }
  
  // stop execution timer
  gettimeofday(&stopTime, 0);
  
  
  // calculate seconds and milliseconds of execution time
  seconds   = stopTime.tv_sec - startTime.tv_sec;
  useconds  = stopTime.tv_usec - startTime.tv_usec;
  
  
  
  float msecs  = ((seconds) * 1000 + useconds/1000.0) + 0.5;
  float secs    = (seconds * 1000 + useconds/1000.0) / 1000.0;
  
  printf("\n\nTime Results:\n");
  printf("Milliseconds: %lf\n", msecs);
  printf("Seconds: %f\n", secs);
  
  
  return NULL;
  // Have to return null
  //return NULL;
}


void* calculateMatrix(void * arguments) // calculates the matrix for a given thread
{
  struct arg_struct *myArgs = (struct arg_struct*)arguments;
  
  // grabbing struct values
  bool main     = myArgs  ->isMain;
  int workAmt   = myArgs  ->work;
  int len       = myArgs  ->length;
  int startNDX  = myArgs  ->start;
  
  int stopNDX   = startNDX+workAmt;
  
  printf("Running CalculateMatrix with a start index of %d and stop index of %d\n", startNDX,stopNDX);
  
  
  //Perform math within bounds of matrix & workload
  for (int i = startNDX; i < stopNDX; i++)
  {
    
    for (int j = 0; j < len; j++)
    {
      
      matrixR[i][j] = matrixA[i][j] + matrixB[i][j];
      
    }
  }
  
  if (!main)
    pthread_exit(NULL);
  
  return 0;
  
}