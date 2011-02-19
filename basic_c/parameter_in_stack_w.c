#include <stdio.h>

int sum(int a1, int a2, int a3)
{
	int a4 = a1 + a2 + a3;
	printf("a1=%p\na2=%p\na3=%p\na4=%p\n", &a1, &a2, &a3, &a4);
	return 	a4;
}

int main()
{
	int i1 = 11;
	int i2 = 3;
	int i3 = 4;
	int i4 = sum(i1, i2, i3);
	printf("i1=%p\ni2=%p\ni3=%p\ni4=%p\n", &i1, &i2, &i3, &i4);
}
