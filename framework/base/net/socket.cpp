#include <base/net/socket.h>
#include <base/log/log.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <poll.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <cassert>
#include <sys/time.h>


namespace base {

int Socket::Create(int domain, int type, int protocol)
{
	

	_fd = socket(domain, type, protocol);
	if (_fd == -1)
		return -1;
	return 0;
}

int Socket::Connect(const SockAddress& addr, unsigned int usec_timeout)
{
	return Connect(addr.Getsockaddr(), addr.Getsockaddrlen(), usec_timeout);
}

int Socket::Connect(const sockaddr* addr, size_t addrlen, unsigned int usec_timeout)
{
	if (usec_timeout == 0)
	{
		return ::connect(_fd, addr, addrlen);
	}
	else
	{
		bool blocking;
		if (GetBlocking(blocking) == -1)
        {
            LOG_ERROR("get socket block error, errno:[%d], errmsg:[%s]", errno, strerror(errno));
			return -1;
        }

		if (blocking)
		{
			if (SetBlocking(false) == -1)
            {
                LOG_ERROR("set socket block error, errno:[%d], errmsg:[%s]", errno, strerror(errno));
				return -1;
            }
		}

		int ret = ::connect(_fd, addr, addrlen);
		if (ret == -1)
		{
			if (errno == EINPROGRESS)
			{
				struct pollfd stPollFd;
				stPollFd.fd = _fd;
				stPollFd.events = POLLOUT;
				stPollFd.revents = 0;
				int timeout = usec_timeout/1000;

				int ret = poll(&stPollFd, 1, timeout);
				if ( ret < 0 ) 
                {
                    LOG_ERROR("poll error, errno:[%d], errmsg:[%s]", errno, strerror(errno));
					return -1;
				}
				else if ( ret == 0 ) 
                {
					errno = ETIMEDOUT;
                    LOG_ERROR("connect timeout");
					return -1;
				}
				else if ( stPollFd.revents != POLLOUT ) {
					return -1;
				}					
				else {
					int error = -1;
					int len = sizeof(error);
					getsockopt(_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
					if (error != 0) 
                    {
                        LOG_ERROR("getsocket error, errno:[%d], errmsg:[%s]", errno, strerror(errno));
						return -1;
					}
				}
			}
			else
			{
				return -1;
			}
		}

		if (blocking)
		{
			if (SetBlocking(true) == -1)
            {
                LOG_ERROR("set socket block error, errno:[%d], errmsg:[%s]", errno, strerror(errno));
				return -1;
            }
		}

		return 0;
	}
}


int Socket::Bind(const sockaddr* addr, size_t addrlen)
{
	if (_fd == -1)
		return -1;

	int i = 1;
	if(setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&i, sizeof(int)) == -1)
	{
		return -1;
	}

	if (::bind(_fd, addr, addrlen) != 0)
	{
		return -1;
	}

	return 0;
}

int Socket::Bind(const SockAddress& addr)
{
	return Bind(addr.Getsockaddr(), addr.Getsockaddrlen());
}

int Socket::Listen(unsigned int backlog)
{
	return ::listen(_fd, backlog);
}

int Socket::Accept(Socket& client)
{
	sockaddr_in clientaddr;
	socklen_t len = sizeof(clientaddr);

	int s = ::accept(_fd, reinterpret_cast<sockaddr*>(&clientaddr), &len);
	if (s == -1)
		return -1;

	client._fd = s;
	return 0;
}

int Socket::SetSendTimeout(unsigned int usec_timeout)
{
    timeval sendrecvtimeout;
    sendrecvtimeout.tv_sec = usec_timeout/(1000*1000);
    sendrecvtimeout.tv_usec = usec_timeout%1000;

	if (setsockopt(_fd, SOL_SOCKET, SO_SNDTIMEO, (void *)&sendrecvtimeout,sizeof(struct timeval)) != 0)
		return -1;

	return 0;
}

int Socket::SetRecvTimeout(unsigned int usec_timeout)
{
    timeval sendrecvtimeout;
    sendrecvtimeout.tv_sec = usec_timeout/(1000*1000);;
    sendrecvtimeout.tv_usec = usec_timeout%1000;

	if (setsockopt(_fd, SOL_SOCKET, SO_RCVTIMEO, (void *)&sendrecvtimeout,sizeof(struct timeval)) != 0)
		return -1;

	return 0;
}

int Socket::SetDelay(bool delay)
{
	int flags = 0;
	if (!delay)
		flags = 1;

	if (setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &flags, sizeof(flags)) != 0)
		return -1;

	return 0;
}

int Socket::GetBlocking(bool& blocking)
{
	int oldflags = fcntl(_fd, F_GETFL, 0);
	if (oldflags == -1)
	{
		return -1;
	}
	blocking = ((oldflags & O_NONBLOCK) ? false : true);
	return 0;
}

int Socket::SetBlocking(bool blocking)
{
	int flags = fcntl(_fd, F_GETFL, 0);
	if (flags == -1)
	{
		return -1;
	}

	if (blocking)
		flags &= ~O_NONBLOCK;
	else
		flags |= O_NONBLOCK;

	if (fcntl(_fd, F_SETFL, flags) != 0)
	{
		return -1;
	}

	return 0;
}

int Socket::Send(const char* p, size_t len)
{
    int ret = ::send(_fd, p, len, 0);
    if (-1 == ret)
    {
        return -1;
    }
    return ret;
    
}

int Socket::SendN(const char* p, size_t len, unsigned int /*usec_timeout*/)
{
    size_t offset = 0 ;
    while (offset < len)
    {
        int ret = Send(p+offset, len-offset);
        if (ret <= 0)
        {
            return ret;
        }
        offset += ret;
    }
    assert(offset == len);
    return offset;    
}

int Socket::Recv(char*p, size_t len)
{
    int ret = ::recv(_fd, p, len, 0);
    if (ret == -1)
    {
        return ret;
    }
    return ret;   
}

int Socket::RecvN(char*p, size_t len, unsigned int /*usec_timeout*/)
{
    size_t offset = 0 ;
    while (offset < len)
    {
        int ret = Recv(p+offset, len-offset);
        if (ret <= 0)
        {
            return ret;
        }
        offset += ret;
    }
    assert(offset == len);
    return offset;    
}

void Socket::Close()
{
	if ( _fd != -1) {
		::close(_fd);
		_fd = -1;
	}
}

int Socket::GetRemoteAddr(SockAddress & stAddr)
{
	union {
		sockaddr generic;
		sockaddr_in  in_4;
		sockaddr_in6 in_6;
		sockaddr_un unix_domain;
	} Addr;

	memset(&Addr, 0, sizeof(Addr));

	socklen_t addrlen = sizeof(Addr);

	if ( getpeername(_fd, &Addr.generic, &addrlen) != 0 ) {
		return -1;
	}

	stAddr.Setsockaddr((sockaddr*)&Addr);
	return 0;
}

int Socket::GetLocalAddr(SockAddress & stAddr)
{
	union {
		sockaddr generic;
		sockaddr_in  in_4;
		sockaddr_in6 in_6;
		sockaddr_un unix_domain;
	} Addr;

	memset(&Addr, 0, sizeof(Addr));

	socklen_t addrlen = sizeof(Addr);

	if ( getsockname(_fd, &Addr.generic, &addrlen) != 0 ) {
		return -1;
	}

	stAddr.Setsockaddr((sockaddr*)&Addr);
	return 0;
}

}
