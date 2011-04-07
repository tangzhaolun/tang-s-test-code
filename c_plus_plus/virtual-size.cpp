#include <iostream>

class NoVirtual {
    int a;
public:
    void x() const {}
    int y() const { return 1;}
};

class NoIntNoVirtual {  //sizeof NoIntNoVirtual is 1
    //char c;   //no effect to sizeof this class
    //unsigned short s; //will change sizeof to 2
public:
    void x() const {}
    //int y() const { return 1;} //no effect to sizeof this class
};

class OneVirtual {
    int a;
public:
    virtual void x() const {}
    int y() const { return 1;}
};

class TwoVirtual {
    int a;
public:
    virtual void x() const {}
    virtual int y() const { return 1;}
};

class InheritedVirtual :public OneVirtual {
public:
    virtual void x() const { 
        int i;
        for (i = 0; i < 5; i++);
    }

    virtual void new_x() const {}
};

int main(){
    std::cout << "int: " << sizeof(int) << std::endl;
    std::cout << "void *: " << sizeof(void *) << std::endl;
    std::cout << "NoVirtual: " << sizeof(NoVirtual) << std::endl;
    std::cout << "OneVirtual: " << sizeof(OneVirtual) << std::endl;
    std::cout << "TwoVirtual: " << sizeof(TwoVirtual) << std::endl;
    std::cout << "NoIntNoVirtual: " << sizeof(NoIntNoVirtual) << std::endl;

    std::cout << "InheritedVirtual: " 
        << sizeof(InheritedVirtual) << std::endl;
}
