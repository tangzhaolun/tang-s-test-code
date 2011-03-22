//#include <libsmscommon/ctcpsocket.hpp>
#include "ctcpsocket.hpp"
#include <iostream>
#include <string.h>

using namespace std;

class MyListener : public CTCPServerListener {
protected:
	bool OnNewConnection (CTCPServer *server, CTCPConnection *connection);
	bool OnData (CTCPServer *server, CTCPConnection *connection);
	void OnDisconnect (CTCPServer *server, CTCPConnection *connection);
};

bool MyListener::OnNewConnection (CTCPServer *server, CTCPConnection *connection)
{
	// Do some connection-authentication work here
	std::cout << "New connection is checked." << std::endl;
	return true;
}

bool MyListener::OnData (CTCPServer *server, CTCPConnection *connection)
{
	//Send and receive data here
	//Or do any other data-processing work here
	char buf[100];
	size_t len = connection->Recv (buf, sizeof(buf) - 1);
    buf[len] = 0;
    if (len <= 0) {
        /* connection was closed by peer. */
        return false;
    }

	std::cout << "Got data " << buf << std::endl;
	if (strncmp(buf, "quit", 4) == 0) {
		connection->Close();
        return false;
    }
	if (strncmp(buf, "shutdown", 8) == 0) {
		server->Shutdown();
        return false;
    }
	return true;
}

void MyListener::OnDisconnect (CTCPServer *server, CTCPConnection *connection)
{
	//Do some work here when no connection, like log and report
	std::cout << "Disconnect is done." << std::endl;
}

int main()
{
	MyListener listener;
	CTCPServer server;
	server.SetListener(&listener);
	server.Create("127.0.0.1", 8031);

	server.Run();
}




