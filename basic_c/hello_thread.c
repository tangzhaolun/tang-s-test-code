#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* print_msg(void *data)
{
	int i;
	for(i = 0; i < 5; i++)
		printf("Hello: %s - %d\n",(char *)data, i);
	return NULL;
}

void* add(void *data)
{
	int i = ((int*)data)[0];
	int j = ((int*)data)[1];
	int sum = i + j;
	return (void *)sum;
}

int main(int argc, char **argv)
{
	int thread_count;
	char *endptr;

	if(argc > 1)
		thread_count = strtoul(argv[1], &endptr, 0);
	else
		thread_count = 1;

	pthread_t thread_id[thread_count];
	int i;

	for(i = 0; i < thread_count; i++){
		char *ii = malloc(10);
		/* char ii[10];*/
		/* to avoid threads over-write each other's ii variable, 
		 * use malloc here instead of ii[10],
		 * but leaves a potential bug because free is not called*/
		sprintf(ii,"%d",i);
    		pthread_create(&thread_id[i], NULL, &print_msg, ii);
	}

	for(i = 0; i < 5; i++)
		printf("Hello: %s %d\n", argv[0], i);

	for(i = 0; i < thread_count; i++) 
		pthread_join(thread_id[i], NULL);


	pthread_t thread_id2;
	int data[2] = {1,5};
	int rev;
	pthread_create(&thread_id2, NULL, &add, data);
	
	pthread_join(thread_id2, (void*) &rev);
	printf("The add result: %d\n", rev);

	return 0;
}
