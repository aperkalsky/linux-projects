#include <stdio.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS	100
#define MAX_RADIAN	3.14
#define RADIAN_STEP	0.0000001

void* threadFunc(void* arg)
{
	double acc = 0;
	double* pArg = (double*)arg;
	
	for(double rad = 0; rad < MAX_RADIAN; rad += RADIAN_STEP)
	{
		acc += cos(rad);
	}
	
	*pArg = acc;
	
	pthread_exit(NULL);
}

int main()
{
    int numThreads;
	pthread_t threadId[MAX_THREADS];
	double calcResult[MAX_THREADS];
    struct timespec start, end;
    double elapsedTime;
	
	
	for(int i = 0; i < MAX_THREADS; i++)
	{
		threadId[i] = 0;
		calcResult[i] = 0;
	}

    printf("Enter a number of threads in range 1..%d: ", MAX_THREADS);

    if(scanf("%d", &numThreads) != 1)
	{
		printf("Incorrect input. Unable to continue\n");
		return 1;
	}
	
	if(numThreads < 1 || numThreads > MAX_THREADS)
	{
		printf("The value %d is out of range\n", numThreads);
		return 1;
	}

    printf("Number of threads chosen: %d\n", numThreads);
	
    // CLOCK_MONOTONIC represents steady time since an arbitrary point
    clock_gettime(CLOCK_MONOTONIC, &start);
	
	for(int i = 0; i < numThreads; i++)
	{
		int status = pthread_create(&threadId[i], NULL, threadFunc, &calcResult[i]);
		
		if(status != 0)
		{
			printf("Failed running thread %d\n", i);
			return 1;
		}
	}
	
	printf("Threads launched\n");

	for(int i = 0; i < numThreads; i++)
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
	

	for(int i = 0; i < numThreads; i++)
	{
		printf("Thread %d calc result = %.6f\n", i, calcResult[i]);
	}

    return 0;
}
