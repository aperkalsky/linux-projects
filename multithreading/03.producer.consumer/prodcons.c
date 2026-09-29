#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <stdbool.h>

#define QUEUE_SIZE      5
#define NUM_ITEMS       20
#define MAX_THREADS     100


atomic_int numItemsRemainingToProduce = NUM_ITEMS;


typedef struct
{
    int buffer[QUEUE_SIZE];

    int head;       // Next position to write
    int tail;       // Next position to read
    int count;      // Number of items currently in queue

    bool finished;  // All items have been put into the queue

    pthread_mutex_t mutex;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;

} Queue;


static void Queue_Init(Queue *q)
{
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->finished = false;

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}


static void Queue_Print(Queue *q)
{
    printf("head: %d, tail: %d, count: %d\n", q->head, q->tail, q->count);
}


static void Queue_Put(Queue *q, int value)
{
    pthread_mutex_lock(&q->mutex);

    /*
     * Queue is full.
     * Wait until a consumer removes something.
     */
    while (q->count == QUEUE_SIZE)
    {
		// Unlocks the mutex and starts waiting on the condition variable in one single, atomic step.
		// This stops race conditions between checking a state and waiting.
		// When another thread signals the condition, the waiting thread wakes up and automatically
		// locks (re-acquires) the mutex before returning
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    /* Put item into queue */
    q->buffer[q->head] = value;
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count++;

    printf("Producer: put %2d   queue=%d\n", value, q->count);

    /*
     * We just added an item, so wake a waiting consumer.
     */
    pthread_cond_signal(&q->not_empty);

    pthread_mutex_unlock(&q->mutex);
}


static bool Queue_Get(Queue *q, int *pValue)
{
    pthread_mutex_lock(&q->mutex);

    /*
     * Queue is empty.
     *
     * If production has not finished yet, wait for a producer.
     */
    while (q->count == 0 && !q->finished)
    {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    /*
     * Queue is empty and no more items will ever arrive.
     */
    if (q->count == 0 && q->finished)
    {
        pthread_mutex_unlock(&q->mutex);
        return false;
    }

    /* Remove item from queue */
    *pValue = q->buffer[q->tail];
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count--;

    printf("Consumer: got %2d   queue=%d\n",
           *pValue, q->count);

    /*
     * We just removed an item, so wake a waiting producer.
     */
    pthread_cond_signal(&q->not_full);

    pthread_mutex_unlock(&q->mutex);

    return true;
}


static void *Producer(void *arg)
{
    Queue *q = (Queue *)arg;

    while (1)
    {
        /*
         * Atomically reserve the next item number.
         */
        int item = atomic_fetch_sub(&numItemsRemainingToProduce, 1);

        if (item < 0)
        {
            break;
        }

        Queue_Put(q, item);

        /*
         * Item 0 was the last item to be produced.
         * No producer will put anything else into the queue.
         */
        if (item == 0)
        {
            pthread_mutex_lock(&q->mutex);

            q->finished = true;

            /*
             * There may be several consumers sleeping on
             * not_empty. Wake all of them so they can
             * discover that production has finished.
             */
            pthread_cond_broadcast(&q->not_empty);

            pthread_mutex_unlock(&q->mutex);
        }

        usleep(100000);
    }

    return NULL;
}


static void *Consumer(void *arg)
{
    Queue *q = (Queue *)arg;

    while (1)
    {
        int value;

        if (!Queue_Get(q, &value))
        {
            /*
             * No item available and production is finished.
             */
            break;
        }

        /*
         * Simulate processing time.
         */
        usleep(300000);

        (void)value;
    }

    return NULL;
}


int requestNumThreads(char *category)
{
    int numThreads;

    printf("Enter a number of %s threads in range 1..%d: ", category, MAX_THREADS);

    if (scanf("%d", &numThreads) != 1)
    {
        printf("Incorrect input. Unable to continue\n");
        return 0;
    }

    if (numThreads < 1 || numThreads > MAX_THREADS)
    {
        printf("The value %d is out of range\n", numThreads);
        return 0;
    }

    printf("Number of %s threads chosen: %d\n",
           category, numThreads);

    return numThreads;
}


int main(void)
{
    Queue queue;

    int numProducers;
    int numConsumers;

    pthread_t producerId[MAX_THREADS];
    pthread_t consumerId[MAX_THREADS];

    Queue_Init(&queue);

    numProducers = requestNumThreads("producer");

    if (!numProducers)
    {
        return 1;
    }

    numConsumers = requestNumThreads("consumer");

    if (!numConsumers)
    {
        return 1;
    }

    /*
     * Start consumers first.
     * They will simply wait on the condition variable
     * until producers put something into the queue.
     */
    for (int i = 0; i < numConsumers; i++)
    {
        int status = pthread_create(&consumerId[i], NULL, Consumer, &queue);

        if (status != 0)
        {
            printf("Failed running consumer thread %d\n", i);
            return 1;
        }
    }

    /*
     * Start producers.
     */
    for (int i = 0; i < numProducers; i++)
    {
        int status = pthread_create(&producerId[i], NULL, Producer, &queue);

        if (status != 0)
        {
            printf("Failed running producer thread %d\n", i);
            return 1;
        }
    }

    printf("Threads launched\n");

    /*
     * Wait for all producers.
     */
    for (int i = 0; i < numProducers; i++)
    {
        pthread_join(producerId[i], NULL);
    }

    /*
     * Wait for all consumers.
     */
    for (int i = 0; i < numConsumers; i++)
    {
        pthread_join(consumerId[i], NULL);
    }

    Queue_Print(&queue);

    printf("Items remaining to produce: %d\n", atomic_load(&numItemsRemainingToProduce));

    pthread_mutex_destroy(&queue.mutex);
    pthread_cond_destroy(&queue.not_empty);
    pthread_cond_destroy(&queue.not_full);

    return 0;
}