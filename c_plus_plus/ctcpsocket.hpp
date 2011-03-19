/**********************************************************************************************************************
 * DESCRIPTION: This file contains the an interface class for sending and receiving TCP/IP packets.
 **********************************************************************************************************************/

#ifndef CTCPSOCKET_HPP_
#define CTCPSOCKET_HPP_

#include <netinet/in.h>
#include <map>


// CTCPServer.hpp
class CTCPServer;
class CTCPConnection;

class CTCPServerListener {
protected:
	friend class CTCPServer;
	virtual bool OnAccept (CTCPServer *server, CTCPConnection *connection) = 0;
	virtual bool OnData (CTCPServer *server, CTCPConnection *connection) = 0;
	virtual void OnDisconnect (CTCPServer *server, CTCPConnection *connection) = 0;
};

class CTCPServer {
public:
	CTCPServer (void);
	~CTCPServer (void);
	bool Create (const char *address, unsigned int port);
	bool Listen (int backlog = 5);
	CTCPConnection *Accept (void);
	void Run ();

	void SetListener (CTCPServerListener *listener) {
		m_listener = listener;
	}

protected:
	virtual bool OnAccept (CTCPConnection *connection);
	virtual bool OnData (CTCPConnection *connection);
	virtual void OnDisconnect (CTCPConnection *connection);

protected:
	int m_sock;
	struct sockaddr_in m_addr;
	CTCPServerListener *m_listener;
	std::map<int, CTCPConnection *> m_connections;
};

class CTCPConnection {
public:
	CTCPConnection (void);
	CTCPConnection (int sock, struct sockaddr_in *addr);
	~CTCPConnection (void);

	bool Connect (const char *address, unsigned int port);
	size_t Recv (void *buf, size_t len, int flags = 0);
	size_t Send (const void *buf, size_t len, int flags = 0);
        void Close (void);
	int sock () { return m_sock; }

protected:
	int m_sock;
	struct sockaddr_in m_addr;
};


#endif   /*CTCPSOCKET_HPP_*/
