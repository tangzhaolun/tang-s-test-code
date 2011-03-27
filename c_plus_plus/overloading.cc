//#include <iostream>

float f1(int, int);

int f(){}
int f(int i){}
//bool f(){}   //compile erro 
//function overloading is only allowed in argument list, not return value
    
float f1(int i1, int i2)
{
    return i1/3 + i1/i2;
}

int main()
{
    return 0;
}
