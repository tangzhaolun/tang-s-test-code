#include <iostream>

class Base {
public:
    //void f() { //without virtual, up casting calls just from base class
    virtual void f() {
        std::cout << "base::f()" << std::endl;
    }
};

class Derived: public Base {
public:
    /*int f() { //error to overload on return type
        std::cout << "int f()" << std::endl;
    }*/

    void f(int i) {
        std::cout << "f(int)" << std::endl;
    }

    void f() {
        std::cout << "unmodified f()" << std::endl;
    }
};

int main(){
    Derived d;
    Base *b = &d;
    b->f();
    //b->f(4);
}
