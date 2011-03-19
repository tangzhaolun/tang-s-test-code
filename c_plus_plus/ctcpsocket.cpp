/**********************************************************************************************************************
 * DESCRIPTION: This file contains the an interface class for sending and receiving TCP/IP packets.
**********************************************************************************************************************/

#include "ctcpsocket.hpp"

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

#define LOG(a,b,args...) fprintf (stderr, ##args)

/********************************************************************************************************************
 * Creates a client socket and builds connection to server.
 *
 * \param port Defines the port to bind the socket.
 * \param ipaddr Defines the client ip address.
 *
 * \return Socket error code, 0 for success
 ********************************************************************************************************************/
CTCPServer::CTCPServer (void)
	: m_sock (-1)
{
}

CTCPServer::~CTCPServer (void)
{
    if (m_sock >= 0)
        close (m_sock);
}

bool
CTCPServer::Create(const char *address, unsigned int port)
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
		::LOG( kLogErr, kErrSWBug, "Server bind Failed - %s", ::strerror(errno));
		close (m_sock);
		m_sock = -1;
		return false;
	}

	return true;
}

bool
CTCPServer::Listen (int blacklog)
{
	int ret = listen (m_sock, blacklog);
	return (ret == 0);
}

CTCPConnection *
CTCPServer::Accept()
{
	struct sockaddr_in addr;
	socklen_t len = sizeof (addr);
	int fd = accept (m_sock, (struct sockaddr *)&addr, &len);

	if (fd < 0) {
            perror ("accept()");
		return NULL;
        }

	return new CTCPConnection (fd, &addr);
}

bool
CTCPServer::OnAccept(CTCPConnection *connection)
{
	if (m_listener != NULL)
		return m_listener->OnAccept(this, connection);
	return false;
}

bool
CTCPServer::OnData(CTCPConnection *connection)
{
	if (m_listener != NULL)
		return m_listener->OnData (this, connection);

	return false;
}

void
CTCPServer::OnDisconnect(CTCPConnection *connection)
{
	if (m_listener != NULL)
		m_listener->OnDisconnect (this, connection);
}

void
CTCPServer::Run(void)
{
	while (1) {
		fd_set rfds;
		FD_ZERO (&rfds);
		
                int nfds = m_sock;
		FD_SET (m_sock, &rfds);
		
                std::map<int, CTCPConnection *>::iterator it;
		for (it = m_connections.begin(); it != m_connections.end(); it++) {
			int sock = (*it).first;
                        CTCPConnection *connection = (*it).second;
                        if (connection->sock() != sock) {
                            /* closed */
                            OnDisconnect (connection);
                            m_connections.erase(sock);
                            delete connection;
                            continue;
                        }
			FD_SET (sock, &rfds);
			if (sock > nfds)
				nfds = sock;
		}

		int ret = select (nfds + 1, &rfds, NULL, NULL, NULL);

		if (ret == -1) {
			perror ("select()");
			break;
		}
		if (ret == 0) {
                        std::cout << "Timeout" << std::endl;
			// timeout
			continue;
		}

		int i = 0;
		for (i = 0; i <= nfds; i++) {
			if (!FD_ISSET (i, &rfds))
				continue;
			if (i == m_sock) {
				CTCPConnection *connection = Accept ();
				if (connection == NULL) {
					// accept failed
					continue;
				}
				if (!OnAccept (connection)) {
					// refused the connection
                                        OnDisconnect (connection);
					delete connection;
				}
				else {
					// accepted the connection
					m_connections[connection->sock()] = connection;
				}
				continue;
			}

			CTCPConnection * connection = m_connections[i];
			assert (connection != NULL);
			if (OnData (connection) == false) {
				// remove connection
				m_connections.erase(i);
				OnDisconnect (connection);
				delete connection;
			}
		}
	}
}



// ===========================
// CTCPConnection
// ===========================
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
    if (m_sock >= 0) 
        close (m_sock);
}

bool
CTCPConnection::Connect(const char *addr, unsigned int port)
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
		::LOG( kLogErr, kErrSWBug, "CLient connect Failed - %s", ::strerror(errno));
		close (m_sock);
		m_sock = -1;
		return false;
	}

	return true;
}

size_t
CTCPConnection::Recv (void *buf, size_t len, int flags)
{
	return recv (m_sock, buf, len, 0);
}

size_t
CTCPConnection::Send (const void *buf, size_t len, int flags)
{
	return send (m_sock, buf, len, flags);
}

void
CTCPConnection::Close (void)
{
    if (m_sock) {
        close (m_sock);
        m_sock = -1;
    }
}


#if 1

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
	server.Create("0.0.0.0", 8031);
	server.Listen(5);
	server.SetListener(&listener);
	server.Run();
}

#endif



#if 0
int32 CTCPClientSocket::create (uint32 port, const char *ipaddr)
{
	mSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (mSocket < 0)
	{
        ::LOG( kLogErr, kErrSWBug, "TCP Socket failed - %s", ::strerror(errno));
        return errno;
    }

    ::memset(&mSocketAddr, 0, sizeof(mSocketAddr));
//////////    mSocketAddr.sin_family = AF_INET;
//////////    mSocketAddr.sin_addr.s_addr = ::inet_addr(ipaddr);
//////////    mSocketAddr.sin_port = htons(port);
//////////
//////////	int ret = ::connect(mSocket, (struct sockaddr *)&mSocketAddr, sizeof mSocketAddr);
//////////	if (ret < 0)
//////////	{
//////////		::LOG( kLogErr, kErrSWBug, "Build connection failed - %s", ::strerror(errno));
//////////		return errno;
//////////	}
//////////
//////////    return 0;
//////////}
//////////
//////////
///////////********************************************************************************************************************
////////// * Sends data to the specified destination.
////////// *
////////// * \param buf Pointer to data buffer - supplied by the caller
////////// * \param len Number of bytes to be sent
////////// * \return Number of bytes sent, -1 for error
////////// ********************************************************************************************************************/
//////////int32 CTCPClientSocket::sendData (const uint8 *buf, uint32 len)
//////////{
//////////    int bytes = ::send(mSocket, buf, len, mSocketFlags);
//////////    if (bytes < 0)
//////////    {
//////////		::LOG( kLogErr, kErrSWBug, "Send data failed.\n");
//////////    }
//////////    return bytes;
//////////}
//////////
///////////********************************************************************************************************************
////////// * Receives data to the specified destination.
////////// *
////////// * \param buf Pointer to data buffer - supplied by the caller
////////// * \param len Number of bytes to be received
////////// * \return Number of bytes sent, -1 for error
////////// ********************************************************************************************************************/
//////////int32 CTCPClientSocket::recvData(uint8 *buf, uint32 len)
//////////{
//////////    int bytes = ::recv(mSocket, buf, len, mSocketFlags);
//////////    if (bytes < 0)
//////////    {
//////////		::LOG( kLogErr, kErrSWBug, "Receive data failed.\n");
//////////    }
//////////    return bytes;
//////////}
//////////
//////////
//////////CTCPServerSocket::CTCPServerSocket (void)
//////////{
//////////	connectFD = -1;
//////////}
//////////
//////////CTCPServerSocket::~CTCPServerSocket (void)
//////////{
//////////	::close(connectFD);
//////////}
//////////
//////////
/////////////********************************************************************************************************************
//////////// * Creates a tcp server socket. Builds connection and be ready to read or write data.
//////////// *
//////////// * \param port Defines the port to bind the socket.
//////////// * \param ipaddr Defines the ip address to bind the socket.
//////////// *
//////////// * \return Socket error code
//////////// ********************************************************************************************************************/
////////////int32 CTCPServerSocket::create (uint32 port, const char *ipaddr)
////////////{
////////////    mSocket = ::socket(AF_INET, SOCK_STREAM, 0);
////////////    if (mSocket < 0)
////////////    {
////////////        ::LOG( kLogErr, kErrSWBug, "Server socket failed - %s", ::strerror(errno));
////////////        return errno;
////////////    }
////////////
////////////    ::memset(&mSocketAddr, 0, sizeof(mSocketAddr));
////////////    mSocketAddr.sin_family = AF_INET;
////////////    mSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY);
////////////    mSocketAddr.sin_port = htons(port);
////////////
////////////    int ret = ::bind(mSocket, reinterpret_cast<struct sockaddr *> (&mSocketAddr), sizeof(mSocketAddr));
////////////    if (ret != 0)
////////////    {
////////////        ::LOG( kLogErr, kErrSWBug, "Server bind Failed - %s", ::strerror(errno));
////////////        return errno;
////////////    }
////////////
////////////    ret = ::listen (mSocket, 5);
////////////	if (ret == -1)
////////////	{
////////////        ::LOG( kLogErr, kErrSWBug, "Server listen Failed - %s", ::strerror(errno));
////////////        return errno;
////////////	}
////////////
////////////	connectFD = ::accept (mSocket, NULL, NULL);
////////////	if (connectFD < 0)
////////////	{
////////////        ::LOG( kLogErr, kErrSWBug, "Server accept Failed - %s", ::strerror(errno));
////////////        return errno;
////////////	}
////////////
////////////    return 0;
////////////}
////////////
////////////
////////////int32 CTCPServerSocket::sendData (const uint8 *buf, uint32 len)
////////////{
////////////    int bytes = ::send(connectFD, buf, len, mSocketFlags);
////////////    if (bytes < 0)
////////////    {
////////////		::LOG( kLogErr, kErrSWBug, "Send data failed.\n");
////////////    }
////////////    return bytes;
////////////}
////////////
////////////int32 CTCPServerSocket::recvData (uint8 *buf, uint32 len)
{
    int bytes = ::recv(connectFD, buf, len, mSocketFlags);
    if (bytes < 0)
    {
		::LOG( kLogErr, kErrSWBug, "Receive data failed.\n");
    }
    return bytes;
}

#endif
