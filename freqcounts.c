/*
  freqcounts.cc
  Alex Barbee
  Oct 27, 2025
  A multithreaded character frequency counter
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
 
const int C_BIT = 8;

const int BLOCK_SIZE = 64 * sizeof(char); //bytes

const int BUFFER_SIZE = 8;


int in = 0, out = 0;
//int count = 0;

const int MAX_THREADS = 4;
const int MIN_THREADS = 2;

int HISTOGRAM[256] = {0};

char SHARE_BUF[BUFFER_SIZE][BLOCK_SIZE];


int THREAD_COUNT;

// Thread synchronization
pthread_mutex_t mutex;
sem_t full, empty;


int workload[MAX_THREADS] = {0};


void parentProcess(FILE *fArg);
void * consumerProcess(void * input);

int main(int argc, char * argv[])
{
  FILE* inF = NULL; //input file
  
  
  if (argc == 3) //argument handler
  {
    inF = fopen(argv[1], "r");
    THREAD_COUNT = atoi(argv[2]);
    if (inF == NULL)
    {
      printf("Error, the file %s does not exist\n", argv[1]);
      return 1;
    }
    else if(THREAD_COUNT > MAX_THREADS || THREAD_COUNT < MIN_THREADS)
    { 
      printf("Error, your chosen thread count must be from %d to %d\n", MIN_THREADS, MAX_THREADS);
      return 1;
    }
  } 
  else 
  {
    printf("Error, freqcounts needs 2 arguments [text-file] and [num-threads]\n");
    return 1;
  }
  
  //if we get here, our arguments have been validated
  printf("Reading from %s with a threadcount of %d\n", argv[1], THREAD_COUNT);
  
  
  // Initialize mutex and semaphores
  pthread_mutex_init(&mutex, NULL);
  sem_init(&full, 0, 0);
  sem_init(&empty, 0, BUFFER_SIZE);  
  
  //begin initializing consumer threads
  pthread_t threads[THREAD_COUNT];
  
  
  printf("Spawning child processses.");
  
  for(int id=0; id<THREAD_COUNT; id++)
  {
    
    //start the thread
    pthread_create(&threads[id], NULL, consumerProcess, (void *)(long)id);
  }
  
  printf("Filling buffer for the first time...\n");
  //run main thread
  parentProcess(inF);
  
  
  //wait for consumers to finish
  for (int i = 0; i < THREAD_COUNT; i++)
    pthread_join(threads[i], NULL);

  
  //printing output
  
  printf("\nFREQUENCY COUNTS\n(%d consumers)\n", THREAD_COUNT);
  printf("Work Load:\n");
  for (int i = 0; i < THREAD_COUNT; i++)
      printf("C%d - %d\n", i + 1, workload[i]);

  printf("\nFrequency Counts:\n---------------------------------------------------------------\n");
  
  for (int i = 0; i < 256; i++)
  {
    if (HISTOGRAM[i] > 0)  //handle special chars
    {
      if (i == '\n')
        printf("\\n -- %d ", HISTOGRAM[i]);
      else if (i == '\t')
        printf("\\t -- %d ", HISTOGRAM[i]);
      else if (i == ' ')
        printf("' ' -- %d ", HISTOGRAM[i]);
      else
        printf("%c -- %d ", i, HISTOGRAM[i]);
    }
  }
  printf("\n---------------------------------------------------------------\n");

  // Cleanup
  pthread_mutex_destroy(&mutex);
  sem_destroy(&full);
  sem_destroy(&empty);
  fclose(inF);

  return 0;
  
  

}
  

void parentProcess(FILE *fArg)
{
  char tempBuffer[BLOCK_SIZE] = {0}; // temporary buffer to load new reads into
  
  bool isEOF = false;
  
  while (!isEOF) // loop until we reach the eof
  {
    fread(tempBuffer, 1, BLOCK_SIZE, fArg);
    //fgets(&tempBuffer, BLOCK_SIZE, fArg);
    
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    memcpy(SHARE_BUF[in], tempBuffer, BLOCK_SIZE);

    in = (in + 1) % BUFFER_SIZE;
    
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    
    if (feof(fArg))
      isEOF = true;
  }
  
  for (int i = 0; i < THREAD_COUNT; i++)
  {
    memset(tempBuffer, -1, BLOCK_SIZE); //set temp buffer to -1
    
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);  //lock
    
    memcpy(SHARE_BUF[in], tempBuffer, BLOCK_SIZE); //set shared buffer
    in = (in + 1) % BUFFER_SIZE;
    
    
    pthread_mutex_unlock(&mutex);  //unlock
    sem_post(&full);
  }
}


void * consumerProcess(void * input)
{
  
  int localHistogram[256] = {0};
  
  int id = (long)input; //process id
  
  char tempBuffer[BLOCK_SIZE] = {0}; //temporary buffer to save current block
  
  bool isDone = false;
  
  while(!isDone)
  {
    
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    
    
    memcpy(tempBuffer, SHARE_BUF[out], BLOCK_SIZE);  //read from the block
    
    out = (out + 1) % BUFFER_SIZE;
    
    
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    

    
    if((char)tempBuffer[0] == -1)
    {
      isDone = true;
    } else
    {
      for(int i = 0; i < BLOCK_SIZE; i++)
      {
        char current_ascii = tempBuffer[i]; // convert char to it's ascii character
        
        localHistogram[current_ascii]++;// increase corresponding histogram value by one.
        
        workload[id]++; //update workload
      }
    }
  
  }
  
  
  pthread_mutex_lock(&mutex);  //lock the histogram
  
  
  for (int i = 0; i < 256; i++)
    HISTOGRAM[i] += localHistogram[i]; //update global histogram
  
  
  pthread_mutex_unlock(&mutex);
  //return NULL
  pthread_exit(NULL);
}
