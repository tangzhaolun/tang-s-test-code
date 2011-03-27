#include <iostream>
#include <exception>

class Base {
    virtual void dummy(){}
};

class Derived: public Base {
    int a;
};

class A {};

int main() {
    try {
        //example of dynamic_cast 
        Base *pba = new Derived;
        Base *pbb = new Base;
        Derived *pd;

        //pd = (Derived *)pba; 
        //Note: above traditional type-casting does no type checking,
        //so type is forced to be converted, which may cause runtime error.
        pd = dynamic_cast <Derived *> (pba);
        if (pd == 0) 
            std::cout << "Dynamic_cast:from derived to derived: Null pointer\n";
        
        //pd = (Derived *)pbb;
        pd = dynamic_cast <Derived *> (pbb);
        if (pd == 0) 
            std::cout << "Dynamic_cast:from base to derived: Null pointer\n";
        //dynamic-cast check pbb points to a not complete object of 
        //the required class (pointed by pd), so it returns null to
        //indicate the failure, which may avoid runtime error 

        
        //example of static_cast
        pd = static_cast <Derived *> (pba);
        if (pd == 0) 
            std::cout << "Static_cast:from derived to derived: Null pointer\n";
        
        pd = static_cast <Derived *> (pba);
        if (pd == 0) 
            std::cout << "Static_cast:from base to derived: Null pointer\n";
        
        A *a = new A;
        //a = static_cast <A *> (pba); //cause compiling error

        //example of reinterpret_cast
        a = reinterpret_cast <A *> (pba);
        //above cast type can be compiled ok, but will cause runtime error
        //reinterpret_cast is similar to traditional type cast, dangerous

    } catch (std::exception& e) {
        std::cout << "Exception :" << e.what() << std::endl;
    }
    return 0;
}
