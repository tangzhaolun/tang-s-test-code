#include <limits.h>
#include <stdio.h>

int main()
{
	char c;
	for (c = '0'; c <= '9'; c++)
		printf("char = %c, asci = %d\n", c, c);
        printf("null character : %d\n", '\0');
        printf("sizeof int:%d\n", sizeof(int));
        printf("sizeof long int:%d\n", sizeof(long int));
        printf("sizeof long long:%d\n", sizeof(long long));

        /*
	printf("char: %d, %d\n", CHAR_MIN, CHAR_MAX);
	printf("short: %d, %d\n", SHRT_MIN, SHRT_MAX);
	printf("int: %d, %d\n", INT_MIN, INT_MAX);
	printf("long: %ld, %ld\n", LONG_MIN, LONG_MAX);
	*/
	return 0;
}
