#include <iostream>
using namespace std;

class A
{
public:
  A()
  {
    cout<<"constructor A"<<endl;
  }

  ~A()
  {
    cout<<"destructor A\n";
  }
};

class B: public A
{
public:
//private: 
//B has son-class C, so B's constructor can't be private,
//only public or protected is allowed.
  B()
  {
    cout<<"constructor B\n";
  }

  ~B()
  {
    cout<<"destructor B\n";
  }
};


class C: public B
{
public:
  C()
  {
    cout<<"constructor C\n";
  }

  ~C()
  {
    cout<<"destructor C\n";
  }
};

int main(void)
{
  C *c = new C();
  delete c;
  return 0;
}
