/********************************************************************************************************************
 * DESCRIPTION: This file contains implementation for tcp socket.
 * Usage Example is also included at the end.
 ********************************************************************************************************************/

#include <sys/types.h>
#include <linux/unistd.h>
#include <linux/types.h>
#include <linux/sysctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <iostream>
#include <algorithm>
//using namespace std;
#include "ctcpsocket.hpp"

#if 0 	//Old version of tcp client class, which is usable but not recommended
CTCPClientSocket::CTCPClientSocket(void)
{
}

CTCPClientSocket::~CTCPClientSocket(void)
{
}

int32 CTCPClientSocket::create(uint32 port, const char *ipaddr)
{
    mSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (mSocket < 0) {
        ::LOG( kLogErr, kErrSWBug, "Socket failed - %s", ::strerror(errno));
        return errno;
    }

    ::memset(&mSocketAddr, 0, sizeof(mSocketAddr));
    mSocketAddr.sin_family = AF_INET;
    mSocketAddr.sin_addr.s_addr = ::inet_addr(ipaddr);
    mSocketAddr.sin_port = htons(port);

    return 0;
}

int32 CTCPClientSocket::Send(const uint8 *buf, uint32 len)
{
    int ret = ::sendto(mSocket, buf, len, mSocketFlags,
    		reinterpret_cast<struct sockaddr *> (&mSocketAddr), sizeof(mSocketAddr));
    return ret;
}

int32 CTCPClientSocket::Recv(uint8 *buf, uint32 len)
{
    int ret = ::recvfrom(mSocket, buf, len, mSocketFlags,
    		reinterpret_cast<struct sockaddr *> (&mRecvSocketAddr), &mRecvSocketAddrLen);
    return ret;
}
#endif


CTCPServer::CTCPServer (void)
{
	m_sock = -1;
	m_blacklog = 5;	//default blacklog is 5
	m_tv = NULL;	//default timeout is null
}

CTCPServer::~CTCPServer (void)
{
    Shutdown ();
    if (m_tv != NULL)
    	delete m_tv;
}


/********************************************************************************************************************
 * Creates a socket, binds to the specified port and ip address, and listens for socket connection.
 *
 * \param address Defines the ip address to bind the socket.
 * \param port Defines the port to bind the socket.
 * \return true on success
 ********************************************************************************************************************/
bool CTCPServer::Create(const char *address, unsigned int port)
{
	if (m_sock != -1)
		return false;

	m_sock = socket (AF_INET, SOCK_STREAM, 0);

	if (m_sock < 0) {
		m_sock = -1;
		return false;
	}

	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = inet_addr (address);
	m_addr.sin_port = htons (port);

	int ret = bind (m_sock, reinterpret_cast<struct sockaddr *> (&m_addr), sizeof(m_addr));
	if (ret != 0) {
		std::cout << "Server bind Failed." << std::endl;
		close (m_sock);
		m_sock = -1;
		return false;
	}

	ret = listen (m_sock, m_blacklog);
	if (ret != 0) {
		std::cout << "Server listen Failed." << std::endl;
		close (m_sock);
		m_sock = -1;
		return false;
	}

	std::cout << "Server create is OK." << std::endl;
	return true;
}


/********************************************************************************************************************
 * Accepts a new connection on a server socket.
 *
 * \return pointer to CTCPConnection Instance
 ********************************************************************************************************************/
CTCPConnection *CTCPServer::Accept()
{
	struct sockaddr_in addr;	//address of client
	socklen_t len = sizeof (addr);
	int fd = accept (m_sock, (struct sockaddr *)&addr, &len);

	if (fd < 0) {
		std::cout << "Server accept Failed" << std::endl;
		return NULL;
        }

	std::cout << "Server accept is OK." << std::endl;
	return new CTCPConnection (fd, &addr);
}


void CTCPServer::SetBlacklog (int32 blacklog)
{
	m_blacklog = blacklog;
}

/********************************************************************************************************************
 * Sets the timeout values. If this function is not called,
 * there is no timeout for select(), and select will block indefinitely.
 *
 * \param sec Defines the seconds
 * \param usec Defines the microseconds
 ********************************************************************************************************************/
void CTCPServer::SetTimeout (int32 sec, int32 usec) {
	if (m_tv == NULL) {
		m_tv = new struct timeval;
	}
	m_tv->tv_sec = sec;
	m_tv->tv_usec = usec;
}

void CTCPServer::Shutdown () {
    if (m_sock == -1)
        return;
    shutdown (m_sock, SHUT_RDWR);
    close (m_sock);
    m_sock = -1;
}

void CTCPServer::SetListener (CTCPServerListener *listener)
{
	m_listener = listener;
}

/********************************************************************************************************************
 * This methods calls listener's virtual function inside, so you MUST implement the listener.
 * Why and What to implement onNewConnection?
 * 		- To verify the legitimacy of new connection, if illegal, do sth. here like close and report.
 * 		- To define additional work that accept syscal can't do, like analysis and log.
 * 		- To do any other job about the new connection.
 *
 * \param connection Defines the newly created connection by server accept function
 * \return true on success
 ********************************************************************************************************************/
bool CTCPServer::OnNewConnection(CTCPConnection *connection)
{
	if (m_listener != NULL)
		return m_listener->OnNewConnection(this, connection);
	return false;
}

/********************************************************************************************************************
 * This methods calls listener's virtual function inside, so you MUST implement the listener.
 * Why and What to implement onData?
 * 		- To send or receive data over socket.
 * 		- To shutdown the server loop.
 * 		- To authenticate the integrity of data received or sent over socket.
 * 		- To process data, like analysis and log.
 *
 * \param connection Defines the newly created connection by server accept function
 * \return true on success
 ********************************************************************************************************************/
bool CTCPServer::OnData(CTCPConnection *connection)
{
	if (m_listener != NULL)
		return m_listener->OnData (this, connection);

	return false;
}

/********************************************************************************************************************
 * This methods calls listener's virtual function inside, so you MUST implement the listener.
 * Why and What to implement onDisconnect?
 * 		- To do any job after connection is closed or failed, like report.
 *
 ********************************************************************************************************************/
void CTCPServer::OnDisconnect(CTCPConnection *connection)
{
	if (m_listener != NULL)
		m_listener->OnDisconnect (this, connection);
}


/********************************************************************************************************************
 * Defines main loop to handle multiple connections on a server socket.
 ********************************************************************************************************************/
void CTCPServer::Run(void)
{
	fd_set rfds;
	FD_ZERO (&rfds);

	int fdmax = m_sock;	//keep track of the biggest fd
	FD_SET (m_sock, &rfds);	//add listener fd into fd set

	while (m_sock != -1) {
		fd_set tmpfds = rfds; //backup fd set because select will intervene it

		int ret = select (fdmax + 1, &tmpfds, NULL, NULL, m_tv);

		if (ret == -1) {
			std::cout << "Server select failed." << std::endl;
			break;
		}
		if (ret == 0) {
			std::cout << "Select Timeout." << std::endl;
			continue;
		}

		//std::cout << "Select returns availabe fd number: " << ret << std::endl;
		//handle all connections after select wakes up
		int i = 0;
		for (i = 0; i <= fdmax; i++) {
			if (!FD_ISSET (i, &tmpfds))
				continue;
			if (i == m_sock) {
				// if it's listener fd, accept new connections here
				CTCPConnection *connection = Accept ();
				if (connection == NULL) {
					continue;
				}
				//check new connection here
				if (OnNewConnection (connection) == false) { //new connection is not desirable
					OnDisconnect (connection);
					delete connection;
				}
				else {	// if new connection is OK, add to the map and fd set
					m_connections[connection->GetSock()] = connection;
					FD_SET (connection->GetSock(), &rfds);
					if (connection->GetSock() > fdmax)
						fdmax = connection->GetSock(); //keep track of the maximum fd

					std::cout << "Sock " << i <<" new connection from "
                        << connection->GetAddr() << std::endl;
				}
				continue;
			}

			CTCPConnection * connection = m_connections[i];
			assert (connection != NULL);
			//process and check data on this connection
			std::cout << "Sock " << i << " data processed." << std::endl;
			if (OnData (connection) == false) { //data-check is not good
				FD_CLR (i, &rfds);
				m_connections.erase(i);
				OnDisconnect (connection);
				delete connection;
                
                if (i == fdmax) {
                    fdmax = m_sock;
                    std::map<int, CTCPConnection *>::iterator it;
                    for (it = m_connections.begin(); it != m_connections.end(); it++) {
                        if ((*it).first > fdmax)
                            fdmax = (*it).first;
                    }
                }
            }

		} //end of connection handling for loop
	} //end of main while loop

    //free memory hold by the map, and call OnDisconnect to do relative work
    std::map<int, CTCPConnection *>::iterator it;
    for (it = m_connections.begin(); it != m_connections.end(); it++) {
        OnDisconnect((*it).second);
        delete (*it).second;
    }
    m_connections.clear();

}


CTCPConnection::CTCPConnection()
	: m_sock (-1)
{
}

CTCPConnection::CTCPConnection(int sock, struct sockaddr_in *addr)
	: m_sock (sock),
	  m_addr (*addr)
{
}

CTCPConnection::~CTCPConnection()
{
    Close ();
}


/********************************************************************************************************************
 * Builds connections for tcp socket.
 *
 * \param addr Defines the address of connected peer (client)
 * \param port Defines the port of communication
 * \return true on success
 ********************************************************************************************************************/
bool CTCPConnection::Connect(const char *addr, unsigned int port)
{
	if (m_sock != -1)
		return false;

	m_sock = socket (AF_INET, SOCK_STREAM, 0);

	if (m_sock < 0) {
		m_sock = -1;
		return false;
	}

	m_addr.sin_family = AF_INET;
	m_addr.sin_addr.s_addr = inet_addr (addr);
	m_addr.sin_port = htons (port);

	int ret = connect (m_sock, reinterpret_cast<struct sockaddr *> (&m_addr), sizeof(m_addr));
	if (ret != 0) {
		std::cout << "Connection Failed." << std::endl;
		close (m_sock);
		m_sock = -1;
		return false;
	}

	std::cout << "Connection is OK." << std::endl;
	return true;
}

/********************************************************************************************************************
 * Receives data from a tcp connection.
 *
 * \param buf Defines the buffer to store transferred data
 * \param len Defines the length of buffer
 * \param flags	Defines the buffer parameters with default value of 0
 * \return -1 if error, 0 if peer performed shutdown, or byte size of successfully transfered data
 ********************************************************************************************************************/
size_t CTCPConnection::Recv (void *buf, size_t len, int flags)
{
	return recv (m_sock, buf, len, 0);
}

/********************************************************************************************************************
 * Sends data to a tcp connection.
 *
 * \param buf Defines the buffer to store transferred data
 * \param len Defines the length of buffer
 * \param flags	Defines the buffer parameters with default value of 0
 * \return -1 if error, or byte size of successfully transfered data
 ********************************************************************************************************************/
size_t CTCPConnection::Send (const void *buf, size_t len, int flags)
{
	return send (m_sock, buf, len, flags);
}

void CTCPConnection::Close (void)
{
    if (m_sock) {
        shutdown (m_sock, SHUT_RDWR);
        close (m_sock);
        m_sock = -1;
        std::cout << "Connection is closed." << std::endl;
    }
}

char * CTCPConnection::GetAddr ()
{
	return inet_ntoa(m_addr.sin_addr);
}


/********************************************************************************************************************
 * EXAMPLE: How to create a tcp server to support multi-connection.
 *
 * Step 1: Write your own listener class to implements CTCPServerListener:
 * 		- overwrite onAccept: define your sending data.
 * 		- overwrite onData: define your receiving data.
 * 		- overwrite onDisconnect: define your disconnecting action.
 *
 * Step 2: In main program, create an instance of your listener, and an instance of CTCPServer.
 * 		- set CTCPServer's listener as your listener.
 * 		- call Create().
 * 		- call Run().
 *
 ********************************************************************************************************************/
#if 0

class MyListener : public CTCPServerListener {
protected:
	bool OnAccept (CTCPServer *server, CTCPConnection *connection);
	bool OnData (CTCPServer *server, CTCPConnection *connection);
	void OnDisconnect (CTCPServer *server, CTCPConnection *connection);
};

bool MyListener::OnAccept (CTCPServer *server, CTCPConnection *connection)
{
        char buf[] = "Hello World!\n";
        std::cout << "new connection accepted" << std::endl;
        connection->Send (buf, sizeof(buf));
	return true;
}

bool MyListener::OnData (CTCPServer *server, CTCPConnection *connection)
{
        char buf[100];
        size_t len = connection->Recv (buf, sizeof(buf) - 1);
        buf[len] = 0;
        std::cout << "Got data " << buf << std::endl;
        if (strncmp(buf, "quit", 4) == 0)
	    connection->Close();
        return true;
}

void MyListener::OnDisconnect (CTCPServer *server, CTCPConnection *connection)
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

#endif

