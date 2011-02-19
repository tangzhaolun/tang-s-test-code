#include <stdio.h>

int strlen_w (char * str)
{
	int i = 0;
	if (str == NULL) return -1;
	if (*str == '\0') return i;
	else
	{
		while (*str++ != '\0')i++;
		return i; //without counting the last terminating char
	}
}

int main()
{
	char str[100];
	while (1)
	{
		puts("Input a string [type q to quit]: ");
		//gets(str);
		scanf("%s", str);
		if (strcmp(str,"q") == 0) break;
		printf("The string length is:%d\n", strlen_w(str));
	}
	return 0;
}
