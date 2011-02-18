#include <stdio.h>
#include <string.h>

/*Define my own strcpy function. */
char *strcpy_w (char *, const char *);

/*Before call strcpy_w function, make sure that dest points enough allocation already.*/
char *strcpy_w (char *dest, const char *src)
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
	//char str1[10], str2[10];
	char str1[10];
	char str2[10] = "123456";
	/*char a[2] = {'a', 'b'};
	char b[2] = "cd";
	printf("a=%s, b=%s\n",a,b);
	printf("length of a = %d, length of b= %d\n", strlen(a), strlen(b));
	*/
	puts("Input source string: ");
	scanf("%10s", str2);
	printf("str1=%s\nstr2=%s\n", str1, str2);
	printf("Copy to str1 is: %s \n", strcpy_w(str1,str2));
} 
