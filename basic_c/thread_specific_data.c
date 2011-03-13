#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_key_t log_key;

void write_to_log(const char* message)
{
	FILE* log = (FILE*) pthread_getspecific(log_key);
	fprintf(log, "%s\n", message);
}

void close_log(void* log)
{
	fclose((FILE*)log);
}

void* thread_func(void* unused)
{
	char filename[20];
	FILE* log;

	sprintf(filename, "thread_%d.log", (int)pthread_self());
	log = fopen(filename, "a+");
	if(log == NULL){
		fprintf(stderr, "File open error.\n");
		return NULL;
	}

	pthread_setspecific(log_key, log);
	
	write_to_log("Thread starting.\n");
	sleep(1);
	write_to_log("Thread finish.\n");

	return NULL;
}

int main()
{
	pthread_t threads[5];

	pthread_key_create(&log_key, close_log);

	int i;
	for(i = 0; i < 5; i++)
		pthread_create(&(threads[i]), NULL, thread_func, NULL);

	for(i = 0; i < 5; i++)
		pthread_join(threads[i], NULL);

	return 0;
}
