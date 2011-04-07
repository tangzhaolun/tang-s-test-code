#include <iostream>
#include <string>

class Pet {
public:
    virtual std::string speak () = 0;
};

std::string Pet::speak() { return "Pet";}

class Dog: public Pet {
public:
    virtual std::string speak () {return Pet::speak();}
};

int main(){
    Dog jack;
    Pet *p1 = &jack;
    Pet &p2 = jack;

    std::cout << "p1->speak : " << p1->speak() << std::endl;
    std::cout << "p2.speak : " << p2.speak() << std::endl;

}
