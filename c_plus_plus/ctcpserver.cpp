#include "ctcpsocket.hpp"
#include <iostream>
#include <string.h>

using namespace std;

class MyListener : public CTCPServerListener {
protected:
	bool OnAccept (CTCPServer *server, CTCPConnection *connection);
	bool OnData (CTCPServer *server, CTCPConnection *connection);
	void OnDisconnect (CTCPServer *server, CTCPConnection *connection);
};

bool
MyListener::OnAccept (CTCPServer *server, CTCPConnection *connection)
{
        char buf[] = "Hello World!\n";
        std::cout << "new connection" << std::endl;
        connection->Send (buf, sizeof(buf));
	return true;
}

bool
MyListener::OnData (CTCPServer *server, CTCPConnection *connection)
{
        char buf[100];
        size_t len = connection->Recv (buf, sizeof(buf) - 1);
        buf[len] = 0;
        std::cout << "Got data " << buf << std::endl;
        if (strncmp(buf, "quit", 4) == 0)
	    connection->Close();
        return true;
}

void
MyListener::OnDisconnect (CTCPServer *server, CTCPConnection *connection)
{
        std::cout << "disconnect" << std::endl;
}

int main()
{
	MyListener listener;
	CTCPServer server;
	server.Create("127.0.0.1", 8031);
	server.Listen(5);
	server.SetListener(&listener);
	server.Run();
}




