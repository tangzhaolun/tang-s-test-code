/* This file test the usage of pthread mutex and queue pointer(linked list).
 * But potential problem is if threads execute faster than enqueue function
 * (use sleep function with enqueue to simulate this),
 * all threads exit before enqueue, then queue is not empty.
 * To avoid this situation, check thread_job_queue_sem.c for solution.*/
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>

struct job {
    int id;
    struct job *next;
};

void process_job (struct job *one_job)
{
    printf("Thread_%d is process job %d\n", (int)pthread_self (), one_job->id);
}

pthread_mutex_t job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_func (void *data)
{
    while (1) {
        struct job *next_job;
        
        /* IMPORTANT: in order to modify a pointer passed into function,
         * make the parameter as pointer to this pointer.*/
        struct job **job_queue = (struct job **)data;

        pthread_mutex_lock (&job_queue_mutex);
        /* Use mutex to avoid multiple threads executing same job.
         * this could happen if there is no mutex, 
         * and the current thread may be interrupted at'next_job = job_queue'
         * and thus another thread will take the same job to do*/
        if (*job_queue == NULL)
            next_job = NULL;
        else {
            /* Remove a job from the head of the queue.*/
            next_job = *job_queue;
            *job_queue = (*job_queue)->next;
        }
        pthread_mutex_unlock (&job_queue_mutex);

        /* If the queue is empty, thread will exit.
         * This may lead to misfunction if the enqueue function is slow,
         * and all threads finish jobs quickly, then exit, 
         * and thus new job is created by enqueue but no thread is availalbe.
         * Check thread_job_queue2.c for semaphore solution.*/
        if (next_job == NULL) 
            break;

        process_job (next_job);
        free (next_job);
    }
    return NULL;
}

/* IMPORTANT: in order to modify the queue pointer,
 * pass it as pointer to pointer.*/
void enqueue_job (struct job **job_queue, struct job * new_job)
{
    pthread_mutex_lock (&job_queue_mutex);
    /* Add new job to the head of the queue.*/
    new_job->next = *job_queue;
    *job_queue = new_job;
    pthread_mutex_unlock (&job_queue_mutex);
}

int main()
{
    const int SIZE = 3;
    pthread_t threads[SIZE];
    int i;

    struct job *job_queue = NULL;
    
#if 0
    /* Create new jobs for a job queue. */
    for (i = 0; i < 50; i++) {
        struct job *new_job = malloc (sizeof (struct job));
        new_job->id = i;
        enqueue_job (&job_queue, new_job);
    }
#endif

    for (i = 0; i < SIZE; i++) {
        pthread_create (&(threads[i]), NULL, thread_func, &job_queue);
    }

    /* Create new jobs for a job queue. */
    for (i = 0; i < 10; i++) {
        struct job *new_job = malloc (sizeof (struct job));
        new_job->id = i;
        enqueue_job (&job_queue, new_job);
        sleep(1);
    }

    for (i = 0; i < SIZE; i++) {
        pthread_join (threads[i], NULL);
    }

    return 0;
}

