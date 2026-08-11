#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>

#define QUEUE_SIZE     5
#define NUM_ITEMS      20

pthread_mutex_t lock;
atomic_int numItemsRemainingToProduce = NUM_ITEMS;
atomic_int numItemsRemainingToConsume = 0;

typedef struct
{
    int buffer[QUEUE_SIZE];

    int head;       // Next position to write
    int tail;       // Next position to read
    int count;      // Number of items currently in queue

    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;

} Queue;


static void Queue_Init(Queue *q)
{
    q->head = 0;
    q->tail = 0;
    q->count = 0;

    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}


static void Queue_Put(Queue *q, int value)
{
    pthread_mutex_lock(&lock);

    /*
     * Queue is full.
     * Wait until a consumer removes something.
     */
    while (q->count == QUEUE_SIZE)
    {
        pthread_cond_wait(&q->not_full, &lock);
    }

    /* Put item into queue */
    q->buffer[q->head] = value;
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count++;

    printf("Producer: put %2d   queue=%d\n",
           value, q->count);

    /*
     * We just added an item, so wake a waiting consumer.
     */
    pthread_cond_signal(&q->not_empty);

    pthread_mutex_unlock(&lock);
}


static int Queue_Get(Queue *q)
{
    int value;

    pthread_mutex_lock(&lock);

    /*
     * Queue is empty.
     * Wait until a producer adds something.
     */
    while (q->count == 0)
    {
        pthread_cond_wait(&q->not_empty, &lock);
    }

    /* Remove item from queue */
    value = q->buffer[q->tail];
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    q->count--;

    printf("Consumer: got %2d   queue=%d\n",
           value, q->count);

    /*
     * We just removed an item, so wake a waiting producer.
     */
    pthread_cond_signal(&q->not_full);

    pthread_mutex_unlock(&lock);

    return value;
}


static void *Producer(void *arg)
{
    Queue *q = (Queue *)arg;
	
	while(numItemsRemainingToProduce >= 0)
	{
        Queue_Put(q, numItemsRemainingToProduce);
		numItemsRemainingToProduce--;
        usleep(100000);
	}

	pthread_exit(NULL);
}


static void *Consumer(void *arg)
{
    Queue *q = (Queue *)arg;
	
	while(numItemsRemainingToConsume <= NUM_ITEMS)
	{
        int value = Queue_Get(q);
		numItemsRemainingToConsume++;
        usleep(300000);
        (void)value;	// tell compiler that we do not use this value
	}

	pthread_exit(NULL);
}


int main(void)
{
    Queue queue;

    pthread_t producer_thread;
    pthread_t consumer_thread;

 	pthread_mutex_init(&lock, NULL);
    Queue_Init(&queue);

    pthread_create(&producer_thread,
                   NULL,
                   Producer,
                   &queue);

    pthread_create(&consumer_thread,
                   NULL,
                   Consumer,
                   &queue);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

	pthread_mutex_destroy(&lock);

    return 0;
}