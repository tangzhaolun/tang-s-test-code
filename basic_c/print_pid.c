#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("The pid is: %d\n", getpid());
	printf("The parent pid is: %d\n", getppid());
	return 0;
}
