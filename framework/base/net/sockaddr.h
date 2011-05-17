#ifndef _BASE_SOCKADDR_H_
#define _BASE_SOCKADDR_H_

#include <ostream>
#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

namespace base {

class SockAddress
{
public:
	SockAddress();

	SockAddress(const sockaddr * pSockaddr);

	void Setsockaddr(const sockaddr * pSockaddr);
    int  SetAddr(const std::string& ip, int port);
	int  SetAddr(const char* addr);


	inline const struct sockaddr * Getsockaddr() const { return &_addr.generic; }

	inline socklen_t Getsockaddrlen() const	{ return _len; }

	inline int GetFamily() const { return _addr.generic.sa_family; }

	const std::string & ToString() const { return _string; }

    std::string GetHost();
    unsigned short GetPort();


private:
	std::string _string;
	socklen_t _len;

	union {
		sockaddr generic;
		sockaddr_in  in;
		sockaddr_un un;
	} _addr;
};
}

#endif
