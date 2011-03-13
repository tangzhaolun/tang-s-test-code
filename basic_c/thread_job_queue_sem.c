/* This file provide semaphore solution to solve problem in thread_job_queue.c
 * But there is new problem: after all jobs are processed, 
 * all threads are blocked by sem_wait function and can't be jointed,
 * so the main can't exit normally*/
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

struct job {
    int id;
    struct job *next;
};

void process_job (struct job *one_job)
{
    printf("Thread_%d is process job %d\n", (int)pthread_self (), one_job->id);
}

pthread_mutex_t job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t job_count;

void *thread_func (void *data)
{
    while (1) {
        sem_wait (&job_count);

        struct job *next_job;
        
        /* IMPORTANT: in order to modify a pointer passed into function,
         * make the parameter as pointer to this pointer.*/
        struct job **job_queue = (struct job **)data;

        pthread_mutex_lock (&job_queue_mutex);
        /* Remove a job from the head of the queue.*/
        next_job = *job_queue;
        *job_queue = (*job_queue)->next;
        
        pthread_mutex_unlock (&job_queue_mutex);

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

    sem_post (&job_count);
}

int main()
{
    const int SIZE = 3;
    pthread_t threads[SIZE];
    int i;

    /* Initialize job queue. */
    struct job *job_queue = NULL;
    /* Initialize semaphore for the job queue. */
    sem_init (&job_count, 0, 0);

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
        /* Use sleep to make enqueue slower than dequeue(thread processing).*/
        sleep(1);
    }

    for (i = 0; i < SIZE; i++) {
        pthread_join (threads[i], NULL);
    }

    sem_destroy (&job_count);

    return 0;
}

