#include <string>
#include <iostream>

int main(){
    std::string str1("Hello");
    std::string::iterator it;
    for (it = str1.begin(); it < str1.end(); it++)
       std::cout << *it;
    std::cout << '\n'; 

    return 0;
}
