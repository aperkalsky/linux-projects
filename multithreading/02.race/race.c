#include <stdio.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS	100
#define NUM_ITERATIONS 1000000

// try one of two these options (mutex or C11 atomic):
// ===================================================
//#define USE_MUTEX
#define USE_ATOMIC
// ===================================================

#ifdef USE_ATOMIC
	#include <stdatomic.h>
#endif

#ifdef USE_MUTEX
pthread_mutex_t lock;
#endif

void* threadFunc(void* arg)
{
    long *cnt = (long *)arg;

    for (long i = 0; i < NUM_ITERATIONS; i++)
    {
#ifdef USE_MUTEX
		pthread_mutex_lock(&lock);
#endif

#ifdef USE_ATOMIC
		atomic_fetch_add(cnt, 1);
#else		
        (*cnt)++;
#endif

#ifdef USE_MUTEX
		pthread_mutex_unlock(&lock);
#endif
    }
	
	pthread_exit(NULL);
}

int main()
{
    int numThreads;
	pthread_t threadId[MAX_THREADS];
	long counter = 0;

    struct timespec start, end;
    double elapsedTime;
		
	for(int i = 0; i < MAX_THREADS; i++)
	{
		threadId[i] = 0;
	}
	
#ifdef USE_MUTEX
	pthread_mutex_init(&lock, NULL);
#endif

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
		int status = pthread_create(&threadId[i], NULL, threadFunc, &counter);
		
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

	printf("Counter value = %lu\n", counter);

#ifdef USE_MUTEX
	pthread_mutex_destroy(&lock);
#endif

    return 0;
}
