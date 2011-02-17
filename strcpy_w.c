#include <stdio.h>


char *strcpy (char *, char *);

char *strcpy (char *dest, char *src)
{
	if (src == 0) return NULL;
	char *tmp = dest;
	while (*src != '\0')
		*tmp++ = *src++;
	*tmp = '\0';
	return dest;
}

void main()
{
	char str1[10], *str2 = "Hello";
	printf("Source string is: %s \n ", str2);
	printf("Copy string is: %s \n", strcpy(str1,str2));
} 
