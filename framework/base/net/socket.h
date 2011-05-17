#ifndef _BASE_SOCKET_H_
#define _BASE_SOCKET_H_

#include <base/net/eventobject.h>
#include <base/net/sockaddr.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <memory>

namespace base {

class Socket
{
private:
	int _fd;

public:
	Socket(int fd=0)
	: _fd(fd)
	{ }

	int GetFd() const { return _fd; }

	void SetFd(int fd) { this->_fd = fd; }

	int Create(int domain, int type, int protocol);

	int Connect(const SockAddress& addr, unsigned int usec_timeout);

	int Connect(const sockaddr* addr, size_t addrlen, unsigned int usec_timeout);

	int Bind(const sockaddr* addr, size_t addrlen);

	int Bind(const SockAddress& addr);

	int Listen(unsigned int backlog);

	int Accept(Socket& client);

	int SetSendTimeout(unsigned int usec_timeout);

	int SetRecvTimeout(unsigned int usec_timeout);

	int SetDelay(bool delay);

	int SetBlocking(bool blocking);

	int GetBlocking(bool& blocking);

	int Send(const char* p, size_t len);

	int Sendn(const char* p, size_t len, unsigned int usec_timeout);

	int Recv(char* p, size_t len);
    
    int Recvn(char*p, size_t len, unsigned int usec_timeout);


	void Close();

	int GetRemoteAddr(SockAddress & stAddr);

	int GetLocalAddr(SockAddress & stAddr);
};

}
#endif

