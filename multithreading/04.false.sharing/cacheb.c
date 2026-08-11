#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>

#define NUM_ITERATIONS	1000000000
#define NUM_THREADS		2

//#define WITH_PADDING

#ifndef WITH_PADDING
typedef struct
{
    uint64_t a;
    uint64_t b;
}Counters;
#else
typedef struct
{
    uint64_t a;
    char padding[64];
    uint64_t b;
}Counters;	
#endif

Counters cnt;

void* threadAFunc(void* arg)
{
	Counters* pArg = (Counters*)arg;
	
	for(uint64_t i = 0; i < NUM_ITERATIONS; i++)
	{
		pArg->a = i;
	}
	
	pthread_exit(NULL);
}

void* threadBFunc(void* arg)
{
	Counters* pArg = (Counters*)arg;
	
	for(uint64_t i = 0; i < NUM_ITERATIONS; i++)
	{
		pArg->b = i;
	}
	
	pthread_exit(NULL);
}

int main()
{
	pthread_t threadId[2];
    struct timespec start, end;
    double elapsedTime;
	int status;
	
	cnt.a = cnt.b = 0;
	
    // CLOCK_MONOTONIC represents steady time since an arbitrary point
    clock_gettime(CLOCK_MONOTONIC, &start);
	
	status = pthread_create(&threadId[1], NULL, threadAFunc, &cnt);
	
	if(status != 0)
	{
		printf("Failed running thread A\n");
		return 1;
	}
	
	status = pthread_create(&threadId[2], NULL, threadBFunc, &cnt);
	
	if(status != 0)
	{
		printf("Failed running thread B\n");
		return 1;
	}
	
	for(int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(threadId[i], NULL);
	}
	
    clock_gettime(CLOCK_MONOTONIC, &end);

	printf("Threads finished\n");
	
    // calculate elapsed time in seconds
    // (Seconds difference) + (Nanoseconds difference converted to seconds)
    elapsedTime = (end.tv_sec - start.tv_sec) + 
                   (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Elapsed time: %.6f seconds\n", elapsedTime);
	
    return 0;
}
