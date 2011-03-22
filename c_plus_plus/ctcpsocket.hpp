/********************************************************************************************************************
 * DESCRIPTION: This file contains classes and interfaces for tcp socket.
 * Three Classes:CTCPServer, CTCPConnection, CTCPServerListener.
 * They work together for a TCP Server Socket, supporting multiple connections.
 * CTCPConnection can serve as TCP client.
 * Please refer to Usage Example in ctcpsocket.cpp file
 ********************************************************************************************************************/

#ifndef CTCPSOCKET_HPP_
#define CTCPSOCKET_HPP_

#include <netinet/in.h>
#include <map>

//#include "cdsTypes.h"
typedef signed int int32;
//#include "cudpsocket.hpp"

#if 0	//Old version of tcp client class, which is usable but not recommended
/********************************************************************************************************************
 * Class CTCPClient inherits from UDP socket and provides necessary functions of a TCP Client Socket.
 ********************************************************************************************************************/
class CTCPClient: public CUDPClientSocket {
public:
    CTCPClient(void);
    virtual ~CTCPClient(void);

    virtual int32 create(uint32 port, const char *ipaddr);

    int32 Send(const uint8 *buf, uint32 len);
    int32 Recv(uint8 *buf, uint32 len);
};
#endif


class CTCPServer;
class CTCPConnection;

class CTCPServerListener {
protected:
	friend class CTCPServer;
	virtual bool OnNewConnection (CTCPServer *server, CTCPConnection *connection) = 0;
	virtual bool OnData (CTCPServer *server, CTCPConnection *connection) = 0;
	virtual void OnDisconnect (CTCPServer *server, CTCPConnection *connection) = 0;
};


class CTCPServer {
public:
	CTCPServer (void);
	~CTCPServer (void);
	bool Create (const char *address, unsigned int port);
	CTCPConnection *Accept (void);
	void Run ();

    void Shutdown();
	void SetListener (CTCPServerListener *listener);
	void SetTimeout (int32 sec, int32 usec);
	void SetBlacklog (int32 blacklog);

protected:
	virtual bool OnNewConnection (CTCPConnection *connection);
	virtual bool OnData (CTCPConnection *connection);
	virtual void OnDisconnect (CTCPConnection *connection);

protected:
	int m_sock;
	struct sockaddr_in m_addr;
	struct timeval *m_tv;
	int m_blacklog;	//the maximum length the queue of pending connections
	CTCPServerListener *m_listener;

	// map, key is socket file descriptor, value is correspondant connection
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
	int GetSock () { return m_sock; }
	char * GetAddr ();

protected:
	int m_sock;
	struct sockaddr_in m_addr;
};


#endif   /*CTCPSOCKET_HPP_*/
