//#include <libsmscommon/ctcpsocket.hpp>
#include "ctcpsocket.hpp"
#include <unistd.h>

#include <iostream>
using namespace std;

int main()
{
    CTCPConnection con;
    if (con.Connect("127.0.0.1", 8031) == false)
    	return -1;

    std::cout << "client is ready to send..." << std::endl;
    char str[] = "Hello world!";
    int i;
    for (i = 0; i < 5; i++){
        con.Send(str, sizeof (str), 0);
        sleep(2);
    }

    std::cout << "sending finished." << std::endl;
    return 0;
}
