#include <iostream>
#include <string>

class Pet {
public:
    virtual std::string speak (){ return "Pet";}
};

class Dog: public Pet {
public:
    virtual std::string speak () {return "Dog";}
};

int main(){
    Dog jack;
    Pet *p1 = &jack;
    Pet &p2 = jack;
    Pet p3;

    //late binding for both
    std::cout << "p1->speak : " << p1->speak() << std::endl;
    std::cout << "p2.speak : " << p2.speak() << std::endl;

    //early binding
    std::cout << "p3.speak : " << p3.speak() << std::endl;
}
