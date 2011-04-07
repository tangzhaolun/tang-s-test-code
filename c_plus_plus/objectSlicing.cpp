// this file is to demonstrate object slicing, and
// how to prevent it by using pure virutal function
// or to pass object by pointer or reference, not by value

#include <iostream>
#include <string>

class Pet {
public:
    virtual std::string speak ();
};

std::string Pet::speak() { return "Pet";}

class Dog: public Pet {
public:
    virtual std::string speak () {return "Dog";}
};

//void voice (Pet &p) //ok to provide polymorphism, so does pointer 
void voice (Pet p){//cause object slicing
    std::cout << p.speak() << std::endl;
}

int main(){
    Pet p;
    Dog jack;

    voice(p);
    voice(jack);

}
