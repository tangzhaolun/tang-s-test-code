/*In thread_job_queue_sem.c there is a problem that after job_queue is empty,
 * all threads are blocked by sem_wait(), then pthread_join fails to return,
 * and main never exits as expected.
 * This file uses condition variable (flag) to solve above prolbem.*/
#include <malloc.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>

struct job {
    int id;
    struct job *next;
};

void process_job (struct job *one_job)
{
    printf("Thread_%d is process job %d\n", (int)pthread_self (), one_job->id);
}

pthread_mutex_t job_queue_mutex = PTHREAD_MUTEX_INITIALIZER;

int thread_flag;
pthread_cond_t thread_flag_cv;
pthread_mutex_t thread_flag_mutex;

void init_flag ()
{
    pthread_mutex_init (&thread_flag_mutex, NULL);
    pthread_cond_init (&thread_flag_cv, NULL);
    thread_flag = 0;
}

void *thread_func (void *data)
{
    while (1) {
        struct job *next_job;
        struct job **job_queue = (struct job **)data;

        pthread_mutex_lock (&thread_flag_mutex);
        while (thread_flag == 0) {
            pthread_cond_wait (&thread_flag_cv, &thread_flag_mutex);
        }
        pthread_mutex_unlock (&thread_flag_mutex);

        pthread_mutex_lock (&job_queue_mutex);
        if ( *job_queue == NULL){ 
            pthread_mutex_unlock (&job_queue_mutex);
            break;
        }
        /* Remove a job from the head of the queue.*/
        next_job = *job_queue;
        *job_queue = (*job_queue)->next;
        pthread_mutex_unlock (&job_queue_mutex);

        process_job (next_job);
        free (next_job);
    }
    return NULL;
}

void set_thread_flag (int flag) 
{
    pthread_mutex_lock (&thread_flag_mutex);
    /* Set the flag value and signal in case thread function is blocked,
     * waiting for the flag to become set.*/
    thread_flag = flag;
    /*pthread_cond_signal (&thread_flag_cv);*/
    pthread_cond_broadcast (&thread_flag_cv);
    pthread_mutex_unlock (&thread_flag_mutex);
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

    init_flag();

    for (i = 0; i < SIZE; i++) {
        pthread_create (&(threads[i]), NULL, thread_func, &job_queue);
    }

    /* Create new jobs for a job queue. */
    for (i = 0; i < 10; i++) {
        struct job *new_job = malloc (sizeof (struct job));
        new_job->id = i;
        enqueue_job (&job_queue, new_job);
#if 0
        /* Use sleep to make enqueue slower than dequeue(thread processing).*/
        sleep(1);
#endif
    }  

    set_thread_flag(1);

    for (i = 0; i < SIZE; i++) {
        pthread_join (threads[i], NULL);
    }

    return 0;
}

